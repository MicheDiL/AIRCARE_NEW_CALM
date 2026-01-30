#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QPixmap>              // ci permette di creare Pixmap object

//////////////////////// Helpers /////////////////////////////
 /* mapping espliciti:
    - ComboIndex → SignalShape
    - SignalShape → StackPageIndex

In Qt Designer abbiamo:
    - QComboBox: 0 Sin, 1 Const, 2 StepTriangle
    - QStackWidget: 0 Sin, 1 Const, 2 StepTriangle*/
static SignalShape shapeFromComboIndex(int idx)
{
    // Combo: Sin, Const, StepTriangle
    switch (idx) {
    case 0: return SignalShape::Sin;
    case 1: return SignalShape::Const;
    case 2: return SignalShape::StepTriangle;
    default: return SignalShape::Sin;
    }
}

static int pageIndexFromShape(SignalShape sh)
{
    // stacked: 0=Sin, 1=Const, 2=Triang
    switch (sh) {
    case SignalShape::Sin:          return 0;
    case SignalShape::Const:        return 1;
    case SignalShape::StepTriangle: return 2;
    default: return 1;
    }
}

static int toIntMs(double v)
{
    // tu usi QDoubleSpinBox anche per ms => arrotondo e clampo
    int ms = int(qRound(v));
    if(ms < 0) ms = 0;
    return ms;
}
/////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Settiamo l'immagine per la connessione USB PC<->CALM nella label "label_usb_img"
    QPixmap pix("C:/Users/mdilucchio/OneDrive - Fondazione Istituto Italiano Tecnologia/Desktop/QtLearning/Qt SerialPort trimmer FW/image resource/calm_pc_usb.png");
    int w = ui->label_usb_img->width();
    int h = ui->label_usb_img->height();
    ui->label_usb_img->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatioByExpanding));

    setWindowTitle("CALM Tuning");

    // 1) Popola porta COM + stato UI iniziale
    loadPorts();
    setConnectedUi(false);

    // 2) Debounce timer (single shot)
    _debounce.setSingleShot(true);
    connect(&_debounce, &QTimer::timeout, this, &MainWindow::sendAllTuning);

    // ================
    // Thread + Worker
    // ================
    _ioThread = new QThread(this);  // thread come child della mainwindow
    _worker = new SerialWorker();   //  _worker SENZA parent (così non rischi di distruggerlo dal thread sbagliato)

    _worker->moveToThread(_ioThread);   // da ora in poi _worker appartiene” al thread I/O. Da qui in poi, gli slot del worker, se chiamati via queued connection, verranno eseguiti nel thread _ioThread

    // Quando il thread finisce, il worker si autodistrugge nel suo event loop
    connect(_ioThread, &QThread::finished, _worker, &QObject::deleteLater);

    // ============================= QUEUEDCONNECTIONS ==============================
    // GUI -> worker: la GUI emette segnali → il worker riceve in queued => Connessioni “queued” esplicite usando Qt::QueuedConnection
    connect(this, &MainWindow::requestOpen,
            _worker, &SerialWorker::openPort,
            Qt::QueuedConnection);  // Qt non chiama openPort subito ma mette una “chiamata” nella coda eventi del thread del receiver (_worker) quando il thread I/O processa gli eventi, esegue openPort lì

    connect(this, &MainWindow::requestClose,
            _worker, &SerialWorker::closePort,
            Qt::QueuedConnection);

    connect(this, &MainWindow::requestWrite,
            _worker, &SerialWorker::writeBytes,
            Qt::QueuedConnection);

    // Worker -> GUI (Qt usa una queued connection implicita, perché sender/receiver thread diversi)
    connect(_worker, &SerialWorker::connectedChanged,
            this, &MainWindow::onWorkerConnected);

    connect(_worker, &SerialWorker::telemetryBatch, // In GUI ricevi un batch e aggiorni i plot in blocco e non facendo un replot per ogni punto/campione
            this, &MainWindow::onTelemetryBatch,
            Qt::QueuedConnection);

    // ================================================================================================


    // disabilita i controlli delle pagine StepTriang/Const finché non implementi il protocollo
    ui->pageDutyTriang->setEnabled(true);
    ui->pageDutyConst->setEnabled(true);

    ui->pageCurrentTriang->setEnabled(true);
    ui->pageCurrentConst->setEnabled(true);

    ui->pagePositionTriang->setEnabled(true);
    ui->pagePositionConst->setEnabled(true);

    //ui->pageVoltTriang->setEnabled(false);
    //ui->pageVoltConst->setEnabled(false);

    // Connetto la ComboBox al pannello corrispondente: cambia pannello parametri in base al tipo di segnale selezionato
    // Collega la QComboBox della shape (Sin / StepTriangle / Const) allo QStackedWidget che contiene le pagine con i parametri relativi.
    auto bindOne = [this](QComboBox* cmb, QStackedWidget* stk, int idxSig, auto applyFn)
    {
        if(!cmb || !stk) return;

        auto sync = [this, cmb, stk, idxSig, applyFn]()
        {
            const SignalShape sh = shapeFromComboIndex(cmb->currentIndex());

            // modello
            _sig[idxSig].type = sh;

            // UI: pagina corretta
            stk->setCurrentIndex(pageIndexFromShape(sh));

            // carica i valori della shape nel pannello giusto (MODEL -> UI)
            (this->*applyFn)();

            // live/manual
            scheduleSendTuning();
        };

        connect(cmb, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [sync](int){ sync(); });

        // sync iniziale
        sync();
    };

    // Duty (0)
    bindOne(ui->cmbDutyType, ui->stkDutyParams, 0, &MainWindow::applyDutyUiFromConfig);

    // Current (1)
    bindOne(ui->cmbCurrentType,  ui->stkCurrentParams,  1, &MainWindow::applyCurUiFromConfig);

    // Position (2)
    bindOne(ui->cmbPositionType,  ui->stkPositionParams,  2, &MainWindow::applyPosUiFromConfig);

    // Voltage (3)
    //bindOne(ui->cmbVoltType, ui->stkVoltParams, 3, &MainWindow::applyVoltUiFromConfig);

    // =================================================================================================
    // Hook spinbox -> schedule: ogni modifica dei parametri va in _sig[i].min_sin, _sig[i].stepAmp, ecc.
    // =================================================================================================
    auto hook = [this](QDoubleSpinBox* spn, auto writer){
        if(!spn) return;
        connect(spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this, writer](double v){
                    writer(v);
                    scheduleSendTuning();
                });
    };

    // ===================
    // DUTY (_sig[0])
    // ===================
    // Duty - SIN page
    hook(ui->spnDutyMin,  [this](double v){ _sig[0].min_sin = v; });
    hook(ui->spnDutyMax,  [this](double v){ _sig[0].max_sin = v; });
    hook(ui->spnDutyFreq, [this](double v){ _sig[0].freq_sin    = v; });

    // Duty - TRIANG page
    hook(ui->spnDutyMinForTriang,           [this](double v){ _sig[0].min_triang = v; });
    hook(ui->spnDutyMaxForTriang,           [this](double v){ _sig[0].max_triang = v; });
    hook(ui->spnDutyStepForTriang,          [this](double v){ _sig[0].stepAmp    = v; });
    hook(ui->spnDutyStepDurationForTriang,  [this](double v){ _sig[0].stepMs     = toIntMs(v); });

    // Duty - CONST page
    hook(ui->spnDutyVal, [this](double v){ _sig[0].constValue = v; });

    // ===================
    // CURRENT (_sig[1])
    // ===================
    hook(ui->spnCurMin,  [this](double v){ _sig[1].min_sin  = v; });
    hook(ui->spnCurMax,  [this](double v){ _sig[1].max_sin  = v; });
    hook(ui->spnCurFreq, [this](double v){ _sig[1].freq_sin = v; });

    hook(ui->spnCurrentMinForTriang,           [this](double v){ _sig[1].min_triang = v; });
    hook(ui->spnCurrentMaxForTriang,           [this](double v){ _sig[1].max_triang = v; });
    hook(ui->spnCurrentStepForTriang,          [this](double v){ _sig[1].stepAmp    = v; });
    hook(ui->spnCurrentStepDurationForTriang,  [this](double v){ _sig[1].stepMs     = toIntMs(v); });

    hook(ui->spnCurrentVal, [this](double v){ _sig[1].constValue = v; });

    // ===================
    // POSITION (_sig[2])
    // ===================
    hook(ui->spnPosMin,  [this](double v){ _sig[2].min_sin = v; });
    hook(ui->spnPosMax,  [this](double v){ _sig[2].max_sin = v; });
    hook(ui->spnPosFreq, [this](double v){ _sig[2].freq_sin    = v; });

    hook(ui->spnPositionMinForTriang,           [this](double v){ _sig[2].min_triang = v; });
    hook(ui->spnPositionMaxForTriang,           [this](double v){ _sig[2].max_triang = v; });
    hook(ui->spnPositionStepForTriang,          [this](double v){ _sig[2].stepAmp    = v; });
    hook(ui->spnPositiontStepDurationForTriang, [this](double v){ _sig[2].stepMs     = toIntMs(v); });

    hook(ui->spnPositionVal, [this](double v){ _sig[2].constValue = v; });

    // ===================
    // VOLTAGE (_sig[3])
    // ===================
    /*hook(ui->spnVoltMin,  [this](double v){ _sig[3].min_sin = v; });
    hook(ui->spnVoltMax,  [this](double v){ _sig[3].max_sin = v; });
    hook(ui->spnVoltFreq, [this](double v){ _sig[3].freq    = v; });

    hook(ui->spnVoltTriMin,     [this](double v){ _sig[3].min_triang = v; });
    hook(ui->spnVoltTriMax,     [this](double v){ _sig[3].max_triang = v; });
    hook(ui->spnVoltTriStepAmp, [this](double v){ _sig[3].stepAmp    = v; });
    hook(ui->spnVoltTriStepMs,  [this](double v){ _sig[3].stepMs     = toIntMs(v); });

    hook(ui->spnVoltConstValue, [this](double v){ _sig[3].constValue = v; });*/

    // Avvia l'event loop del thread I/O
    _ioThread->start(); // parte il thread ed entra nel suo event loop (coda eventi)

    connect(ui->chkLive, &QCheckBox::toggled, this, &MainWindow::on_chkLive_toggled);

    // Stato iniziale: tuning disabilitato finché non connesso
    ui->grpTuning->setEnabled(false);
    ui->btnApply->setEnabled(false);
}

