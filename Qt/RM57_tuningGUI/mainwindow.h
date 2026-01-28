/* connessione UART + debounce tuning + apertura DeviceDialog

Perché serve:

- Legge UI (COM/baud, spinbox min/max/freq).
- Non parla “direttamente” con QSerialPort: manda richieste al worker via segnali.
- Implementa debounce: invia i parametri solo quando l’utente “si ferma” ????????
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>

#include "protocol.h"
#include "serialworker.h"
#include "devicedialog.h"

#ifdef MCU_SIM
#include "mcusimworker.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    // GUI -> Worker (nel thread I/O)
    void requestOpen(QString port, int baud);
    void requestClose();
    void requestWrite(QByteArray data);

private slots:
    // auto-connessioni dal Designer (clicked)
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnApply_clicked();

    // auto-connessione dal Designer (checkStateChanged)
    void on_chkLive_toggled(bool on);

    // Worker -> GUI
    void onWorkerConnected(bool ok, QString reason);
    void onTelemetryBatch(const QVector<TelemetrySample> batch);

    // Debounce
    void scheduleSendTuning();   // chiamata dai valueChanged
    void sendAllTuning();        // scatta su timeout del timer (LIVE) o su Apply (MANUALE)

private:
    Ui::MainWindow *ui = nullptr;

    #ifdef ADD_FUNCTIONS_TO_GUI
        SignalConfig  _sig[4]; // 0 Duty, 1 Current, 2 Position, 3 Voltage

        // Quando cambi forma, riempi i campi della pagina visibile con i valori memorizzati nella struct
        // Risultato: cambi forma e ritrovi gli ultimi valori usati per quella forma
        void applyDutyUiFromConfig();
        void applyCurUiFromConfig();
        void applyPosUiFromConfig();
        //void applyVoltUiFromConfig();

    #endif

    // Thread + Worker
    QThread* _ioThread = nullptr;
    SerialWorker* _worker = nullptr;

    // Dashboard plot
    DeviceDialog* _dlg = nullptr;

    // Debounce timer (solo in LIVE)
    QTimer _debounce;
    static constexpr int kDebounceMs = 150;

    // Stato “dirty” (solo in MANUALE)
    bool _tuningDirty = false;

    // helpers
    void loadPorts();
    void setConnectedUi(bool connected, const QString& text = QString());
    bool isConnected() const;

    void markDirty(bool dirty);

    #ifdef MCU_SIM
        QThread* _simThread = nullptr;
        McuSimWorker* _sim = nullptr;
    #endif

};
#endif // MAINWINDOW_H
