#include "devicedialog.h"
#include "ui_devicedialog.h"
#include "plotzoomdialog.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <cmath>   // std::ceil
#include <QtGlobal> // qBound
/*********************************************************************************************************************
 *                                                  HELPERS
**********************************************************************************************************************/
/* Helper che serve per non far crescere i dati all’infinito (memoria).
- limita in base al tempo (rimuove punti più vecchi di tMin)
*/
static void trimOld(QCustomPlot* p, double tNow, double winSec, double factor)
{
    // factor tipico:
    //  - 1.2 in live (taglio più aggressivo) → conservi ~12 s ma se la telemetria è molto rapida, in 12s possono essere contenuti tantissimi punti
    //  - 3.0 quando l'utente esplora (taglio più conservativo) → conservi ~30 s ma se la telemetria è molto rapida, in 30s possono essere ancora più punti
    const double tMin = tNow - winSec * factor;
    for(int i=0; i<p->graphCount(); ++i)
        p->graph(i)->data()->removeBefore(tMin);
}

/* è il guard-rail: dice “anche se per qualche motivo (ad es. telemetria troppo alta) non ho tagliato
 *  abbastanza per tempo, non voglio superare maxPts punti per curva*/
static void limitMaxPoints(QCustomPlot* p, int maxPts)
{
    for(int gi=0; gi<p->graphCount(); ++gi)
    {
        QCPGraph* g = p->graph(gi);
        if(!g) continue;

        auto d = g->data();
        if(!d) continue;

        const int sz = d->size();
        if(sz <= maxPts) continue;

        // Tieni gli ultimi maxPts punti -> rimuoviamo tutto prima del punto (sz - maxPts)-esimo.
        const int drop = sz - maxPts;
        auto itKeep = d->constBegin();
        for(int k=0; k<drop; ++k) ++itKeep;
        const double keyKeep = itKeep->key;
        d->removeBefore(keyKeep);   // Rimuove tutti i punti con key < keyKeep
    }
}

/* calcolo maxPts in modo coerente con:
- finestra visibile _windowSec
- frequenza dei sample per curva (Hz)
- un fattore di margine*/
static inline int computeMaxPts(double windowSec,   // punti necessari per _windowSec
                                double fCurveHz,    // punti/sec per una singola curva
                                double keepFactor,   // es. 1.2 live, 3.0 explore
                                double margin = 1.5, // jitter/batch/overshoot
                                int minPts = 400,
                                int maxPtsHard = 20000)
{
    if (windowSec <= 0.0 || fCurveHz <= 0.0)
        return minPts;

    const double raw = windowSec * fCurveHz * keepFactor * margin;
    const int pts = int(std::ceil(raw));

    return qBound(minPts, pts, maxPtsHard);
}

static void autoscaleYVisible(QCustomPlot* p, double xMin, double xMax)
{
    double ymin =  1e300;
    double ymax = -1e300;

    for(int gi=0; gi<p->graphCount(); ++gi){
        auto* g = p->graph(gi);
        if(!g) continue;
        if(!g->visible()) continue; // Serve per non fare l'autoscale sull'asse Y relativo alle curve nascoste

        auto data = g->data();
        auto it = data->findBegin(xMin, true);

        for(; it != data->constEnd() && it->key <= xMax; ++it){
            ymin = std::min(ymin, it->value);
            ymax = std::max(ymax, it->value);
        }
    }

    if(ymin > ymax) return; // nessun dato

    double m = 0.10 * (ymax - ymin);
    if(m == 0) m = 1.0; // caso linea piatta (Voltage)

    p->yAxis->setRange(ymin - m, ymax + m);
}

static inline QList<QCustomPlot*> allPlots(Ui::DeviceDialog* ui)
{
    return { ui->plotDuty, ui->plotCurrents, ui->plotVoltages, ui->plotPosition };
}

