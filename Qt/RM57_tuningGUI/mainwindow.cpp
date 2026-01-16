#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDoubleSpinBox>

#include <QPixmap>              // ci permette di creare Pixmap object

#ifdef MCU_SIM          // se compili senza MCU_SIM → MCU_SIM_RUN diventa 0
#ifndef MCU_SIM_RUN     // se compili con MCU_SIM ma metti MCU_SIM_RUN=0 → thread non parte mai
#define MCU_SIM_RUN 1
#endif
#else
#define MCU_SIM_RUN 0
#endif


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

    //============== BEGIN: NUOVE MODIFICHE =============
    // Connetto la ComboBox al pannello corrispondente
    connect(ui->cmbDutyType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx){
                ui->stkDutyParams->setCurrentIndex(idx);
                _dutyCfg.type = static_cast<DutyWaveform>(idx);
                scheduleSendTuning();
            });
    // ============= END: NUOVE MODIFICHE ==============

    // Avvia l'event loop del thread I/O
    _ioThread->start(); // parte il thread ed entra nel suo event loop (coda eventi)

    // ==========================
    // Hook spinbox -> schedule
    // ==========================
    auto hookSpin = [this](QDoubleSpinBox* spn){
        if(!spn) return;
        connect(spn, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::scheduleSendTuning);
    };

    connect(ui->chkLive, &QCheckBox::toggled,
            this, &MainWindow::on_chkLive_toggled);

    // Signal tuning (min/max/freq)
    hookSpin(ui->spnDutyMin);
    hookSpin(ui->spnDutyMax);
    hookSpin(ui->spnDutyFreq);

    hookSpin(ui->spnCurMin);
    hookSpin(ui->spnCurMax);
    hookSpin(ui->spnCurFreq);

    hookSpin(ui->spnPosMin);
    hookSpin(ui->spnPosMax);
    hookSpin(ui->spnPosFreq);

    // PID tuning (Kp/Ki/Kd)
    hookSpin(ui->spnKp);
    hookSpin(ui->spnKi);
    hookSpin(ui->spnKd);

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

    #if MCU_SIM_RUN
        if(_sim){
            QMetaObject::invokeMethod(_sim, "stopSim", Qt::QueuedConnection);
            _sim = nullptr;
        }
        if(_simThread){
            _simThread->quit();
            _simThread->wait();
            _simThread->deleteLater();
            _simThread = nullptr;
        }
    #endif

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

    #if MCU_SIM_RUN
        // Simulatore attivo: richiede Virtual Serial Port Driver COM1<->COM2
        // Qt app (SerialWorker) deve aprire COM1; il simulatore aprirà COM2.
        const QString pcPort = ui->cmbPort->currentText();
        if(pcPort.compare("COM2", Qt::CaseInsensitive)==0){
            QMessageBox::warning(this, "Sim", "In SIM mode select COM1 (MCU sim is on COM2).");
            on_btnDisconnect_clicked();
            return;
        }

        if(!_simThread){
            _simThread = new QThread(this);
            _sim = new McuSimWorker();
            _sim->moveToThread(_simThread);

            connect(_simThread, &QThread::finished, _sim, &QObject::deleteLater);

            // logging/status -> GUI
            /*connect(_sim, &McuSimWorker::simStatus, this, [this](bool ok, const QString& msg){
                Q_UNUSED(ok);
                // opzionale: mostra msg su statusbar o qDebug
                // ui->statusBar->showMessage(msg, 3000);
            });*/

            connect(_simThread, &QThread::started, this, [this]{
                // COM2 è il lato "MCU simulato"
                const int baud = ui->cmbBaud->currentText().toInt();
                QMetaObject::invokeMethod(_sim, "startSim", Qt::QueuedConnection,
                                          Q_ARG(QString, "COM2"),
                                          Q_ARG(int, baud));
            });

            //
            connect(_simThread, &QThread::started, _worker, &SerialWorker::startBatching);

            _simThread->start();
        }
    #endif

    // Regola richiesta:
    // - LIVE: invio tuning con debounce (quindi pianifico sendAllTuning tra kDebounceMs)
    // - MANUALE: invio solo con Apply -> abilito Apply (dirty=true) così puoi inviare subito
    if(ui->chkLive->checkState() == Qt::Checked){
        _debounce.start(kDebounceMs);
        markDirty(false);
    }else{
        markDirty(true);
    }
}

