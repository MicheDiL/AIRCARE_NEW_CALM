#include "devicedialog.h"
#include "ui_devicedialog.h"
#include "plotzoomdialog.h"
#include <QToolButton>
#include <QHBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <cmath>   // std::ceil
#include <QtGlobal> // qBound

#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>
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
    installTopLogDirBar();
    setWindowTitle("CALM Monitoring");

    // ogni plot viene configurato ugualmente (2 curve + assi) e cambia solo la label Y
    setupPlot(ui->plotDuty,     "Duty [%]");
    setupPlot(ui->plotCurrents, "Current [mA]");
    setupPlot(ui->plotVoltages, "Voltage [V]");
    setupPlot(ui->plotPosition, "Position [V]");

    // Avvia clock per tempo "continuo" (asse X fluido)
    _wall.start();
    _lastWallMs = _wall.elapsed();

    //////////
    _rateWall.start();
    _rateLastMs = _rateWall.elapsed();
    //////////

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

        for(QCustomPlot* p : allPlots(ui))
        {
            const bool override =
                _plotState.contains(p) ? _plotState[p].userOverride : false;

            // Se l'utente sta esplorando, trim più conservativo:
            // così non cancelli ciò che sta guardando.
            const double trimFactor = override ? 3.0 : 1.2;

            trimOld(p, t, _windowSec, trimFactor);

            //////////
            /* Quanto tagliare? Dipende dalla frequenza reale di arrivo (che può cambiare se cambi s_period_ms).
            → quindi tu stimi la frequenza “reale” nel dialog e poi calcoli maxPts coerente con _windowSec.*/
            TelemetryType type{};
            if(p == ui->plotDuty) type = TelemetryType::Duty;
            else if(p == ui->plotCurrents) type = TelemetryType::Current;
            else if(p == ui->plotVoltages) type = TelemetryType::Voltage;
            else type = TelemetryType::Position;

            double fCurveHz = curveHzForPlot(type); // calcola la frequenza di telemetria dai dati che arrivano da RM57

            // fallback: se ancora non hai stime (all'avvio), assumi un valore ragionevole
            if(fCurveHz <= 1e-3)
                fCurveHz = 50.0; // oppure 100.0
            ///////////

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

    //////////
    // Nel caso della PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING rappresenta il numero di FRAME che arrivano nel periodo
    // Nel caso della SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING rappresenta il numero di curve/sample che voglio aggiornare
    _framesCount += batch.size();
    //////////

    for(const auto& s : batch)
    {
        _lastT = s.tSec;  // aggiorna il tempo dell'ultimo batch ricevuto (MCU time)

        ///////////////////////// SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING /////////////////////////////
        // Conta i tick reali: riconoscere un tick quando cambia tSec
        // Converti tSec in "chiave ms" robusta (evita problemi float tipo 0.005 -> 0.0049999)
        const qint64 tickKeyMs = qint64(std::llround(s.tSec * 1000.0));
        // Se è cambiata la chiave temporale, è arrivato un nuovo burst/tick
        if (tickKeyMs != _lastTickKeyMs) {
            _lastTickKeyMs = tickKeyMs;
            _tickCount++;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////

        // contatori curve
        const int idx = int(s.type)*2 + (s.id ? 1 : 0);
        if(idx>=0 && idx <8){
            _curveCount[idx]++; // conta quanti sample per singola curva (8 curve)
        }


        QCustomPlot* p = plotForType(s.type);
        if(!p) continue;

        // scegli quale curva (graph) in base a id
        const int gi = (s.id == 0) ? 0 : 1;
        if(gi >= p->graphCount()) continue;

        p->graph(gi)->addData(s.tSec, s.value); // aggiunge i dati di ogni curva in un contenitore di QCustomPlot che si chiama QCPGraphDataContainer
        // Dato quanto detto alla riga precedente allora il problema di accumulare dati sarebbe sovraccaricare la CPU.
        // Risolviamo questo potenziale problema con gli helper trimOld() e limitMaxPoints()
    }

    // reset “punto di ancoraggio” del tempo continuo
    _lastWallMs = _wall.elapsed();

    updateRatesIfDue();
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

/////////
void DeviceDialog::updateRatesIfDue()
{
    /* Nella PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART:
        - misura ogni ~1s quanti TelemetrySample sono arrivati (_framesCount)
        - stima _framesHz = _framesCount/dt
        - stima _ticksHz = _framesHz/8 assumendo che arrivino sempre 8 sample per tick
       Nella SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART:
        - conta i tick reali (quanti burst distinti sono arrivati)
    */

    const qint64 now = _rateWall.elapsed();
    const qint64 dtMs = now - _rateLastMs;

    // aggiorna ogni ~1000ms (puoi anche 500ms)
    if(dtMs < 1000)
        return;

    const double dt = dtMs / 1000.0;

    _framesHz = _framesCount / dt;  // “Hz” dei sample che arrivano a DeviceDialog

    ///////////// PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART //////////////////
    //_ticksHz  = _framesHz / 8.0;    // assume 8 frame per tick
    //////////// SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART ////////////////
    // tick rate misurato contando i burst reali
    _ticksHz  = _tickCount / dt;
    //////////////////////////////////////////////////////////////////////////////

    for(int i=0;i<8;i++){
        _curveHz[i] = _curveCount[i] / dt; // Hz per curva
        _curveCount[i] = 0;
    }

    _framesCount = 0;
    _rateLastMs = now;
    _tickCount   = 0;

    // (opzionale) qui potresti aggiornare una label di status tipo:
    // ui->lblRate->setText(QString("frames=%1 Hz, ticks=%2 Hz").arg(_framesHz,0,'f',1).arg(_ticksHz,0,'f',1));
}

double DeviceDialog::curveHzForPlot(TelemetryType t) const
{
    const int base = int(t)*2;
    if(base < 0 || base+1 >= 8) return 0.0;

    const double h0 = _curveHz[base + 0];
    const double h1 = _curveHz[base + 1];

    return std::max(h0, h1);
}
////////

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

    const QString kRecBtnStyle =
        "QPushButton {"
        "  background: #F3F3F3;"
        "  border: 1px solid #BDBDBD;"
        "  border-radius: 6px;"
        "  padding: 4px 10px;"
        "  color: #202020;"
        "}"
        "QPushButton:hover { background: #E9E9E9; }"
        "QPushButton:checked {"
        "  background: #D32F2F;"     // rosso REC ON
        "  border: 1px solid #8E1E1E;"
        "  color: white;"
        "  font-weight: 700;"
        "}"
        "QPushButton:checked:hover { background: #C62828; "
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
    /* Lambda function di tipo anonimo -> aouto lo deduce
     [...] è la lista di cattura della lambda ovvero cosa la lambda conoscerà dall'esterno */
    auto addBtn = [this, &kLiveBtnStyle, &kRecBtnStyle](QVBoxLayout* boxLayout, QCustomPlot* plot, TelemetryType type, const QString& tip)
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
        auto* btnLive = new QPushButton("LIVE", header);    // figlio di header che è il parent
        btnLive ->setStyleSheet(kLiveBtnStyle);
        btnLive ->setToolTip(tip);
        btnLive ->setAutoDefault(false);
        btnLive ->setFixedHeight(22);
        btnLive ->setMaximumWidth(60);

        // Creo un bottone e lo rendo toggle
        auto* btnRec = new QPushButton("REC", header);  // figlio di header che è il parent
        btnRec->setStyleSheet(kRecBtnStyle);
        btnRec->setToolTip("Start/Stop recording su file per questo plot");
        btnRec->setAutoDefault(false);
        btnRec->setFixedHeight(22);
        btnRec->setMaximumWidth(60);
        btnRec->setCheckable(true); // così rendo toggle il bottone creato

        // Metti il pulsante prima del plot, allineato a destra
        //boxLayout->insertWidget(0, btn, 0, Qt::AlignRight);
        row->addWidget(btnLive);
        row->addWidget(btnRec);

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

        // connessioni che collegano i bottoni creati a una lamda function
        connect(tMeasure, &QToolButton::toggled, this, [=](bool on){
            applyVis(0, on);
        });
        connect(tTarget,  &QToolButton::toggled, this, [=](bool on){
            applyVis(1, on);
        });
        /* la capture list delle lamda delle due connessioni è [=] perchè sta a significare:
         tutto quello che la lambda usa dentro di sè, sta già fuori (visto che applyVis è a sua volta
         una lambda definita poco sopra) => la lambda della connessione prende tutto per valore =>
         in questo caso la lambda collegata al segnale si porta dentro una copia di applyVis per valore
         e quindi indirettamente anche di tutto ciò che applyVis a sua volta si portava dentro essendo essa una lambda.
        */

        connect(btnLive,  &QPushButton::clicked, this, [this, plot]{
            forceLive(plot);
        });
        connect(btnRec, &QPushButton::toggled,   this, [this, type](bool on){   // "on" viene dal metodo toggled(bool)
            emit recordToggled(type, on);
        });
    };

    addBtn(ui->vboxDuty,     ui->plotDuty,     TelemetryType::Duty,       "Return to live view (Duty)");
    addBtn(ui->vboxCurrents, ui->plotCurrents, TelemetryType::Current,    "Return to live view (Currents)");
    addBtn(ui->vboxVoltages, ui->plotVoltages, TelemetryType::Voltage,    "Return to live view (Voltages)");
    addBtn(ui->vboxPosition, ui->plotPosition, TelemetryType::Position,   "Return to live view (Position)");
}