static QToolButton* makeLegendToggle(const QColor& c, const QString& text, QWidget* parent) // Crea una "mini-legend" interattiva
{
    auto* b = new QToolButton(parent);
    b->setCheckable(true);
    b->setChecked(true);
    b->setAutoRaise(true);
    b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    b->setText(text);

    QPixmap pm(10,10);
    pm.fill(c);
    b->setIcon(QIcon(pm));

    // stile: quando OFF testo più grigio
    b->setStyleSheet(
        "QToolButton{"
        "  color:#202020;"
        "  padding:2px 6px;"
        "  background-color:#f0f0f0;"   // grigio chiaro
        "  border:1px solid #d0d0d0;"   // opzionale
        "  border-radius:3px;"         // opzionale
        "}"
        "QToolButton:hover{"
        "  background-color:#e8e8e8;"   // opzionale: hover leggermente più scuro
        "}"
        "QToolButton:checked{ font-weight:600; }"
        "QToolButton:!checked{ color:#808080; }"
        );

    return b;
}
/*********************************************************************************************************************
 *                                                  METODI DELLA CLASSE
**********************************************************************************************************************/

DeviceDialog::DeviceDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);
    setWindowTitle("CALM Monitoring");

    // ogni plot viene configurato ugualmente (2 curve + assi) e cambia solo la label Y
    setupPlot(ui->plotDuty,     "Duty [%]");
    setupPlot(ui->plotCurrents, "Current [mA]");
    setupPlot(ui->plotVoltages, "Voltage [V]");
    setupPlot(ui->plotPosition, "Position [deg]");

    #ifndef MCU_SIM_RUN
    // Avvia clock per tempo "continuo" (asse X fluido)
    _wall.start();
    _lastWallMs = _wall.elapsed();

    // ===== Timer FAST: 16ms solo scroll X + replot =====
    _timerFast.setInterval(16);
    _timerFast.setTimerType(Qt::PreciseTimer);

    connect(&_timerFast, &QTimer::timeout, this, [this]{
        const double t = tDisplayNow();

        // Autoscroll X SOLO se non c'è override utente
        for(QCustomPlot* p : allPlots(ui))
        {
            // qui facciamo SOLO autoscroll in X (leggero)
            const bool override =
                _plotState.contains(p) ? _plotState[p].userOverride : false;

            if(!override)
                p->xAxis->setRange(t, _windowSec, Qt::AlignRight);
        }

        // Replot queued (non blocca)
        for(QCustomPlot* p : allPlots(ui))
            p->replot(QCustomPlot::rpQueuedReplot);
    });
    _timerFast.start();

    // ===== Timer SLOW: lavoro pesante 6-10 Hz (trim/limit/autoscale)=====
    _timerSlow.setInterval(150);               // 100..200ms consigliato
    _timerSlow.setTimerType(Qt::CoarseTimer);  // precisione non critica

    connect(&_timerSlow, &QTimer::timeout, this, [this]{
        const double t = tDisplayNow();
        const double xMax = t;
        const double xMin = t - _windowSec;

        // Telemetria per curva a 50 Hz (period_ms=20)
        // Idealmente period_ms lo passi da MainWindow o lo tieni come costante configurabile.
        static constexpr int period_ms = 20;        // POSSIBILE MIGLIORIA: aggiungere il tuning di period_ms da GUI
        const double fCurveHz = 1000.0 / period_ms; // POSSIBILE MIGLIORIA: stimiamo fCurveHz misurando realmente la frequenza che arriva (contatori per type/id in finestra mobile)

        for(QCustomPlot* p : allPlots(ui))
        {
            const bool override =
                _plotState.contains(p) ? _plotState[p].userOverride : false;

            // Se l'utente sta esplorando, trim più conservativo:
            // così non cancelli ciò che sta guardando.
            const double trimFactor = override ? 3.0 : 1.2;

            trimOld(p, t, _windowSec, trimFactor);

            // maxPts coerente con quanto stai cercando di tenere
            const int maxPts = computeMaxPts(_windowSec, fCurveHz, trimFactor, 1.5);
            limitMaxPoints(p, maxPts);

            // Autoscale Y solo in live (no override)
            if(!override)
                autoscaleYVisible(p, xMin, xMax);
        }
        // Niente replot qui: lo fa già il FAST timer
    });
    _timerSlow.start();
    #endif

    setWindowFlags(windowFlags()
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::Window, true); // la tratta come finestra top-level
    setWindowModality(Qt::NonModal);

    installLiveButtons();

    // Hook zoom popup (double click)
    /* Qui stai facendo due livelli di lambda:
     - hookZoom è una lambda che “installa” la logica di zoom su un plot qualsiasi
     - Dentro, fai un connect che collega il segnale mouseDoubleClick a un’altra lambda
     - La seconda lambda cattura [this, p] dove "thisil DeviceDialog e "p" è il plot su cui hai cliccato*/
    auto hookZoom = [this](QCustomPlot* p){

        connect(p, &QCustomPlot::mouseDoubleClick, this, [this, p](QMouseEvent*){

            if(_zooms.contains(p) && _zooms[p]){    // se esiste già un popup per quel plot
                _zooms[p]->raise();                 // lo porta in primo piano
                _zooms[p]->activateWindow();        // gli da focus
                return;
            }

            auto* z = new PlotZoomDialog(nullptr);     // altrimenti crei il popup con parent this
            z->attachTo(p); // collega e condivide i dati
            z->show();      // lo visualizza
            _zooms[p] = z;

            connect(z, &QObject::destroyed, this, [this, p]{    // quando l’utente chiude il popup, Qt emette destroyed
                _zooms.remove(p);
            });
        });
    };

    // applichi hook ai 4 plot
    hookZoom(ui->plotDuty);
    hookZoom(ui->plotCurrents);
    hookZoom(ui->plotVoltages);
    hookZoom(ui->plotPosition);
}

