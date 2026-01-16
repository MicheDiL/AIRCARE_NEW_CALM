#include "mainwindow.h"

#ifdef Q_OS_WIN           // Codice condizionale per Windows
#define INITGUID          // Forza la definizione dei GUID in questa TU
#include <initguid.h>     // Per definire i GUID, deve precedere <usbiodef.h>
#include <windows.h>
#include <dbt.h>
#include <usbiodef.h>     // Qui è dichiarato extern GUID_DEVINTERFACE_USB_DEVICE
#endif

#include "wacomcalibrator.h"

static HDEVNOTIFY g_hDevNotify = nullptr; // Dichiarazione di una variabile globale o membro di classe per conservare l'HDEVNOTIFY ritornato da RegisterDeviceNotification


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _server(nullptr)             // _server inizialmente è nullptr
{
    QWidget *central = new QWidget(this);

    setCentralWidget(central);

    // === PERSONALIZZAZIONE ASPETTO GRAFICO UI ===
    // qApp è una macro Qt che punta all’istanza globale di QApplication
    qApp->setStyleSheet(R"(
        QWidget {
            background-color: lightblue;
        }

        QPushButton {
            background-color: lightgray;
            color: black;
            border: 1px solid gray;
            padding: 5px;
        }

        QPushButton:hover {
            background-color: #dcdcdc;
        }

        QLineEdit, QListWidget, QSpinBox {
            background-color: white;
            color: black;
            border: 1px solid gray;
        }
    )");

    // === oggetti del menù laterale ===
    btnMenuConnections = new QPushButton("Connections");
    btnCalibration = new QPushButton("Wacom Calib.");
    btnMenuOperate = new QPushButton("Operate");
    btnMenuQuit = new QPushButton("Quit");

    // === struttura del menù laterale ===
    QVBoxLayout *menuLayout = new QVBoxLayout;
    menuLayout->addWidget(btnMenuConnections);
    menuLayout->addWidget(btnCalibration);
    menuLayout->addWidget(btnMenuOperate);
    menuLayout->addWidget(btnMenuQuit);
    menuLayout->addStretch();                           // Sposta tutto a sinistra

    // Raggruppamento del menu in un QGroupBox
    QGroupBox *controlGroup = new QGroupBox(this);
    controlGroup->setLayout(menuLayout);
    controlGroup->setStyleSheet(
        "QGroupBox { "
            "  border: 1px solid black;"
            "  border-radius: 5px;"
            "  margin: 20px;"
            "  font-size: 16px;"
            "}"
    );


    // === STACK centrale che contiene le due "pagine" principali: connessioni e area operativa===
    stack = new QStackedWidget;                         // Creo un oggetto contenitore che può contenere più widget figli, ma ne mostra solo uno alla volta
    stack->addWidget(createConnectionsPage());          // Aggiunge il widget della ConnectionPage come primo pannello (index 0)
    stack->addWidget(createOperatePage());              // Aggiunge il widget della OperatePage come secondo pannello (index 1)
    stack->setCurrentIndex(0);                          // Imposta qual è il widget visibile all'avvio dell'app (in tal caso Index 0)
    highlightMenuButton(btnMenuConnections);               // Evidenzia il tasto ConnectionPage del menù come attivo

    // ========== LAYOUT PRINCIPALE ==========
    QHBoxLayout *mainLayout = new QHBoxLayout;          // Layout orizzontale contenente menù laterale e area centrale
    mainLayout->addLayout(menuLayout);
    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(stack);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // === Collegamenti per le opzioni del menù ===
    connect(btnMenuConnections, &QPushButton::clicked, this, [this]() {
        stack->setCurrentIndex(0);
        //updateMenuHighlight(btnMenuConnections);
        highlightMenuButton(btnMenuConnections);

        // ✅ Rimuove l’icona quando si entra nella pagina Connections
        //btnMenuConnections->setIcon(QIcon());  // resetta l’icona

    });

    connect(btnMenuOperate, &QPushButton::clicked, this, [this]() {
        stack->setCurrentIndex(1);
        //updateMenuHighlight(btnMenuOperate);
        highlightMenuButton(btnMenuOperate);
    });
    connect(btnMenuQuit, &QPushButton::clicked, this, [this]() {
        highlightMenuButton(btnMenuQuit);
        QCoreApplication::quit();
    });

    setWindowState(Qt::WindowMaximized);   // Imposta la finestra in modalità full screen all'avvio

    #ifdef Q_OS_WIN
        registerForDeviceNotifications();
    #endif
}