// Crea barra per selzione directory di log
void DeviceDialog::installTopLogDirBar()
{
    // Determina una directory di default del tipo C:\Users\<utente>\Documents\CALM_Logs
    const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation); // QStandardPaths è la utility Qt che sa dove sono le cartelle “canoniche” sul sistema dell’utente
    const QString def  = QDir(docs).filePath("CALM_Logs");

    // Carica ultima directory salvata
    {
        QSettings s;
        const QString last = s.value("log/dir", def).toString();
        _logDir = last.isEmpty() ? def : last;
    }

    // Crea widget barra + layout orizzontale
    auto* bar = new QWidget(this);      // bar è figlio di this (ovvero di devicedialog)
    auto* row = new QHBoxLayout(bar);   // row figlio di bar
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(10);

    auto* lbl = new QLabel("Log directory:", bar);

    _cmbLogDir = new QComboBox(bar);
    _cmbLogDir->setMinimumWidth(60);

    // Determina una directory di default del tipo C:\Users\<utente>\Desktop
    const QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    // Determina una directory di default del tipo C:\Users\<utente>\Temp
    const QString temp    = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    // Popolamento della combo con testo + costruzione dell'intero percorso in cui fare il log
    _cmbLogDir->addItem("Documents/CALM_Logs", def);    // il secondo argomento di addItem è lo userData
    _cmbLogDir->addItem("Desktop", desktop);
    _cmbLogDir->addItem("Temp", temp);
    /* Quindi abbiamo 3 item con i rispettivi userData associati */

    // Inserisco anche l'attuale _logDir se non coincide con i preset
    if (_cmbLogDir->findData(_logDir) < 0) {
        _cmbLogDir->insertItem(0, _logDir, _logDir);
    }

    // Item speciale “Browse...”: userData vuoto come sentinel
    _cmbLogDir->addItem("Browse...", QString());    // lo userData è una stringa vuota

    // Cerca l'indice dell'item il cui userData è _logDir e lo impostiamo come quello scelto dall'utente
    _cmbLogDir->setCurrentIndex(_cmbLogDir->findData(_logDir));

    // Aggiungiamo la label che mostra la directory selezionata per intero
    auto* lblPath = new QLabel(_logDir, bar);
    lblPath->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblPath->setStyleSheet("color:#404040;");

    row->addWidget(lbl);
    row->addWidget(_cmbLogDir, 1);  // a comboBox _cmbLogDir con stretch factor 1
    row->addWidget(lblPath, 2);     // il path ha più “peso” in larghezza rispetto alla comboBox: sensato perché un path può essere lungo

    // Inserisci la barra in cima al layout principale del dialog
    //    Caso tipico: root layout è un QVBoxLayout
    if (auto* v = qobject_cast<QVBoxLayout*>(this->layout())) {
        v->insertWidget(0, bar);
    } else {
        // fallback: se root non è VBox, creiamo un VBox wrapper
        auto* old = this->layout();
        auto* wrapper = new QVBoxLayout();
        wrapper->setContentsMargins(10, 10, 10, 10);
        wrapper->setSpacing(10);
        wrapper->addWidget(bar);

        if (old) {
            // “impacchetto” il vecchio layout dentro un widget
            auto* oldHost = new QWidget(this);
            oldHost->setLayout(old);
            wrapper->addWidget(oldHost, 1);
        }
        this->setLayout(wrapper);
    }

    // GESTIONE CASO IN CUI UTENTE CAMBIA DIRECTORY DOPO AVER AVVIATO REGISTRAZIONE: cambio selezione -> aggiorna directory e notifica serialworker
    connect(_cmbLogDir, &QComboBox::currentIndexChanged, this, [this, lblPath](int idx){
        /* nella capture list della lambda ci sta:
            - this per accedere a _cmbLogDir, _logDir, emit, ecc
            - lblPath per aggiornare la label
        */
        const QString data = _cmbLogDir->itemData(idx).toString();  // prende lo userData dal nuovo item selezionato dall'utente e lo convertiamo in una QString

        // Se il nuovo item selezionato dall'utente è Browse...
        if (_cmbLogDir->itemText(idx) == "Browse...") {
            // setta la directory di partenza se il nuovo item selezionato è "Browse..."
            const QString start = _logDir.isEmpty() ? QDir::homePath() : _logDir; // se _logDir non è settata parti da Home
            const QString dir = QFileDialog::getExistingDirectory(this, "Select log directory", start);

            if (dir.isEmpty()) {
                // rollback alla dir corrente
                const int back = _cmbLogDir->findData(_logDir);
                if (back >= 0) _cmbLogDir->setCurrentIndex(back);
                return;
            }

            // Applica nuova directory e creala
            _logDir = dir;
            QDir().mkpath(_logDir);

            // se non esiste come item, aggiungilo in cima
            int existing = _cmbLogDir->findData(_logDir);
            if (existing < 0) {
                _cmbLogDir->insertItem(0, _logDir, _logDir);
                existing = 0;
            }
            _cmbLogDir->setCurrentIndex(existing);

            lblPath->setText(_logDir); // Aggiorna label path

            // persisti (opzionale)
            QSettings s;
            s.setValue("log/dir", _logDir);

            // Notifica il serialworker
            emit logDirChanged(_logDir);

            return;
        }

        // Preset normale
        if (!data.isEmpty()) {
            _logDir = data;
            QDir().mkpath(_logDir);

            lblPath->setText(_logDir);

            // persisti (opzionale)
            QSettings s;
            s.setValue("log/dir", _logDir);

            emit logDirChanged(_logDir);
        }
    });

    // 7) emetti subito il valore iniziale (così worker è allineato)
    QDir().mkpath(_logDir);
    emit logDirChanged(_logDir);
}