DeviceDialog::~DeviceDialog(){
    delete ui;
}

void DeviceDialog::appendTelemetryBatch(const QVector<TelemetrySample>& batch)
{
    if(batch.isEmpty()) return;

    #if MCU_SIM_RUN
    double lastT = 0.0;
    #endif

    for(const auto& s : batch)
    {
        #if MCU_SIM_RUN
        lastT = s.tSec;
        #else
        _lastT = s.tSec;  // tieni l’ultimo tempo visto
        #endif

        QCustomPlot* p = plotForType(s.type);
        if(!p) continue;

        const int gi = (s.id == 0) ? 0 : 1;
        if(gi >= p->graphCount()) continue;

        p->graph(gi)->addData(s.tSec, s.value); // aggiunge i dati di ogni curva in un contenitore di QCustomPlot che si chiama QCPGraphDataContainer
        // Dato quanto detto alla riga precedente allora il problema di accumulare dati sarebbe sovraccaricare la CPU.
        // Risolviamo questo potenziale problema con gli helper trimOld() e limitMaxPoints()
    }


    #if MCU_SIM_RUN
    // Autoscroll X (una volta sola, non per punto)
    ui->plotDuty->xAxis->setRange(lastT, _windowSec, Qt::AlignRight);
    ui->plotCurrents->xAxis->setRange(lastT, _windowSec, Qt::AlignRight);
    ui->plotVoltages->xAxis->setRange(lastT, _windowSec, Qt::AlignRight);
    ui->plotPosition->xAxis->setRange(lastT, _windowSec, Qt::AlignRight);

    // Taglia i dati vecchi: evita crescita infinita (memoria + tempo replot)
    trimOld(ui->plotDuty,     lastT, _windowSec, 1.2);
    trimOld(ui->plotCurrents, lastT, _windowSec, 1.2);
    trimOld(ui->plotVoltages, lastT, _windowSec, 1.2);
    trimOld(ui->plotPosition, lastT, _windowSec, 1.2);

    // Guardrail max points
    static constexpr int kMaxPts = 2000;
    for(QCustomPlot* p : {ui->plotDuty, ui->plotCurrents, ui->plotVoltages, ui->plotPosition})
        limitMaxPoints(p, kMaxPts);

    // Autoscale Y solo se non override utente
    const double xMax = lastT;
    const double xMin = lastT - _windowSec;
    for(QCustomPlot* p : {ui->plotDuty, ui->plotCurrents, ui->plotVoltages, ui->plotPosition})
    {
        auto it = _plotState.find(p);
        const bool override = (it != _plotState.end()) ? it->userOverride : false;
        if(!override)
            autoscaleYVisible(p, xMin, xMax);
    }

    // Replot una volta per plot (queued)
    ui->plotDuty->replot(QCustomPlot::rpQueuedReplot);
    ui->plotCurrents->replot(QCustomPlot::rpQueuedReplot);
    ui->plotVoltages->replot(QCustomPlot::rpQueuedReplot);
    ui->plotPosition->replot(QCustomPlot::rpQueuedReplot);
    #else
    // reset “punto di ancoraggio” del tempo continuo
    _lastWallMs = _wall.elapsed();
    #endif
}

