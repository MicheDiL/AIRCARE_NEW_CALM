/* SerialWorker è l’oggetto che vive nel thread I/O e:

- crea e possiede QSerialPort (thread affinity corretta)
- apre/chiude la porta
- riceve bytes (readyRead) e li accumula in _rx
- ricostruisce i pacchetti con un parser robusto (resync)
- emette telemetryReceived(TelemetrySample) verso la GUI
- espone writeBytes(QByteArray) per mandare tuning al MCU

QSerialPort nel worker thread e non in GUI perché l’I/O e il parsing possono generare burst e
latenza; tenerli fuori dal GUI thread evita freeze, e con queued signals garantisco thread-safety
e affinità corretta degli oggetti Qt
*/

#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>      // base di tutto il mondo Qt (signals/slots, proprietà, thread affinity)
#include <QSerialPort>  // oggetto Qt che incapsula una porta seriale (apertura/lettura/scrittura asincrona)
#include <QByteArray>   // contenitore di bytes. Serve sia per accumulare stream RX sia per inviare pacchetti binari.
#include <QElapsedTimer>
#include "protocol.h"
// Nel worker aggiungi una coda e un timer di flush
#include <QVector>
#include <QTimer>
// Facciamo il log dei dati di telemetria per analisi offline
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

class SerialWorker : public QObject
{
    Q_OBJECT                // macro che abilita il meta-object system (signals/slots, introspezione)
public:
    explicit SerialWorker(QObject *parent = nullptr);   // SerialWorker eredita da QObject, quindi ha thread affinity e può usare signals/slots. Il parent qui è nullo (poichè lo crei in MainWindow), e non influenza la thread-safety perché poi fai moveToThread
    ~SerialWorker();

public slots:
    /* Queste funzioni verranno chiamate dalla GUI con connessione Qt::QueuedConnection (perché thread diversi).
    In pratica: la GUI “chiede” e il worker “esegue” nel suo thread.*/
    void openPort(const QString& portName, int baud);
    void closePort();
    void writeBytes(const QByteArray& data);        // tuning verso MCU
    void startBatching();   // avvia il timer nel thread del worker
    void stopBatching();    // opzionale

    // Slot per il log
    void setLogDirectory(const QString& dir);   // Slot chiamato nel caso in cui l'utente cambi la directory di log durante una registrazione in corso
    void setRecording(TelemetryType type, bool enabled);

signals:
    // eventi verso la GUI
    void connectedChanged(bool ok, QString reason);      // notifica a MainWindow se connessione è ok oppure fallita (con reason)
    void telemetryReceived(TelemetrySample s);           // passa campioni già decodificati
    void telemetryBatch(QVector<TelemetrySample> batch); // Segnale emesso da QTimer nel thread del worker che ogni _flushMs ms crea un batch di dati da mandare al thread GUI per il plot
private slots:
    void onReadyRead();

private:
    QSerialPort* _sp = nullptr;  // creato nel thread del worker, è puntatore perché lo crei/distruggi dinamicamente quando apri/chiudi
    QByteArray _rx;              // buffer di ricezione (stream UART)
    QElapsedTimer _t0;           // per dare un timestamp relativo PC ai campioni (utile per il plot)

    /*****************************************************************************/
    /* L'idea è che invece di mandare alla GUI ogni singolo pacchetto di TelemetrySample parsato da serialworker,
     i pacchetti di TelemtrySAmple parsati vengono accumulati in una coda _pending e poi mandati ogni X ms */
    QVector<TelemetrySample> _pending; // contenitore in cui accumulo la coda di campioni “non ancora inviati” al thread GUI per il plot

    QTimer* _flushTimer = nullptr;     // scandisce l’invio batch dei campioni. Questo QTimer lo creiamo e lo startiamo dentro uno slot che viene eseguito nel thread del worker
    int _flushMs = 33; // ~30 Hz (puoi fare 20–50ms) garantisce massimo 30 batch/s.
    /* contro: se in 33 ms arrivano tantissimi campioni, _pending diventa enorme →
     *  - batch enorme (copia/swap grande)
     *  - più latenza “percepita” (la GUI vede update a scatti)
       SOLUZIONE: FLUSH IBRIDO -> timer garantisce refresh costante e fluido
                                  soglia impedisce che la coda diventi troppo grande e che aumenti la latenza
    */
    void flushNow();
    int _flushMaxSamples = 300;      // SOGLIA: flush immediato se superi N sample. Miglioramento possibile: renderlo "adattivo” in base a baud/txInterval
    /*****************************************************************************/

    void parseBuffer();
    void resetPort();            // chiude la porta se aperta

    // Facciamo il log dei dati di telemetria per analisi offline
    struct RecSink {
        bool enabled = false;   // stato REC on/off
        QFile file;
        QTextStream ts;         // scrive testo sul file
        qint64 lines = 0;       // contatore righe per flush periodico
    };

    std::array<RecSink, 4> _rec;  // contenitore di un RecSink per ogni TelemetryType
    QString _logDir;              // BASE directory scelta dalla GUI (ComboBox)
    QString _sessionDir;          // directory effettiva di sessione (BASE + timestamp)

    void ensureRecDir();          // crea _sessionDir se vuota
    void maybeLogFrame(double tSec, TelemetryType type, quint8 id, qint16 raw, double value);
    QString typeName(TelemetryType t) const;
    bool anyRecordingEnabled() const;
    void stopAllRecording();      // utility
};

#endif // SERIALWORKER_H