MainWindow::~MainWindow()
{
    #ifdef Q_OS_WIN
        // Facciamo l'unregister (buona pratica)
        if (g_hDevNotify) {
            UnregisterDeviceNotification(g_hDevNotify); // Rimuove la registrazione, evitando che Windows continui a notificare una finestra ormai distrutta
            g_hDevNotify = nullptr;
        }
    #endif
}

// ========================================================== Definizione dei Metodi ==========================================================
#ifdef Q_OS_WIN
void MainWindow::registerForDeviceNotifications()
{

    HWND hwnd = reinterpret_cast<HWND>(winId()); // Trasforma l’ID della finestra MainWindow in HWND nativo, indispensabile per chiamare le API Win32
    if (!hwnd) {
        qWarning() << "Impossibile registrare le notifiche: HWND non valido!";
        return;
    }

    // La struttura DEV_BROADCAST_DEVICEINTERFACE ci permette di chiedere a Windows di mandarci notifiche per i “dispositivi USB”. QUindi questa struttura si comporta come un FILTRO per i dispositivi USB
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));                // azzera tutti i campi per sicurezza
    NotificationFilter.dbcc_size       = sizeof(DEV_BROADCAST_DEVICEINTERFACE); // indica la dimensione della struttura
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;            // dice che stiamo interessati ai dispositivi di tipo “Device Interface” (ad esempio USB, Bluetooth ecc.)

    // "GUID_DEVINTERFACE_USB_DEVICE" è la GUID ovvero un ID standard dei dispositivi USB
    // Includendola, chiediamo al sistema di notificarci quando un device USB arriva o viene rimosso
    NotificationFilter.dbcc_classguid  = GUID_DEVINTERFACE_USB_DEVICE;

    // Chiama la funzione di Windows "RegisterDeviceNotification" per associare
    //    la finestra (identificata da hwnd) al filtro "NotificationFilter"
    //    in modo da ricevere i messaggi WM_DEVICECHANGE relativi a dispositivi USB.
    g_hDevNotify = RegisterDeviceNotification(
        hwnd,                               // Finestra che vogliamo “ascolti” gli eventi
        &NotificationFilter,                // il filtro che descrive quali tipi di dispositivi e notifiche ci interessano
        DEVICE_NOTIFY_WINDOW_HANDLE         // Flag che significa: stiamo fornendo un HWND
        );

    // Controlla se la registrazione è avvenuta con successo
    if (!g_hDevNotify) {
        DWORD err = GetLastError(); // funzione che mi permette di scoprire il codice di errore di Windows
        qWarning() << "RegisterDeviceNotification fallita. Errore:" << err;
    } else {
        qDebug() << "Registrazione USB device notifications riuscita.";
    }
}
#endif