MainWindow::~MainWindow()
{
    // Chiudi porta + timer
    on_btnDisconnect_clicked();

    // Arresta thread I/O in modo pulito
    if(_ioThread){
        _ioThread->quit();   // ferma event loop
        _ioThread->wait();   // aspetta che sia finito
        _ioThread = nullptr; // è child di MainWindow, verrà distrutto da Qt
    }

    delete ui;
}

bool MainWindow::isConnected() const
{
    // criterio pratico: se Disconnect è abilitato, sei connesso
    return ui->btnDisconnect->isEnabled();
}

void MainWindow::loadPorts()
{
    ui->cmbPort->clear();
    for(const auto& p : QSerialPortInfo::availablePorts()){
        ui->cmbPort->addItem(p.portName());
    }
}

void MainWindow::setConnectedUi(bool connected, const QString& text)
{
    ui->btnConnect->setEnabled(!connected);
    ui->btnDisconnect->setEnabled(connected);

    ui->cmbPort->setEnabled(!connected);
    ui->cmbBaud->setEnabled(!connected);

    ui->grpTuning->setEnabled(connected);

    if(!text.isEmpty()){
        ui->lblConnText->setText(text);
    } else {
        ui->lblConnText->setText(connected ? "Connected" : "Disconnected");
    }

    if(connected){
        ui->lblLed->setStyleSheet(
            "QLabel#lblLed{background-color:rgb(0,180,0);border:1px solid rgb(0,80,0);border-radius:8px;}"
            );
    }else{
        ui->lblLed->setStyleSheet(
            "QLabel#lblLed{background-color:rgb(200,0,0);border:1px solid rgb(80,0,0);border-radius:8px;}"
            );
    }

    // Apply: in LIVE off; in manual dipende da dirty
    if(!connected){
        ui->btnApply->setEnabled(false);
    } else {
        const bool live = (ui->chkLive->checkState() == Qt::Checked);
        ui->btnApply->setEnabled(!live && _tuningDirty);
    }
}