void DeviceDialog::setupPlot(QCustomPlot* p, const QString& yLabel)
{
    p->legend->setVisible(false);    // rende visibile la legenda interna di qcustomplot

    p->addGraph();  // crea curva con indice 0
    p->graph(0)->setName("measure");   // accede alla curva con indice 0 e setta il nome per la legenda
    p->graph(0)->setPen(QPen(Qt::blue));

    p->addGraph();
    p->graph(1)->setName("target");
    p->graph(1)->setPen(QPen(Qt::red));

    p->xAxis->setLabel("Time (s)");
    p->yAxis->setLabel(yLabel);

    // Range iniziali. Poi in streaming l’asse x scorrerà
    p->xAxis->setRange(0, _windowSec);
    p->yAxis->setRange(-10, 10);

    // abilita zoom/drag sui plot piccoli
    p->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    p->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    p->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    p->setSelectionRectMode(QCP::srmZoom);

    _plotState[p] = PlotState{};
    p->setFocusPolicy(Qt::StrongFocus);  // aiuta wheel/focus su alcune piattaforme

    connect(p, &QCustomPlot::mousePress, this, [this, p](QMouseEvent*){
        auto &st = _plotState[p];
        st.userOverride = true;
        st.lastUser.restart();
    });

    connect(p, &QCustomPlot::mouseWheel, this, [this, p](QWheelEvent*){
        auto &st = _plotState[p];
        st.userOverride = true;
        st.lastUser.restart();
    });

    connect(p, &QCustomPlot::mouseRelease, this, [this, p](QMouseEvent*){
        auto &st = _plotState[p];
        st.lastUser.restart();

        // dopo un po' di inattività puoi riabilitare il follow automatico:
        QTimer::singleShot(1500, this, [this, p]{
            auto it = _plotState.find(p);
            if(it == _plotState.end()) return;

            if(it->lastUser.isValid() && it->lastUser.elapsed() >= 1500)
                it->userOverride = false;
        });
    });

}

QCustomPlot* DeviceDialog::plotForType(TelemetryType t) const
{
    // mapping pulito: tipo → widget
    switch(t){
    case TelemetryType::Duty:     return ui->plotDuty;
    case TelemetryType::Current:  return ui->plotCurrents;
    case TelemetryType::Voltage:  return ui->plotVoltages;
    case TelemetryType::Position: return ui->plotPosition;
    }
    return nullptr;
}

double DeviceDialog::liveTimeNow() const
{
    // In real HW usi tempo continuo (asse X fluido anche se i batch arrivano a scatti)
    return tDisplayNow();
}

double DeviceDialog::tDisplayNow() const
{
    // tempo continuo: anche se i batch arrivano a scatti, la GUI scorre in modo uniforme
    const qint64 nowMs = _wall.elapsed();
    const double dt = (nowMs - _lastWallMs) / 1000.0;
    return _lastT + dt;
}

void DeviceDialog::forceLive(QCustomPlot* p)
{
    if(!p) return;

    if(!_plotState.contains(p))
        _plotState[p] = PlotState{};

    // 1) disattiva l’override utente per quel plot
    auto &st = _plotState[p];
    st.userOverride = false;
    st.lastUser.invalidate();

    p->deselectAll();

    // 2) riallinea X e Y “subito” (così vedi immediatamente il live)
    const double t = liveTimeNow();
    const double xMax = t;
    const double xMin = t - _windowSec;

    p->xAxis->setRange(t, _windowSec, Qt::AlignRight);
    autoscaleYVisible(p, xMin, xMax);

    // 3) replot (queued, non blocca)
    p->replot(QCustomPlot::rpQueuedReplot);
}

void DeviceDialog::forceLiveAll()
{
    for(QCustomPlot* p : {ui->plotDuty, ui->plotCurrents, ui->plotVoltages, ui->plotPosition})
        forceLive(p);
}

