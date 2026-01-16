/*
 * Author: Michele Di Lucchio
 *  04/04/25
 *  Classe progettata come un contenitore di funzioni “utility” in questo caso,
 *  la funzione che controlla se la tavoletta Wacom (con determinati VID/PID) è
 *  già connessa tramite USB prima dell’avvio dell’applicazione. Dato che questa classe è concepita
 *  solo per fornire funzioni statiche, ovvero funzioni che possono essere chiamate senza creare
 *  un oggetto allora:
 *  - questa classe non ha costruttori (né costruttore di default, né costruttori con parametri)
 *      dato che non ci serve istanziare questa classe e non vogliamo gestire
 *      dati membri o il suo stato interno.
 */

#ifndef USBSTARTCONNECTION_H
#define USBSTARTCONNECTION_H

#include <QString>

#ifdef Q_OS_WIN
// Include delle SetupAPI
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <usbiodef.h>
#include <initguid.h>

#ifdef _MSC_VER // se utilizzo il compilatore Microsoft (ad esempio MSVC in VisualStduio).
#pragma comment(lib, "setupapi.lib") // istruisce il linker ad aggiungere la libreria “setupapi.lib” durante la fase di linking
/* Nel caso attuale, dato che utilizziamo il compilatore di Qt (MinGW), allora affinchè il compilatore includa la libreria "setupapi"
         * di Windows ho dovuto includere tale libreria nel file .pro*/
#endif
#endif

class WacomChecker
{
public:
    // Metodo statico che restituisce true se una tavoletta Wacom (VID/PID) è già connessa
    static bool isWacomAlreadyConnected(); // questo metodo usa le SetupAPI di Windows
};

#endif // USBSTARTCONNECTION_H
