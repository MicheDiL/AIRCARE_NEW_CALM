/*
 * Author: Michele Di Lucchio
 *  06/05/25
    MainWindow è il cuore dell’interfaccia che fornisce una GUI user-friendly.
    Al suo interno è presente un menù con le voci "Connection" ed "Operate":
        - Connection: finestra per avviare il Server e verificare le connessioni USB
        - Operate:    finestra operative per muovere il laser
    Interagisce attivamente con il server TCP:
        - Avvia e ferma il server
        - Invia messaggi testuali ai client (sendToAll)
        - Manda le coordinate del pennino ricevute da DrawingArea al server TCP, che le inoltra ai client
    Inoltre la MainWindow viene registrata  per ricevere le notifiche di hotplug USB su Windows.
    Questo è necessario per far sì che il sistema operativo invii i messaggi WM_DEVICECHANGE alla finestra,
    e di conseguenza alla classe usbmanager che intercetta questi messaggi/eventi e riconosce se è stata
    collegata/scollegata la tavoletta Wacom.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>                          // Finestra principale Qt
#include <QPushButton>                          // Pulsanti GUI
#include <QSpinBox>                             // Selezione numeri interi (es. porta)
#include <QLineEdit>                            // Campo di testo per messaggi
#include <QLabel>                               // Etichette GUI
#include <QListWidget>                          // Console visuale dei messaggi
#include <QGroupBox>                            // Contenitori con bordo e titolo
#include <QStackedWidget>                       // Serve per creare un'area centrale come quella in cui mostriamo i pannelli del menù (Pagine multiple GUI)
#include <QVBoxLayout>                          // Layout grafici
#include <QFormLayout>                          // Layout grafici
#include <QHBoxLayout>                          // Layout grafici
#include <QStyle>                               // Per accedere allo stile del sistema
#include <QCoreApplication>                     // Per gestire eventi e uscita dall'app
#include <QApplication>                         // Accesso a QApplication globale
#include <QTimer>
#include <QLCDNumber>
#include <QCheckBox>

//#include "MyTcpServer.h"                        // Modulo che gestisce il server TCP
#include "myudpserver.h"
#include "drawingarea.h"                        // Modulo che rappresenta l'area di disegno

class MainWindow : public QMainWindow           // Eredita da QMainWindow, la classe base Qt per finestre con barra dei menu, barra di stato, widget centrale
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

#ifdef Q_OS_WIN // Codice condizionale per Windows
    void registerForDeviceNotifications();      // Registriamo la finestra per le notifiche di hotplug USB
#endif

public slots:
    // Questi slot devono essere `public` per ricevere segnali provenienti dal modulo drawingarea
    //void showExpressKeyPressed(QString keyName);    // Mostra nella GUI quale ExpressKey è stata premuta

    void updateScaleFactor(float newScaleFactor);   // Aggiorna  nella GUI il fattore di scala
    void updateCoordinateLabel(QPointF pos);        // Per visualizzare le coordiante del pennino nella tool bar

    // Slot che rispondono ad eventi USB che possono essere invocati da segnali interni alla classe o da eventi di sistema
    void onWacomUSBConnected();
    void onWacomUSBDisconnected();

private slots:
    // Slot che rispondono ad eventi utente fatti nella MainWindow (click)
    void on_btnStartServer_clicked();
//    void on_btnSendToAll_clicked();
    void onDrawingStateChanged(DrawingState newState);

    // Slot che rispondono a segnali dal server TCP
    void newClientConnected();
    void clientDisconnected();
    void clientDataReceived(QString message);

private:
    //MyTcpServer *_server;                       // Collegamento al server TCP
    MyUdpServer *_server;
    DrawingArea *drawingArea;                   // Collegamento all'area operativa

    QList<QPointF> recordedTrajectory;          // Memorizza i punti di un disegno che vuoi ripetere

    ///// Pulsanti del menu /////
    QPushButton *btnMenuConnections;
    QPushButton *btnMenuOperate;
    QPushButton *btnMenuQuit;

    // Stack centrale
    QStackedWidget *stack;                      // Widget che contiene più schermate, ma ne mostra solo una alla volta simulando un "cambio pagina"

    // Pannelli dedicati alle opzioni del menù
    QWidget *createConnectionsPage();
    QWidget *createOperatePage();  // area di disegno

    ///// PANNELLO PER LE CONNESSIONI /////

    /// LAN
    // Indicatori grafici
    QSpinBox *spnServerPort;                    // Selezione porta
    QLabel *lblConnectionStatus;                // Stato connessione server (pallino)
    QPushButton *btnStart;                      // avvia/arresta server
    QLabel *lblServer;                          // stato testuale server

    // Send message
    QLineEdit *lnMessage;                       // campo messaggio
    QPushButton *btnSendToAll;                  // invio a tutti i Client

    // Console
    QListWidget *lstConsole;                     // console dei messaggi
    QPushButton *btnClear;                       // svuota console

    /// USB
    // Indicatori grafici
    QLabel *usbStatusLabel;                 // Stato USB (testo)
    QLabel *usbStatusIndicator;             // Stato USB (pallino)

    ///// PANNELLO PER L'AREA OPERATIVA /////
    // Indicatori grafici
    QLabel *FreeHandStatusLabel;            // testo
    QLabel *FreeHandStatusIndicator;        // pallino
    QLabel *recordStatusLabel;
    QLabel *recordStatusIndicator;
    QLabel *repeatStatusLabel;
    QLabel *repeatStatusIndicator;
    QLabel *workingStatusLabel;
    QLabel *workingStatusIndicator;
    QLabel *scanStatusLabel;                // nuovo indicatore per il riempimento della traiettoria
    QLabel *scanStatusIndicator;
    QLabel *scaleFactorLabel;               // testo
    QLCDNumber *scaleFactorDisplay;         // display LCD
    QLabel *penCoordsLabel;                 // testo
    QLineEdit *penCoordsBox;                // box

    QPushButton *btnCalibration;
    QLabel *lblStrokeLength;

    ///// METODI DELLA CLASSE /////
    void highlightButtons(QPushButton *activeButton);
    void highlightMenuButton(QPushButton *activeButton);
    void stopServer();                                  //  Metodo interno per fermare e distruggere l’istanza del server.
};

#endif // MAINWINDOW_H
