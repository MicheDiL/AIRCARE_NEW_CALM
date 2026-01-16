#include "plotzoomdialog.h"
#include <QVBoxLayout>  // serve per piazzare _plot dentro il dialog senza usare geometry manuale
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <cstring>
#include <cmath>


/*********************************************************************************************************************
 *                                                  HELPERS
**********************************************************************************************************************/
static QCPGraph* pickClosestGraph(QCustomPlot* plot, const QPoint& pos)
{
    QCPGraph* best = nullptr;
    double bestDist = 1e100;
    const QPointF pf(pos);

    for(int i=0; i<plot->graphCount(); ++i){
        auto* g = plot->graph(i);
        if(!g || !g->visible()) continue;

        // selectTest ritorna:
        //  -1  se NON è selezionabile in quel punto (fuori tolleranza)
        // >=0  distanza in pixel dalla curva
        const double d = g->selectTest(pf, false);
        if(d >= 0 && d < bestDist){
            bestDist = d;
            best = g;
        }
    }
    return best; // nullptr se non hai cliccato vicino a nessuna curva
}

/*********************************************************************************************************************
 *                                                  METODI DELLA CLASSE
**********************************************************************************************************************/

PlotZoomDialog::PlotZoomDialog(QWidget* parent)
    : QDialog(parent)
{
    // imposti titolo e dimensione iniziale del plot ingrandito
    setWindowTitle("CALM focus");
    resize(900, 450);

    auto* lay = new QVBoxLayout(this);  // layout “figlio” del dialog
    lay->setContentsMargins(8,8,8,8);
    lay->setSpacing(6);

    // Top bar (legend toggles + LIVE)
    buildTopBar();
    lay->addWidget(_btnMeas->parentWidget()); // la topbar è parentWidget dei bottoni

    // Plot
    _plot = new QCustomPlot(this);                 // anche _plot figlio del dialog
    lay->addWidget(_plot, /*stretch*/1);          // lo inserisce nel layout: si ridimensiona automaticamente

    // Finestra scalabile + chiusura sicura
    setWindowFlags(windowFlags()
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::Window, true);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Abilito interazioni sui plot grandi come: drag, zoom e selezionabilità delle curve
    _plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables); // | è OR bitwise perché QCP::Interaction è una bitmask
    _plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    _plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    _plot->setSelectionRectMode(QCP::srmZoom);

    // Eventi mouseMove anche senza click
    _plot->setMouseTracking(true);
    _plot->setFocusPolicy(Qt::StrongFocus);
    _plot->setSelectionTolerance(10);

    // Stato iniziale: tracer nascosto e nessuna curva attiva
    setupPointerReadout();
    setTracerVisible(false);
    _activeGraph = nullptr;

    // === Override utente: stessa logica del DeviceDialog nel popup ===
    connect(_plot, &QCustomPlot::mousePress, this, [this](QMouseEvent*){
        markUserInteraction();
    });
    connect(_plot, &QCustomPlot::mouseWheel, this, [this](QWheelEvent*){
        markUserInteraction();
    });

    updateLiveButtonUi();
}

void PlotZoomDialog::buildTopBar()
{
    auto* bar = new QWidget(this);
    auto* row = new QHBoxLayout(bar);
    row->setContentsMargins(0,0,0,0);
    row->setSpacing(10);

    auto makeToggle = [](const QColor& c, const QString& text, QWidget* parent){
        auto* b = new QToolButton(parent);
        b->setCheckable(true);
        b->setChecked(true);
        b->setAutoRaise(true);
        b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        b->setText(text);

        QPixmap pm(10,10);
        pm.fill(c);
        b->setIcon(QIcon(pm));

        b->setStyleSheet(
            "QToolButton{ color:#202020; padding:2px 6px; }"
            "QToolButton:checked{ font-weight:600; }"
            "QToolButton:!checked{ color:#808080; }"
            );
        return b;
    };

    // inizialmente colori “placeholder”; li aggiorniamo in attachTo leggendo le pen reali
    _btnMeas = makeToggle(Qt::blue, "measure", bar);
    _btnTarg = makeToggle(Qt::red,  "target",  bar);

    row->addWidget(_btnMeas);
    row->addWidget(_btnTarg);
    row->addStretch();

    _btnLive = new QPushButton("LIVE", bar);
    _btnLive->setAutoDefault(false);
    _btnLive->setFixedHeight(22);
    _btnLive->setMaximumWidth(70);
    _btnLive->setStyleSheet(
        "QPushButton {"
        "  background: #E8E8E8;"
        "  border: 1px solid #BDBDBD;"
        "  border-radius: 6px;"
        "  padding: 4px 10px;"
        "  color: #202020;"
        "}"
        "QPushButton:hover { background: #DEDEDE; }"
        "QPushButton:pressed { background: #D0D0D0; }"
        "QPushButton:disabled { color:#9A9A9A; }"
        );
    _btnLive->setToolTip("Return to live view (follow the small plot ranges)");
    row->addWidget(_btnLive);

    connect(_btnLive, &QPushButton::clicked, this, [this]{
        forceLive();
    });
}