void MainWindow::markDirty(bool dirty)
{
    _tuningDirty = dirty;

    // Apply abilitato SOLO in manuale e solo se dirty
    if(!isConnected()){
        ui->btnApply->setEnabled(false);
        return;
    }

    const bool live = (ui->chkLive->checkState() == Qt::Checked);
    ui->btnApply->setEnabled(!live && _tuningDirty);
}

//=====================================================================================
//                                          SLOT
//=====================================================================================
void MainWindow::on_btnConnect_clicked()
{
    if(ui->cmbPort->currentIndex() < 0){
        QMessageBox::warning(this, "UART", "Select a COM port");
        return;
    }

    const QString port = ui->cmbPort->currentText();
    const int baud = ui->cmbBaud->currentText().toInt();

    // “Stato transitorio” mentre il worker prova ad aprire
    ui->btnConnect->setEnabled(false);
    ui->lblConnText->setText("Connecting...");

    // queued verso worker thread
    emit requestOpen(port, baud);
}

void MainWindow::on_btnDisconnect_clicked()
{
    _debounce.stop();
    markDirty(false);

    // Chiusura porta nel worker thread
    emit requestClose();

    // UI
    setConnectedUi(false);

    // chiudo dashboard (puoi anche fare hide se preferisci riusarla)
    if(_dlg){
        _dlg->close();
        // non delete: è parented a MainWindow, quindi resta. Se vuoi distruggerla:
        // _dlg->deleteLater(); _dlg=nullptr;
    }
}

