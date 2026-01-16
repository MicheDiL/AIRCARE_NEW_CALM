//#include "penmanager.h" // non è più necessaria perché gestisci lo stato del pennino tramite DrawingArea::drawingStateChanged(bool)
#include "mainwindow.h"
#include "usbmanager.h"
#include "usbstartconnection.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // Istanzio la mia applicazione usando la classe PenManger che eredita da QApplication
    //PenManager calmApp(argc, argv);
    QApplication CalmGuiApp(argc, argv);

    // Controllo all’avvio se la tavoletta Wacom è già connessa via USB
    bool wacomConnected = WacomChecker::isWacomAlreadyConnected();

    // Creiamo la finestra principale
    MainWindow mainWindow;

    // Assegno alla mia applicazione il riferimento alla MainWindow (e, se vuoi, alla DrawingArea)
    //calmApp.setMainWindow(&mainWindow);

    /* Creo ed installo il filtro per ricevere i messaggi di sistema sulle connessioni USB.
     In questo modo, ogni volta che Windows manda un WM_DEVICECHANGE, passerà da usbmanager*/
    //UsbManager *usbFilter = new UsbManager(&calmApp); // creo il filtro
    //calmApp.installNativeEventFilter(usbFilter); // installo il filtro in modo che tutte le finestre e tutti i messaggi passino anche da lì
    UsbManager *usbFilter = new UsbManager(&CalmGuiApp); // creo il filtro
    CalmGuiApp.installNativeEventFilter(usbFilter); // installo il filtro in modo che tutte le finestre e tutti i messaggi passino anche da lì

    // 6) Collega i segnali "device arrived/removed" agli slot di MainWindow
    QObject::connect(usbFilter, &UsbManager::usbDeviceArrived,
                     &mainWindow, &MainWindow::onWacomUSBConnected);
    QObject::connect(usbFilter, &UsbManager::usbDeviceRemoved,
                     &mainWindow, &MainWindow::onWacomUSBDisconnected);

    // Aggiorna subito la GUI se Wacom era già connesso
    if (wacomConnected) {
        mainWindow.onWacomUSBConnected();
    } else {
        mainWindow.onWacomUSBDisconnected();
    }

    // Mostriamo la finestra principale
    mainWindow.show();
    // Avviamo l’event loop
    //return calmApp.exec();
    return CalmGuiApp.exec();
}