void PlotZoomDialog::attachTo(QCustomPlot* src)
{
    // crea graph nel plot grande con stesso name e pen

    if(!src) return;

    // disconnetti vecchia sorgente
    if(_srcConn)
        disconnect(_srcConn);
    _src = src;

    // Copia graphs e condivide i data container
    _plot->clearGraphs();   // resetta eventuali graph precedenti del popup
    _plot->legend->setVisible(false);    // rende visibile la legenda (utile per distinguere ID0/ID1)

    const int n = src->graphCount();    // quante curve ha il plot sorgente. Nel tuo caso 2 (ID0 e ID1)
    for(int i=0; i<n; ++i){
        _plot->addGraph();
        _plot->graph(i)->setName(src->graph(i)->name());
        _plot->graph(i)->setPen(src->graph(i)->pen());
        _plot->graph(i)->setData(src->graph(i)->data()); // condivisione dei dati con i plot picocli (QSharedPointer)
        _plot->graph(i)->setSelectable(QCP::stWhole);     // IMPORTANT: curva cliccabile
        _plot->graph(i)->setVisible(src->graph(i)->visible());
    }

    //
    // Aggiorna colori icon dei toggle leggendo i pen reali
    if(_plot->graphCount() >= 1){
        QPixmap pm(10,10); pm.fill(_plot->graph(0)->pen().color());
        _btnMeas->setIcon(QIcon(pm));
    }
    if(_plot->graphCount() >= 2){
        QPixmap pm(10,10); pm.fill(_plot->graph(1)->pen().color());
        _btnTarg->setIcon(QIcon(pm));
    }
    //

    // Labels e range iniziali (apri allineato al piccolo)
    _plot->xAxis->setLabel(src->xAxis->label());
    _plot->yAxis->setLabel(src->yAxis->label());
    // Allineamento iniziale (come “apri già in live”)
    syncRangesFromSource();

    // Connessione LIVE: quando il piccolo replota, il grande:
    // - segue i range SOLO se non sei in override
    // - replota sempre per vedere i nuovi dati
    _srcConn = connect(src, &QCustomPlot::afterReplot, this, [this]{
        if(!_src) return;

        if(!_userOverride){
            syncRangesFromSource();
        }
        _plot->replot(QCustomPlot::rpQueuedReplot);
    });

    // Toggle show/hide curve
    connect(_btnMeas, &QToolButton::toggled, this, [this](bool on){
        setGraphVisible(0, on);
    });
    connect(_btnTarg, &QToolButton::toggled, this, [this](bool on){
        setGraphVisible(1, on);
    });

    // Default active graph: target se visibile, altrimenti measure
    _activeGraph = nullptr;
    if(_plot->graphCount() > 1 && _plot->graph(1)->visible())
        _activeGraph = _plot->graph(1);
    else if(_plot->graphCount() > 0 && _plot->graph(0)->visible())
        _activeGraph = _plot->graph(0);

    if(_activeGraph){
        _tr->setGraph(_activeGraph);
        _tr->setPen(_activeGraph->pen());
        setTracerVisible(true);
        _readout->setText(QString("Selected: %1\nx=---  y=---").arg(_activeGraph->name()));
    } else {
        setTracerVisible(false);
        _readout->setText("Hidden\n(click a curve)");
    }

    // Sei in LIVE di default
    setUserOverride(false);

    _plot->replot();
}

void PlotZoomDialog::syncRangesFromSource()
{
    if(!_src) return;
    _plot->xAxis->setRange(_src->xAxis->range());
    _plot->yAxis->setRange(_src->yAxis->range());
}

void PlotZoomDialog::setGraphVisible(int idx, bool vis)
{
    if(!_plot) return;
    if(idx < 0 || idx >= _plot->graphCount()) return;

    _plot->graph(idx)->setVisible(vis);

    // Se spegni la curva attiva, nascondi tracer (e lascia selezione “memorizzata” a nullptr)
    if(!vis && _activeGraph == _plot->graph(idx)){
        setTracerVisible(false);
    }

    _plot->replot(QCustomPlot::rpQueuedReplot);
}

void PlotZoomDialog::markUserInteraction()
{
    // appena l’utente interagisce, entri in override finché non premi LIVE
    setUserOverride(true);
}

void PlotZoomDialog::setUserOverride(bool on)
{
    if(_userOverride == on) return;
    _userOverride = on;
    updateLiveButtonUi();
}

