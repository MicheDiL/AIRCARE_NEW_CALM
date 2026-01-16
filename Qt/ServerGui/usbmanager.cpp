#include "usbmanager.h"
#include <QDebug>

bool UsbManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *)
{
#ifdef Q_OS_WIN

    if (eventType == "windows_generic_MSG") {     // Su Windows, Qt utilizza eventType impostato a "windows_generic_MSG" per i messaggi di finestra
        // Una volta ricevuto il messaggio bisogna eseguire il cast dell'oggetto alla struttura MSG
        // e poi leggerne i campi per capire di che messaggio si tratta e come gestirlo
        MSG *msg = static_cast<MSG*>(message);

        if (msg->message == WM_DEVICECHANGE)      // Intercettiamo WM_DEVICECHANGE inviato dal sistema operativo Windows quando un dispositivo hardware cambia stato (arrivo, rimozione, ecc.)
        {
            // wParam = tipo di cambiamento
            // lParam = info sul device

            // Leggo l'eventCode (wParam)
            UINT eventCode = static_cast<UINT>(msg->wParam);

            switch (eventCode) {
            case DBT_DEVICEARRIVAL: { // Un device USB è stato connesso

                QString deviceId = QString::fromWCharArray(L"unknown"); // inizializzo la variabile deviceId con il testo "unknown"

                // Controllo i dettagli del device connesso in lParam
                PDEV_BROADCAST_HDR lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam); // ottengo il puntatore a DEV_BROADCAST_HDR che ci dice il tipo di device
                if (lpdb && lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) // verifico se si tratta di una periferica di interfaccia (USB)
                {
                    PDEV_BROADCAST_DEVICEINTERFACE dbi = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(lpdb);
                    deviceId = QString::fromWCharArray(dbi->dbcc_name); // dbcc_name contiene la stringa di identificazione del device di cui ne otteniamo il testo in un QString chiamto deviceId
                }

                qDebug() << "[USB] Device connected:" << deviceId;

                // Controllo che la stringa deviceId contenga i parametri VID e PID caratteristici della tavoletta Wacom
                if(isCALMWacomTablet(deviceId)){
                    emit usbDeviceArrived(deviceId); // emetto il segnale e avviso altre parti del codice Qt (via connect(...))
                }

            }
            break;

            case DBT_DEVICEREMOVECOMPLETE: { // Un device USB è stato disconnesso

                QString deviceId = QString::fromWCharArray(L"unknown");

                PDEV_BROADCAST_HDR lpdb = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
                if (lpdb && lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    PDEV_BROADCAST_DEVICEINTERFACE dbi = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(lpdb);
                    deviceId = QString::fromWCharArray(dbi->dbcc_name);
                }

                qDebug() << "[USB] Device removed:" << deviceId;

                if(isCALMWacomTablet(deviceId)){
                    emit usbDeviceRemoved(deviceId);
                }
            }
            break;

            default:
                // Altri tipi di device change
                break;
            }
        }
    }
#endif
    // Restituisci false per far proseguire la propagazione dell’evento
    return false;
}

bool UsbManager::isCALMWacomTablet(const QString &deviceId)
{
    // Controlliamo la presenza della stringa "VID_056A" e "PID_0376" in deviceId
    if (deviceId.contains("VID_056A", Qt::CaseInsensitive) && deviceId.contains("PID_0376", Qt::CaseInsensitive))
    {
        // Se vuoi filtrare ulteriormente in base al numero seriale (così facendo, solo una specifica tavoletta verrà riconosciuta):
        // if (!deviceId.contains("8FH00S2007284", Qt::CaseInsensitive)) {
        //     return false; // non corrisponde esattamente al serial
        // }
        return true;
    }
    return false;
}
