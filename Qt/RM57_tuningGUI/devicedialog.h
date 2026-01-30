/* È la “dashboard” dei dati: 4 tipologie (duty/currents/voltages/positions) e per ognuna 2 grafici (id 0 e id 1)
Filosofia adottata: follow live quando l'utente non sta esplorando, freeze quando esplora.  */

#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>  // è una finestra secondaria
#include <QHash>    // mappa un plot piccolo → popup zoom aperto (se esiste)
#include "qcustomplot.h"
#include "protocol.h"   // serve per TelemetrySample e TelemetryType
#include <QTimer>
#include <QElapsedTimer>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <array>
#include <QComboBox>

namespace Ui { class DeviceDialog; }

class PlotZoomDialog;   // forward declaration: ti basta sapere che esiste il tipo. In .cpp poi includi plotzoomdialog.h

class DeviceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeviceDialog(QWidget* parent=nullptr);
    ~DeviceDialog();

    void appendTelemetryBatch(const QVector<TelemetrySample>& batch); // aggiunge punti e aggiorna lastT

signals:
    /* Ricorda: questi slot sono frutto di connessioni fra thread (ovvero Qt:QueuedConnection) quindi se usiamo dei tipi
     definiti da noi (ad. es enum) bisogna fare la registrazione nel meta-type system (cosa che abbiamo fatto in protocol.h e main.cpp)*/
    void recordToggled(TelemetryType type, bool enabled);       // segnale che devicedialog manda a serialworker per avviare il log dello streaming
    void logDirChanged(const QString dir);  // passiamo la stringa della directory (il const promette di non modificare il parametro in ingresso alla funzione)

private:
    Ui::DeviceDialog* ui = nullptr;     // puntatore alla UI generata da Qt Designer

    double _windowSec = 10.0;       // finestra temporale visibile in X: gli ultimi 10 secondi

    void setupPlot(QCustomPlot* p, const QString& yLabel);  // inizializza ogni plot (2 graph, assi, legenda…)
    QCustomPlot* plotForType(TelemetryType t) const;    // fa mapping type → plot UI (plotCurrents, plotVoltages, ecc.)

    /* tiene traccia dei popup aperti per ogni plot piccolo:
     * - chiave: puntatore al plot piccolo
     * - valore: puntatore al popup
     * Quindi se fai doppio click di nuovo, invece di crearne un altro, richiami quello già aperto
    */
    QHash<QCustomPlot*, PlotZoomDialog*> _zooms;

    struct PlotState {              // struttura che serve per le interazioni da parte dell'utente con i plot
        bool userOverride = false;  // flag che ci dice se l'utente sta provando a fare azioni su un plot
        QElapsedTimer lastUser;     // calculate how much time has elapsed between two events
    };

    QHash<QCustomPlot*, PlotState> _plotState;

    //////////
    // --- streaming rate estimation ---
    QElapsedTimer _rateWall;
    qint64 _rateLastMs = 0;

    quint64 _framesCount = 0;                 // quanti TelemetrySample hai ricevuto (tipicamente 8 per tick)
    std::array<quint64, 8> _curveCount{};     // 4 type * 2 id = 8 curve

    double _framesHz = 0.0;
    double _ticksHz  = 0.0;
    quint64 _tickCount = 0;
    qint64 _lastTickKeyMs = -1;

    std::array<double, 8> _curveHz{};         // stima degli Hz per curva

    void updateRatesIfDue();                  // calcola ogni periodo di ~1s
    double curveHzForPlot(TelemetryType t) const; // max(id0,id1) per quel tipo
    //////////

    /* timer GUI (es. 16 ms, Precise) fa:
     * - scorrere l’asse X con un tempo continuo (non a gradini)
     * - replot a frequenza costante (60 Hz)*/
    double _lastT = 0.0;         // ultimo tSec ricevuto (telemetria)
    QElapsedTimer _wall;         // clock GUI
    qint64 _lastWallMs = 0;      // wall-time del momento in cui è arrivato l’ultimo batch/sample

    double tDisplayNow() const;  // _lastT + delta_wall

    // ===== Due timer: FAST vs SLOW =====
    QTimer _timerFast;           // 16 ms: scroll + replot: anche se i batch arrivano “a scatti” (ogni 33ms), tu vuoi che l’asse X scorra fluido (≈ 60 fps)
    QTimer _timerSlow;           // 100-200 ms: trim/limit/autoscale

    // ===== Pulsanti per interattività plot =====
    void installLiveButtons();              // crea e inserisce i pulsanti
    void forceLive(QCustomPlot* p);         // rimette un plot in live
    void forceLiveAll();                    // rimette tutti i plot in live
    double liveTimeNow() const;             // tempo da usare per riallineare gli assi

    // ===== Menu per slezione directory file log ======
    QComboBox* _cmbLogDir = nullptr;
    QString _logDir;            // percorso della directory di log
    void installTopLogDirBar(); // è la funzione che costruisce la UI top bar per selezionare la directory di log
};

#endif // DEVICEDIALOG_H
