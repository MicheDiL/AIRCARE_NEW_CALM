/*
    Author: Michele Di Lucchio
    02/04/2025
    Obiettivo: Questa classa rappresenta un FILTRO che intercetta i messaggi nativi di Windows
                (messaggi Win32) relativi alle connessioni USB prima che vengano elaborati da Qt.
               Ci permette di capire se è stata collegata/scollegata via USB la tevoletta Wacom.
*/

#ifndef USBMANAGER_H
#define USBMANAGER_H


#include <QAbstractNativeEventFilter>   // Classe che ci consente di intercettare i messaggi nativi del sistema operativo (in questo caso Windows)
#include <QObject>                      // Se vuoi emettere segnali, questa classe deve ereditare (anche) da QObject  e avere il macro Q_OBJECT
#include <windows.h>                    // Per i messaggi e codici nativi Win32 come MSG, WM_DEVICECHANGE, ecc.
#include <dbt.h>                        // Contiene le costanti, le strutture e le definizioni necessarie per gestire le notifiche relative ai dispositivi hardware, come ad esempio l’inserimento o la rimozione di periferiche (USB, drive esterni, dispositivi di input ecc.)
//#include <QByteArray>

class UsbManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit UsbManager(QObject *parent = nullptr) : QObject(parent) {}

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;        // Metodo chiamato automaticamente da Qt ogni volta che c’è un messaggio di finestra nativo (Win32)

private:
    bool isCALMWacomTablet(const QString &deviceId);                                                    // Metodo che rileva se il Wacom Tablet è connesso o disconnesso

signals:
    // Segnali che vengono emessi quando connettiamo o disconnettiamo periferiche USB
    void usbDeviceArrived(QString deviceId);
    void usbDeviceRemoved(QString deviceId);
};

#endif // USBMANAGER_H