QWidget* MainWindow::createConnectionsPage()
{
    QWidget *widget = new QWidget;          // Crea la pagina principale
    QVBoxLayout *mainLayout  = new QVBoxLayout(widget); // Inserisco il widget in un layout verticale

    // ================== GRUPPO LAN ==================
    QGroupBox *grpLan = new QGroupBox("LAN"); // Creo la box che incornicerà la parte LAN
    QVBoxLayout *lanLayout = new QVBoxLayout; // Uso il layout verticale per organizzare le sezioni al suo interno
    lanLayout->setContentsMargins(10, 20, 10, 10);
    lanLayout->setSpacing(12);  // spaziatura uniforme tra le sezioni verticali
    grpLan->setLayout(lanLayout);

    grpLan->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid black;
            border-radius: 8px;
            margin-top: 20px;
            padding: 10px;
            font-weight: bold;
            font-size: 15px;
        }
        QGroupBox::title {
            color: black;
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
        }
    )");

    // === • Network Settings
    QLabel *lblNetwork = new QLabel("• Network Settings");
    lblNetwork->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;"); // Font differenziato
    lanLayout->addWidget(lblNetwork);

    QFormLayout *netLayout = new QFormLayout; // layout a due colonne per campi etichetta/valore
    netLayout->setContentsMargins(6, 4, 6, 4);

    // Etichette
    QLabel *lblPort = new QLabel("Port:");
    lblServer = new QLabel("Server not listening:");
    lblPort->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    lblServer->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");

    // Porta
    spnServerPort = new QSpinBox;
    spnServerPort->setRange(1024, 65535);
    spnServerPort->setValue(12345);
    netLayout->addRow(lblPort, spnServerPort);

    // Bottone da solo
    btnStart = new QPushButton("Start Server");
    btnStart->setStyleSheet(R"(
        QPushButton {
            background-color: lightgray;
            color: black;
        }
    )");
    netLayout->addRow("", btnStart); // pulsante da solo senza etichetta

    // Pallino
    lblConnectionStatus = new QLabel;
    lblConnectionStatus->setFixedSize(25, 25);
    lblConnectionStatus->setStyleSheet(R"(
        #lblConnectionStatus[state = "0"] {
            background-color: red;
            border-radius: 12px;
            border: 2px solid white;
        }
        #lblConnectionStatus[state = "1"] {
            background-color: green;
            border-radius: 12px;
            border: 2px solid white;
        }
    )");
    lblConnectionStatus->setProperty("state", "0");
    lblConnectionStatus->setObjectName("lblConnectionStatus");

    // Layout orizzontale per Server: [pallino]
    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(lblConnectionStatus);
    netLayout->addRow(lblServer, statusLayout);
    lanLayout->addLayout(netLayout);

    // === Separatore: Linea orizzontale per dividere visivamente le sezioni
    QFrame *line1 = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    lanLayout->addWidget(line1);

    // === • Console
    QLabel *lblConsole = new QLabel("• Console");
    lblConsole->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    lanLayout->addWidget(lblConsole);

    QVBoxLayout *consoleLayout = new QVBoxLayout;
    consoleLayout->setContentsMargins(6, 4, 6, 4);
    lstConsole = new QListWidget;
    btnClear = new QPushButton("Clear");
    btnClear->setMinimumHeight(30);
    btnClear->setStyleSheet(R"(
        QPushButton {
            background-color: lightgray;
            color: black;
        }
    )");
    consoleLayout->addWidget(lstConsole);
    consoleLayout->addWidget(btnClear);
    lanLayout->addLayout(consoleLayout);

    // === Separatore
    QFrame *line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    lanLayout->addWidget(line2);

    // === • Send Message
    QLabel *lblSend = new QLabel("• Send Message");
    lblSend->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    lanLayout->addWidget(lblSend);

    QFormLayout *msgLayout = new QFormLayout;
    msgLayout->setContentsMargins(6, 4, 6, 4);
    lnMessage = new QLineEdit;
    lnMessage->setMinimumHeight(30);
    /*lnMessage->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #444;
            border-radius: 6px;
            padding: 4px 8px;
            font-size: 14px;
        }
    )");*/

    btnSendToAll = new QPushButton("Send To All Clients");
    btnSendToAll->setMinimumHeight(30);
    btnSendToAll->setStyleSheet(R"(
        QPushButton {
            background-color: lightgray;
            color: black;
        }
    )");

    //msgLayout->addRow("Message:", lnMessage);
    QLabel *lblMessage = new QLabel("Message:");
    lblMessage->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    msgLayout->addRow(lblMessage, lnMessage);
    msgLayout->addRow("", btnSendToAll);
    lanLayout->addLayout(msgLayout);

    lanLayout->addStretch();

    // === GRUPPO USB
    QGroupBox *grpUsb = new QGroupBox("USB");
    QVBoxLayout *usbLayout = new QVBoxLayout;
    usbLayout->setContentsMargins(10, 20, 10, 10);
    usbLayout->setSpacing(12);  // spaziatura uniforme tra le sezioni
    grpUsb->setLayout(usbLayout);

    grpUsb->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid black;
            border-radius: 8px;
            margin-top: 20px;
            padding: 10px;
            font-weight: bold;
            font-size: 15px;
        }
        QGroupBox::title {
            color: black;
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
        }
    )");

    // === • Tablet Connection
    QLabel *lblConnection = new QLabel("• Tablet Status");
    lblConnection->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    usbLayout->addWidget(lblConnection);

    QFormLayout *layout = new QFormLayout; // layout a due colonne per campi etichetta/valore
    layout->setContentsMargins(6, 4, 6, 4);

    usbStatusLabel = new QLabel("Tablet Status:");
    usbStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");

    // Crea il pallino nero
    usbStatusIndicator = new QLabel;
    usbStatusIndicator->setFixedSize(25, 25);
    usbStatusIndicator->setStyleSheet(R"(
        QLabel {
            border-radius: 12px;
            border: 2px solid white;
        }
    )");

    // Allinea Status + Pallino orizzontalmente
    QHBoxLayout *usbStatusLayout = new QHBoxLayout;
    usbStatusLayout->addWidget(usbStatusIndicator);
    layout->addRow(usbStatusLabel, usbStatusLayout);
    usbLayout->addLayout(layout);

    // === LAYOUT COMPLESSIVO
    mainLayout->addWidget(grpLan);
    mainLayout->addWidget(grpUsb);
    mainLayout->addStretch();

    // === CONNECTIONS
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::on_btnStartServer_clicked);
//    connect(btnSendToAll, &QPushButton::clicked, this, &MainWindow::on_btnSendToAll_clicked);
    connect(btnClear, &QPushButton::clicked, lstConsole, &QListWidget::clear);

    return widget;
}