void MainWindow::onWorkerConnected(bool ok, QString reason)
{
    if(!ok){
        setConnectedUi(false);
        if(!reason.isEmpty())
            QMessageBox::critical(this, "UART", "Connect failed: " + reason);
        return;
    }

    // Connesso: apro DeviceDialog con i 4 plot (Duty/Current/Voltage/Position)
    if(!_dlg){
        _dlg = new DeviceDialog(nullptr);              // top-level indipendente
        _dlg->setAttribute(Qt::WA_DeleteOnClose, true);
        connect(_dlg, &QObject::destroyed, this, [this]{ _dlg = nullptr; });
    }
    _dlg->show();
    _dlg->raise();
    _dlg->activateWindow();
    connect(this, &QObject::destroyed, _dlg, &QWidget::close);

    // UI
    setConnectedUi(true);

    // Regola richiesta:
    // - LIVE: invio tuning con debounce (quindi pianifico sendAllTuning tra kDebounceMs)
    // - MANUALE: invio solo con Apply -> abilito Apply (dirty=true) così puoi inviare subito
    if(ui->chkLive->checkState() == Qt::Checked){
        _debounce.start(kDebounceMs);
        markDirty(false);
    }else{
        markDirty(true);
    }

    // Colleghiamo i segnali emessi nel thread GUI agli slot definiti nel thread serialworker usando la strategia Qt::QueuedConnection
    const bool ok0 = connect(_dlg, &DeviceDialog::logDirChanged,
            _worker,  &SerialWorker::setLogDirectory,
            Qt::QueuedConnection);

    qInfo() << "connect recordToggled->setRecording =" << ok0;

    const bool ok1 = connect(_dlg, &DeviceDialog::recordToggled,
                             _worker,  &SerialWorker::setRecording,
                             Qt::QueuedConnection);
    qInfo() << "connect recordToggled->setRecording =" << ok1;
}

void MainWindow::onTelemetryBatch(const QVector<TelemetrySample> batch)
{
    if(!_dlg) return;

    _dlg->appendTelemetryBatch(batch); // passa batch per valore
}