void MainWindow::onTelemetryBatch(QVector<TelemetrySample> batch)
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

    // ==========================================================
    // Signal tuning: 4 eventi queued + 4 chiamate a _sp->write()
    // ==========================================================
    /*
    // Duty / Current / Position
    emit requestWrite(makeSignalTuningCmd(
        TelemetryType::Duty,
        ui->spnDutyMin->value(),
        ui->spnDutyMax->value(),
        ui->spnDutyFreq->value()
        ));

    emit requestWrite(makeSignalTuningCmd(
        TelemetryType::Current,
        ui->spnCurMin->value(),
        ui->spnCurMax->value(),
        ui->spnCurFreq->value()
        ));

    emit requestWrite(makeSignalTuningCmd(
        TelemetryType::Position,
        ui->spnPosMin->value(),
        ui->spnPosMax->value(),
        ui->spnPosFreq->value()
        ));

    // Voltage: NON tunabile -> nessun comando.

    // PID
    emit requestWrite(makePidTuningCmd(
        0,  //pidId
        ui->spnKp->value(),
        ui->spnKi->value(),
        ui->spnKd->value()
        ));

    // Manual mode: pulisco dirty e disabilito Apply
    markDirty(false);
    */

    /* Sia nel caso LIVE ON che LIVE OFF, la differenza tra LIVE e MANUALE è solo quando
     * arrivi a chiamare sendAllTuning() (debounce vs Apply).
     * Dopo l’emit la pipeline è identica: i 4 emit non “scrivono” subito: diventano 4 eventi QMetaCallEvent
     * in coda al _ioThread. Dopo i 4 emit, nella queue del _ioThread ci sono 4 chiamate a _sp->write(data) (di SerialWorker) in ordine.
        Nota: l’ordine resta quello di emissione (Duty → Current → Position → PID) perché sono 4 eventi accodati
               ovvero i 4 frame (12 + 12 + 12 + 18 byte)
       Dunque quando _ioThread gira nel suo event loop prende gli eventi uno per uno ed esegue _sp->write(data);
       Di seguito una ottimizzazione che riduce il carico delle chiamate.
    */

    // =================================================================
    // Signal tuning: un solo evento queued, un solo write verso driver
    // =================================================================

    QByteArray out;
    out.reserve(12 + 12 + 12 + 18); // pre-alloca il buffer così non riallochi più volte mentre appendi

    out += makeSignalTuningCmd(
        TelemetryType::Duty,
        ui->spnDutyMin->value(),
        ui->spnDutyMax->value(),
        ui->spnDutyFreq->value()
        );

    out += makeSignalTuningCmd(
        TelemetryType::Current,
        ui->spnCurMin->value(),
        ui->spnCurMax->value(),
        ui->spnCurFreq->value()
        );

    out += makeSignalTuningCmd(
        TelemetryType::Position,
        ui->spnPosMin->value(),
        ui->spnPosMax->value(),
        ui->spnPosFreq->value()
        );

    out += makePidTuningCmd(
        /*pidId*/0,
        ui->spnKp->value(),
        ui->spnKi->value(),
        ui->spnKd->value()
        );

    emit requestWrite(out);     /*<-- UN SOLO emit => UN SOLO evento queued => UN SOLO unico write(out) dove out contiene:

    3 × SignalTuningCmdWire da 12 byte = 36 B

        1 × PidTuningCmdWire da 18 byte = 18 B
            Totale = 54 byte consecutivi sul filo.

          Quindi sul pin RX della SCI3 arrivano 54 byte back-to-back (più start/stop UART) con timing dettato dal baudrate.*/

    markDirty(false);
}