QWidget* MainWindow::createOperatePage()
{
    // Inizializza l'area di disegno (DrawingArea una mia classe custom)
    drawingArea = new DrawingArea(this);

    // Gestione dello Scaling Factor
    drawingArea->setFocus(); // Questo garantirà che, all’entrata nella pagina "Operate", drawingArea riceva gli eventi da tastiera

    QWidget *drawingContainer = new QWidget;
    QVBoxLayout *drawingContainerLayout = new QVBoxLayout(drawingContainer);
    drawingContainerLayout->setContentsMargins(0, 0, 0, 0);
    drawingContainerLayout->addWidget(drawingArea); // Incapsula drawingArea in un QWidget (chiamato drawingContainer)

    // === Pennino ===
    FreeHandStatusLabel = new QLabel("Free Hand:");
    FreeHandStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    FreeHandStatusIndicator = new QLabel;
    FreeHandStatusIndicator->setFixedSize(25, 25);
    FreeHandStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;"); // stato iniziale: non rilevato

    // === Record Trajectory ===
    recordStatusLabel = new QLabel("Record Trajectory:");
    recordStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    recordStatusIndicator = new QLabel;
    recordStatusIndicator->setFixedSize(25, 25);
    recordStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");

    // === Repeat Trajectory ===
    repeatStatusLabel = new QLabel("Repeat Trajectory:");
    repeatStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    repeatStatusIndicator = new QLabel;
    repeatStatusIndicator->setFixedSize(25, 25);
    repeatStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");

    // === Fill Trajectory ===
    scanStatusLabel = new QLabel("Scan Trajectory:");
    scanStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    scanStatusIndicator = new QLabel;
    scanStatusIndicator->setFixedSize(25, 25);
    scanStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");

    // === Scale Factor ===
    scaleFactorLabel = new QLabel("Scale Factor:");
    scaleFactorLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    scaleFactorDisplay = new QLCDNumber;
    scaleFactorDisplay->setDigitCount(4); //  permette "0.00" fino a "9.99" senza troncamento
    scaleFactorDisplay->setSegmentStyle(QLCDNumber::Flat);
    scaleFactorDisplay->setFixedHeight(25);
    scaleFactorDisplay->setFixedWidth(80);
    scaleFactorDisplay->display(1.00); // valore iniziale
    scaleFactorDisplay->setStyleSheet("background-color: white; border: 1px solid gray;  color: black;"); // imposta sfondo bianco + bordo grigio
    scaleFactorDisplay->display(QString::number(1.00, 'f', 2));  // forza due cifre decimali

    // === Coordinate del pennino ===
    penCoordsLabel = new QLabel("Pen Coordinates:");
    penCoordsLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    penCoordsBox = new QLineEdit;
    penCoordsBox->setReadOnly(true);
    penCoordsBox->setFixedHeight(25);
    penCoordsBox->setFixedWidth(180);
    penCoordsBox->setStyleSheet("background-color: white; border: 1px solid gray;");

    // === Stato Operativo del CALM ===
    workingStatusLabel  = new QLabel("CALM:");
    workingStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    workingStatusIndicator = new QLabel;
    workingStatusIndicator->setFixedSize(25, 25);
    workingStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");

    //  Bottone calibrazione ===
    //btnCalibration = new QPushButton("Calibrazione pixel/mm");
    btnCalibration->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");
    lblStrokeLength = new QLabel("L = 0.00 mm");
    lblStrokeLength->setStyleSheet("font-family: 'Segoe UI'; font-size: 15px;  color: black; font-weight: bold;");

    // Separatore visivo tra area e barra di stato
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine); // linea orizzontale
    separator->setFrameShadow(QFrame::Sunken); // effetto affondato (3D)
    separator->setLineWidth(1); // spessore linea
    separator->setStyleSheet("color: #888; margin-top: 10px; margin-bottom: 10px;");     // colore grigio scuro

    // Layout della barra di stato
    QHBoxLayout *statusBarLayout = new QHBoxLayout;
    statusBarLayout->setContentsMargins(10, 5, 10, 5); // Margini interni per non schiacciare il contenuto della barra di stato
    statusBarLayout->addWidget(workingStatusLabel);
    statusBarLayout->addWidget(workingStatusIndicator);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(FreeHandStatusLabel);
    statusBarLayout->addWidget(FreeHandStatusIndicator);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(recordStatusLabel);
    statusBarLayout->addWidget(recordStatusIndicator);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(repeatStatusLabel);
    statusBarLayout->addWidget(repeatStatusIndicator);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(scanStatusLabel);
    statusBarLayout->addWidget(scanStatusIndicator);
    //statusBarLayout->addWidget(btnCalibration);


    statusBarLayout->addStretch();
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(scaleFactorLabel);
    statusBarLayout->addWidget(scaleFactorDisplay);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(penCoordsLabel);
    statusBarLayout->addWidget(penCoordsBox);
    statusBarLayout->addSpacing(20);
    statusBarLayout->addWidget(lblStrokeLength);


    // Widget contenitore per lo sfondo grigio della barra
    QWidget *statusBarWidget = new QWidget;
    statusBarWidget->setLayout(statusBarLayout);
    statusBarWidget->setStyleSheet(
        "background-color: lightgray; "
        "border-radius: 6px;"
    );

    // Contenitore principale
    QGroupBox *drawingGroup = new QGroupBox("Operating Area", this);
    QVBoxLayout *drawingLayout = new QVBoxLayout(drawingGroup);
    drawingLayout->addWidget(drawingContainer, 95);     // 90%
    drawingLayout->addWidget(separator, 0);            // separatore
    drawingLayout->addWidget(statusBarWidget, 5);      // 10%

    // Applica uno stile grafico al contenitore
    drawingGroup->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid black;
            border-radius: 8px;
            margin-top: 20px;
            padding: 10px;
            font-weight: bold;
            font-size: 15px;
        }
        QGroupBox::title {
             color: black;
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
        }
    )");

    //connect(drawingArea, &DrawingArea::buttonPressed, this, &MainWindow::showButtonPressed);
    connect(drawingArea, &DrawingArea::scaleFactorChanged,  this, &MainWindow::updateScaleFactor);
    //connect(drawingArea, &DrawingArea::penPositionUpdated,  this, &MainWindow::updateCoordinateLabel);
    connect(drawingArea, &DrawingArea::stateChanged,        this, &MainWindow::onDrawingStateChanged);

    connect(btnCalibration, &QPushButton::clicked, this, [this]() {
        wacomcalibrator *dialog = new wacomcalibrator(drawingArea, this);
        dialog->show();
    });

    // forza lo stato iniziale
    onDrawingStateChanged(DrawingState::FreeHand);

    return drawingGroup;
}