void MainWindow::on_chkLive_toggled(bool on)
{
    qDebug() << "chkLive toggled:" << on;

    if(!isConnected()){
        // se non connesso, non permettere live
        ui->chkLive->blockSignals(true);
        ui->chkLive->setChecked(false);
        ui->chkLive->blockSignals(false);
        return;
    }

    if(on){
        // LIVE ON: Apply disabilitato, usa debounce
        ui->btnApply->setEnabled(false);
        ui->chkLive->setText("Live");
        if(_tuningDirty){
            _tuningDirty = false;
            _debounce.start(kDebounceMs); // invierà dopo che l’utente “si ferma”
        }
    }else{
        // LIVE OFF: stop debounce, Apply dipende da dirty
        ui->chkLive->setText("Send Live");
        _debounce.stop();
        ui->btnApply->setEnabled(_tuningDirty);
    }
}


void MainWindow::scheduleSendTuning()
{
    if(!isConnected()) return;

    const bool live = (ui->chkLive->checkState() == Qt::Checked);

    if(live){
        // LIVE: invio con debounce
        _debounce.start(kDebounceMs);
        // in live Apply non serve
        ui->btnApply->setEnabled(false);
        _tuningDirty = false;
    }else{
        // MANUALE: non invio nulla, segno dirty e abilito Apply
        markDirty(true);
        _debounce.stop();
    }
}

void MainWindow::on_btnApply_clicked()
{
    if(!isConnected()) return;

    // In manuale: invio solo qui
    if(ui->chkLive->checkState() != Qt::Checked){
        sendAllTuning();
        return;
    }

    // In LIVE normalmente il bottone è disabilitato, ma se per qualche motivo viene cliccato:
    // lo tratto come "send now" (non rompe il requisito live)
    _debounce.stop();
    sendAllTuning();
}


void MainWindow::sendAllTuning()    // Ogni volta che viene chiamta vengono emessi i 4 requestWrite (3 signal + PID)
{
    if(!isConnected()) return;

    // =================================================================
    // Signal tuning: un solo evento queued, un solo write verso driver
    // =================================================================

    QByteArray out; // array di byte in cui sono contenuti i segnali di tuning verso RM57

    // Costruiamo i pacchetti WaveformTuningCmdWire da 16 bytes per Duty/Current/Postion
    out += makeWaveformTuningCmd(TelemetryType::Duty,     1, _sig[0]);  // type, id 1 = target, SignalConfig
    out += makeWaveformTuningCmd(TelemetryType::Current,  1, _sig[1]);
    out += makeWaveformTuningCmd(TelemetryType::Position, 1, _sig[2]);

    out += makePidTuningCmd(
        /*pidId*/0,
        ui->spnKp->value(),
        ui->spnKi->value(),
        ui->spnKd->value()
        );

    emit requestWrite(out);     /*<-- UN SOLO emit => UN SOLO evento queued verso serialworker => UN SOLO unico write(out) dove out contiene:

        3 × WaveformTuningCmdWire da 16 byte (Duty/Current/Position, id=1 “target”)

        1 × PidTuningCmdWire da 18 byte

            Totale: 16 + 16 + 16 + 18 = 66 byte consecutivi*/

    markDirty(false);

}

void MainWindow::applyDutyUiFromConfig()
{
    // blocco i segnali di tutti i controlli duty che tocco
    QSignalBlocker b0(ui->spnDutyMin);
    QSignalBlocker b1(ui->spnDutyMax);
    QSignalBlocker b2(ui->spnDutyFreq);

    QSignalBlocker b3(ui->spnDutyMinForTriang);
    QSignalBlocker b4(ui->spnDutyMaxForTriang);
    QSignalBlocker b5(ui->spnDutyStepForTriang);
    QSignalBlocker b6(ui->spnDutyStepDurationForTriang);
    QSignalBlocker b7(ui->spnDutyVal);

    const auto &c = _sig[0];

    switch(c.type)
    {
    case SignalShape::Sin:
        ui->spnDutyMin->setValue(c.min_sin);
        ui->spnDutyMax->setValue(c.max_sin);
        ui->spnDutyFreq->setValue(c.freq_sin);
        break;

    case SignalShape::StepTriangle:
        ui->spnDutyMinForTriang->setValue(c.min_triang);
        ui->spnDutyMaxForTriang->setValue(c.max_triang);
        ui->spnDutyStepForTriang->setValue(c.stepAmp);
        ui->spnDutyStepDurationForTriang->setValue(double(c.stepMs));
        break;

    case SignalShape::Const:
        ui->spnDutyVal->setValue(c.constValue);
        break;
    }
}

