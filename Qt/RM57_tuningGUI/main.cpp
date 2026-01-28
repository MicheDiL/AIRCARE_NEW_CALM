#include "mainwindow.h"
#include <QApplication>
#include "protocol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Inizializza l’event loop Qt per GUI e oggetti

    /* già qui si vede la “thread-safe event-driven pipeline”: i tipi custom sono
     * registrati prima che i segnali inizino a viaggiare */
    qRegisterMetaType<TelemetryType>("TelemetryType");
    qRegisterMetaType<TelemetrySample>("TelemetrySample");  // Fondamentale perché TelemetrySample viaggia in signals/slots cross-thread. Senza registrazione, Qt non può serializzare il tipo nella queued connection
    qRegisterMetaType<QVector<TelemetrySample>>("QVector<TelemetrySample>");

    MainWindow w;
    w.show();
    return a.exec();
}