void MainWindow::highlightButtons(QPushButton *activeButton)
{
    QList<QPushButton*> buttons = {btnStart, btnSendToAll, btnClear};

    for (QPushButton *btn : buttons) {
        if (btn == activeButton) {
            btn->setStyleSheet("background-color: rgb(134,124,126); font-weight: bold;");

            // Verifica se il pulsante premuto è uno di quelli da evidenziare temporaneamente
            if (btn == btnSendToAll || btn == btnClear || btn == btnStart) {
                // Ripristina lo stile dopo 100 ms
                QTimer::singleShot(100, this, [btn]() {
                    btn->setStyleSheet("");  // reset stile
                });
            }
        } else {
            // Tutti gli altri pulsanti vengono riportati allo stile predefinito
            btn->setStyleSheet("");
        }
    }
}

void MainWindow::highlightMenuButton(QPushButton *activeButton)
{
    QList<QPushButton*> menuButtons = {btnMenuConnections, btnCalibration, btnMenuOperate, btnMenuQuit};

    for (QPushButton *btn : menuButtons) {
        if (btn == activeButton) {
            btn->setStyleSheet("background-color: rgb(134,124,126); font-weight: bold;");
        } else {
            btn->setStyleSheet("");
        }
    }
}

void MainWindow::stopServer()
{
    // Funzione chiamata dallo slot on_btnStartServer_clicked() invocato quando l'utente clicca sul pulsante "Stop Server"
    if (_server) {
        delete _server;                                 // Dealloca la memoria del server =>  tutti i socket ancora connessi vengono automaticamente chiusi da Qt
        _server = nullptr;                              // Azzera il puntatore per sicurezza
    }

    // Aggiorna l’indicatore visivo della GUI a "disconnesso"
    lblConnectionStatus->setProperty("state", "0");
    lblConnectionStatus->style()->polish(lblConnectionStatus);

    // Riporta il bottone allo stato iniziale
    btnStart->setText("Start Server");
    lblServer->setText("Server not listening");
    clientDisconnected();
}