void MainWindow::applyCurUiFromConfig()
{
    QSignalBlocker b0(ui->spnCurMin);
    QSignalBlocker b1(ui->spnCurMax);
    QSignalBlocker b2(ui->spnCurFreq);

    QSignalBlocker b3(ui->spnCurrentMinForTriang);
    QSignalBlocker b4(ui->spnCurrentMaxForTriang);
    QSignalBlocker b5(ui->spnCurrentStepForTriang);
    QSignalBlocker b6(ui->spnCurrentStepDurationForTriang);

    QSignalBlocker b7(ui->spnCurrentVal);

    const auto &c = _sig[1];

    switch(c.type)
    {
    case SignalShape::Sin:
        ui->spnCurMin->setValue(c.min_sin);
        ui->spnCurMax->setValue(c.max_sin);
        ui->spnCurFreq->setValue(c.freq_sin);
        break;

    case SignalShape::StepTriangle:
        ui->spnCurrentMinForTriang->setValue(c.min_triang);
        ui->spnCurrentMaxForTriang->setValue(c.max_triang);
        ui->spnCurrentStepForTriang->setValue(c.stepAmp);
        ui->spnCurrentStepDurationForTriang->setValue(double(c.stepMs));
        break;

    case SignalShape::Const:
        ui->spnCurrentVal->setValue(c.constValue);
        break;
    }
}

void MainWindow::applyPosUiFromConfig()
{
    QSignalBlocker b0(ui->spnPosMin);
    QSignalBlocker b1(ui->spnPosMax);
    QSignalBlocker b2(ui->spnPosFreq);

    QSignalBlocker b3(ui->spnPositionMinForTriang);
    QSignalBlocker b4(ui->spnPositionMaxForTriang);
    QSignalBlocker b5(ui->spnPositionStepForTriang);
    QSignalBlocker b6(ui->spnPositiontStepDurationForTriang);

    QSignalBlocker b7(ui->spnPositionVal);

    const auto &c = _sig[2];

    switch(c.type)
    {
    case SignalShape::Sin:
        ui->spnPosMin->setValue(c.min_sin);
        ui->spnPosMax->setValue(c.max_sin);
        ui->spnPosFreq->setValue(c.freq_sin);
        break;

    case SignalShape::StepTriangle:
        ui->spnPositionMinForTriang->setValue(c.min_triang);
        ui->spnPositionMaxForTriang->setValue(c.max_triang);
        ui->spnPositionStepForTriang->setValue(c.stepAmp);
        ui->spnPositiontStepDurationForTriang->setValue(double(c.stepMs));
        break;

    case SignalShape::Const:
        ui->spnPositionVal->setValue(c.constValue);
        break;
    }
}

/*void MainWindow::applyVoltUiFromConfig()
{
    QSignalBlocker b0(ui->spnVoltMin);
    QSignalBlocker b1(ui->spnVoltMax);
    QSignalBlocker b2(ui->spnVoltFreq);

    QSignalBlocker b3(ui->spnVoltTriMin);
    QSignalBlocker b4(ui->spnVoltTriMax);
    QSignalBlocker b5(ui->spnVoltTriStepAmp);
    QSignalBlocker b6(ui->spnVoltTriStepMs);

    QSignalBlocker b7(ui->spnVoltConstValue);

    const auto &c = _sig[3];

    switch(c.type)
    {
    case SignalShape::Sin:
        ui->spnVoltMin->setValue(c.min_sin);
        ui->spnVoltMax->setValue(c.max_sin);
        ui->spnVoltFreq->setValue(c.freq);
        break;

    case SignalShape::StepTriangle:
        ui->spnVoltTriMin->setValue(c.min_triang);
        ui->spnVoltTriMax->setValue(c.max_triang);
        ui->spnVoltTriStepAmp->setValue(c.stepAmp);
        ui->spnVoltTriStepMs->setValue(double(c.stepMs));
        break;

    case SignalShape::Const:
        ui->spnVoltConstValue->setValue(c.constValue);
        break;
    }
}*/