void PlotZoomDialog::updateLiveButtonUi()
{
    // Se sei già in LIVE (override=false), il bottone LIVE non serve → lo disabiliti.
    // Quando esplori (override=true), lo abiliti per “tornare live”.
    if(_btnLive)
        _btnLive->setEnabled(_userOverride);
}
void PlotZoomDialog::forceLive()
{
    // L’utente preme LIVE: esci da override e riallineati subito al plot piccolo
    setUserOverride(false);
    syncRangesFromSource();
    _plot->replot(QCustomPlot::rpQueuedReplot);
}

void PlotZoomDialog::setTracerVisible(bool on)
{
    if(!_tr) return;
    _tr->setVisible(on);


    if(_readout) _readout->setVisible(true);   // il readout lo teniamo sempre, ma lo aggiorniamo come testo
}


void PlotZoomDialog::setupPointerReadout()
{
    // tracer
    _tr = new QCPItemTracer(_plot);  // crea un “item” grafico agganciato al plot (come figlio logico del plot)
    _tr->setStyle(QCPItemTracer::tsCircle);
    _tr->setPen(QPen(Qt::black));   // verrà aggiornato quando clicchi la curva
    _tr->setBrush(Qt::NoBrush);
    _tr->setSize(8);
    _tr->setInterpolating(true);

    // testo in alto a sinistra dell’axisRect
    _readout = new QCPItemText(_plot);
    _readout->position->setType(QCPItemPosition::ptAxisRectRatio);
    _readout->position->setCoords(0.10, 0.04);  // 3% da sinistra, 2% dall’alto
    _readout->setPadding(QMargins(6, 4, 6, 4));
    _readout->setBrush(QBrush(QColor(255, 255, 255, 200)));
    _readout->setPen(QPen(Qt::gray));
    _readout->setText("Hidden\n(click a curve)");

    // 1) Click su una curva => selezione curva attiva
    connect(_plot, &QCustomPlot::mousePress, this, [this](QMouseEvent* e){
        if(e->button() != Qt::LeftButton) return;

        QCPGraph* g = pickClosestGraph(_plot, e->pos());

        if(!g){
            // CLICK NEL VUOTO => NASCONDI tracer, MA NON PERDI la curva attiva
            setTracerVisible(false);

            if(_readout){
                if(_activeGraph)
                    _readout->setText(QString("Hidden (selected: %1)\n(click curve to show)")
                                          .arg(_activeGraph->name()));
                else
                    _readout->setText("Hidden\n(click a curve)");
            }

            _plot->replot(QCustomPlot::rpQueuedReplot);
            return;
        }

        // CLICK SU CURVA => SELECT
        // Se clicchi su una curva diversa, la rendi attiva
        if(_activeGraph != g){
            _activeGraph = g;
            _tr->setGraph(_activeGraph);
            _tr->setPen(_activeGraph->pen());
        }

        // Toggle: se è nascosto -> mostra, se è mostrato -> resta mostrato (puoi anche decidere di togglare off qui)
        setTracerVisible(true);

        if(_readout){
            _readout->setText(QString("Selected: %1\nx=---  y=---").arg(_activeGraph->name()));
        }

        _plot->replot(QCustomPlot::rpQueuedReplot);
    });

    // Movimento mouse -> aggiorna SOLO la curva selezionata
    connect(_plot, &QCustomPlot::mouseMove, this, [this](QMouseEvent* e){
        updatePointerAt(e->pos());
    });
}

bool PlotZoomDialog::yAtXInterpolated(QCPGraph* g, double x, double& yOut)
{
    // Questa funzione prende una curva e restituisce y(x) stimata

    if(!g) return false;
    auto data = g->data();
    if(!data || data->isEmpty()) return false;

    auto it1 = data->findBegin(x, true); // primo punto con key >= x
    if(it1 == data->constEnd()){
        auto itLast = data->constEnd(); --itLast;
        yOut = itLast->value;
        return true;
    }

    if(it1 == data->constBegin()){
        yOut = it1->value;
        return true;
    }

    auto it0 = it1; --it0;

    const double x0 = it0->key, y0 = it0->value;
    const double x1 = it1->key, y1 = it1->value;

    if(x1 == x0){
        yOut = y1;
        return true;
    }

    const double t = (x - x0) / (x1 - x0);
    yOut = y0 + t * (y1 - y0);
    return true;
}

void PlotZoomDialog::updatePointerAt(const QPoint& mousePos)
{
    if(!_activeGraph || !_tr || !_tr->visible())
        return;

    // pixel -> coord X
    const double x = _plot->xAxis->pixelToCoord(mousePos.x());

    // calcoli y(x) interpolata
    double y = 0.0;
    if(!yAtXInterpolated(_activeGraph, x, y))
        return;

    // posiziona il tracer sulla curva selezionata
    _tr->setGraphKey(x);
    _tr->updatePosition();

    _readout->setText(QString("Selected: %1\nx=%2  y=%3")
                          .arg(_activeGraph->name())
                          .arg(x, 0, 'f', 3)
                          .arg(y, 0, 'f', 3));

    _plot->replot(QCustomPlot::rpQueuedReplot);
}