// ========================================================== Definizione degli Slot ==========================================================

void MainWindow::onDrawingStateChanged(DrawingState newState)
{
    // reset indicatori
    recordStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");
    repeatStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");
    FreeHandStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");
    scanStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");

    switch (newState) {
    case DrawingState::FreeHand:
        FreeHandStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
        break;
    case DrawingState::Recording:
        recordStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
        break;
    case DrawingState::Repeating:
        repeatStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
        break;
    case DrawingState::Scanning:
        scanStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
        break;
    }
}

void MainWindow::updateScaleFactor(float newScaleFactor)
{
    scaleFactorDisplay->display(QString::number(newScaleFactor, 'f', 2));
}

void MainWindow::updateCoordinateLabel(QPointF pos)
{
    QFont font("Segoe UI", 8, QFont::Bold);
    penCoordsBox->setFont(font);
    penCoordsBox->setAlignment(Qt::AlignCenter); // centra il testo nella QLabel

    penCoordsBox->setText(QString("X: %1, Y: %2")
                              .arg(pos.x(), 0, 'f', 4)
                              .arg(pos.y(), 0, 'f', 4));

    connect(drawingArea, &DrawingArea::strokeLengthUpdated, this, [this](float length) {
        lblStrokeLength->setText(QString("Length: %1 mm").arg(length, 0, 'f', 2));
    });
}


void MainWindow::onWacomUSBConnected()
{
    // 1) Aggiorniamo la label, ad esempio
    usbStatusLabel->setText("Connected");
    // Se vuoi un’icona accesa o colorata, puoi fare:
    // usbStatusLabel->setPixmap(QPixmap(":/images/usb_on.png"));
    usbStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    usbStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
    // 2) Se vuoi fare altro, tipo log o ulteriori azioni:
    //qDebug() << "Wacom collegato, deviceId:" << deviceId;
}

void MainWindow::onWacomUSBDisconnected()
{
    usbStatusLabel->setText("Not Connected");
    // usbStatusLabel->setPixmap(QPixmap(":/images/usb_off.png"));
    usbStatusLabel->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px;  color: black;");
    usbStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");
    //qDebug() << "Wacom scollegato, deviceId:" << deviceId;
}