void DeviceDialog::installLiveButtons() // funziona perché nel tuo .ui ci sono già vboxDuty, vboxCurrents, ecc
{
    const QString kLiveBtnStyle =
        "QPushButton {"
        "  background: #E8E8E8;"
        "  border: 1px solid #BDBDBD;"
        "  border-radius: 6px;"
        "  padding: 4px 10px;"
        "  color: #202020;"
        "}"
        "QPushButton:hover {"
        "  background: #DEDEDE;"
        "}"
        "QPushButton:pressed {"
        "  background: #D0D0D0;"
        "}";

    // ---------- Pulsante globale in alto per riallineare tutti i plot in modalità live ----------
    /*{
        auto* row = new QHBoxLayout();
        row->addStretch();

        auto* btnAll = new QPushButton("LIVE", this);
        btnAll->setStyleSheet(kLiveBtnStyle);
        btnAll->setToolTip("Return to live view (all plots)");
        btnAll->setAutoDefault(false);
        btnAll->setFixedHeight(24);

        row->addWidget(btnAll);

        // Inserisce la row in cima al vboxMain (senza modificare .ui)
        ui->vboxMain->insertLayout(0, row);

        connect(btnAll, &QPushButton::clicked, this, [this]{
            forceLiveAll();
        });
    }*/

    // ---------- Pulsanti per-plot (uno per groupbox) ----------
    auto addBtn = [this, &kLiveBtnStyle](QVBoxLayout* boxLayout, QCustomPlot* plot, const QString& tip)
    {
        if(!boxLayout || !plot) return;

        // Riga header
        auto* header = new QWidget(this);
        auto* row = new QHBoxLayout(header);
        row->setContentsMargins(0,0,0,0);
        row->setSpacing(10);

        // Legenda a sinistra (usa i colori reali dei graph)
        auto* tMeasure = makeLegendToggle(plot->graph(0)->pen().color(), "measure", header);
        auto* tTarget  = makeLegendToggle(plot->graph(1)->pen().color(), "target",  header);

        row->addWidget(tMeasure);
        row->addWidget(tTarget);
        row->addStretch();

        // Bottone LIVE (grigio chiaro)
        auto* btnLive = new QPushButton("LIVE", header);
        btnLive ->setStyleSheet(kLiveBtnStyle);
        btnLive ->setToolTip(tip);
        btnLive ->setAutoDefault(false);
        btnLive ->setFixedHeight(22);
        btnLive ->setMaximumWidth(60);

        // Metti il pulsante prima del plot, allineato a destra
        //boxLayout->insertWidget(0, btn, 0, Qt::AlignRight);
        row->addWidget(btnLive);

        // Inserisci header come primo elemento del groupbox layout (sopra al plot)
        boxLayout->insertWidget(0, header, 0);

        // Toggle -> show/hide graph + replot + autoscale coerente
        auto applyVis = [this, plot](int gi, bool vis)
        {
            if(!plot || gi >= plot->graphCount()) return;

            plot->graph(gi)->setVisible(vis);

            // se l’utente aveva selezionato la curva e poi la spegne, pulisci selezioni
            plot->deselectAll();

            // aggiorna anche il popup se esiste
            if(_zooms.contains(plot) && _zooms[plot])
                _zooms[plot]->setGraphVisible(gi, vis);

            // autoscale immediato se non override utente
            const bool override = _plotState.contains(plot) ? _plotState[plot].userOverride : false;
            if(!override){
                const double t = liveTimeNow();
                autoscaleYVisible(plot, t - _windowSec, t);
            }

            plot->replot(QCustomPlot::rpQueuedReplot);
        };

        connect(tMeasure, &QToolButton::toggled, this, [=](bool on){ applyVis(0, on); });
        connect(tTarget,  &QToolButton::toggled, this, [=](bool on){ applyVis(1, on); });

        connect(btnLive, &QPushButton::clicked, this, [this, plot]{
            forceLive(plot);
        });
    };

    addBtn(ui->vboxDuty,     ui->plotDuty,     "Return to live view (Duty)");
    addBtn(ui->vboxCurrents, ui->plotCurrents, "Return to live view (Currents)");
    addBtn(ui->vboxVoltages, ui->plotVoltages, "Return to live view (Voltages)");
    addBtn(ui->vboxPosition, ui->plotPosition, "Return to live view (Position)");
}