void MainWindow::on_btnStartServer_clicked()
{
    highlightButtons(btnStart);
    // Caso 1: il server è già attivo → dobbiamo fermarlo
    if (_server && _server->isStarted()) {                // Controlla che il puntatore _server esiste (cioè non è nullptr) e che il server sia in ascolto sulla porta (cioè listen() ha avuto successo)
        lblServer->setText("Server listening");           // Aggiorna la label lblServer per mostrare "Server listening"
        stopServer();                                     // Richiama la funzione che spegne il server facendo disconnettere tutti i client
        return;                                           // Esce dalla funzione: niente altro da fare
    }

    // Caso 2: server NON attivo → dobbiamo avviarlo
    int port = spnServerPort->value();                    // Legge il numero di porta scelto dall’utente nel QSpinBox spnServerPort
    //_server = new MyTcpServer(port);                      // Crea una nuova istanza della classe MyTcpServer e dato che per definizione il costruttore di questa classe vuole in input un int port allora passa come argomento il valore port al suo costruttore
                                                          // Non appena creo questa nuova istanza allora viene eseguito il codice del costruttore di MyTcpServer
    _server = new MyUdpServer(port);

    // Colleghiamo i segnali del server ai rispettivi slot della MainWindow
    //connect(_server, &MyTcpServer::newClientConnected, this, &MainWindow::newClientConnected);
    connect(_server, &MyUdpServer::newClientConnected, this, &MainWindow::newClientConnected);
    //connect(_server, &MyTcpServer::dataReceived,       this, &MainWindow::clientDataReceived);
    connect(_server, &MyUdpServer::dataReceived,       this, &MainWindow::clientDataReceived);
    //connect(_server, &MyTcpServer::clientDisconnect,   this, &MainWindow::clientDisconnected);
    connect(_server, &MyUdpServer::clientDisconnect,   this, &MainWindow::clientDisconnected);
    //connect(drawingArea, &DrawingArea::penPositionUpdated,  _server, &MyTcpServer::sendPenPosition);
    connect(drawingArea, &DrawingArea::penPositionUpdated,  _server, &MyUdpServer::sendPenPosition);

    connect(_server, &MyUdpServer::UpdatePenPositionLabel, this, &MainWindow::updateCoordinateLabel);

    //connect(drawingArea, &DrawingArea::sendFullTrajectory,
    //        _server, &MyTcpServer::sendFullTrajectoryToClient);


    // Verifica se il server si è avviato correttamente
    /*auto state = (_server->isStarted()) ? "1" : "0";
    lblConnectionStatus->setProperty("state", state);
    lblConnectionStatus->style()->polish(lblConnectionStatus);*/
    bool started = (_server->isStarted()); // true se listen() è andato a buon fine
    QString state = started ? "1" : "0"; // "1" = verde, "0" = rosso
    lblConnectionStatus->setProperty("state", state); // aggiorna proprietà personalizzata
    lblConnectionStatus->style()->polish(lblConnectionStatus); // forza l'aggiornamento visivo

    // Aggiorna il testo del bottone in base allo stato ottenuto
    btnStart->setText(started ? "Stop Server" : "Start Server");
    lblServer->setText(started ? "Server listening" : "Server not listening");
}

/*void MainWindow::on_btnSendToAll_clicked()
{
    highlightButtons(btnSendToAll);
    QString message = lnMessage->text().trimmed();
    if (_server) _server->sendToAll(message);
}*/

void MainWindow::newClientConnected()
{
    lstConsole->addItem("New client connected");
    workingStatusIndicator->setStyleSheet("background-color: green; border-radius: 12px; border: 2px solid white;");
    //drawingArea->setSendMode(DrawingArea::SendMode::Batch);

}

void MainWindow::clientDataReceived(QString message)
{
    lstConsole->addItem(message);

    // Se non siamo già nella pagina Connections, mostra una notifica
    /*if (stack->currentIndex() != 0) {
        QIcon alertIcon(":/icons/alert.PNG");
        if (!alertIcon.isNull()) {
            btnMenuConnections->setIcon(alertIcon);
            btnMenuConnections->setIconSize(QSize(16, 16));
        } else {
            qWarning() << "Icona non trovata!";
        }       // 3. imposta dimensione visiva
    }*/
}

void MainWindow::clientDisconnected()
{
    lstConsole->addItem("Client Disconnected");
    workingStatusIndicator->setStyleSheet("background-color: red; border-radius: 12px; border: 2px solid white;");
    drawingArea->setSendMode(DrawingArea::SendMode::Live);
}

