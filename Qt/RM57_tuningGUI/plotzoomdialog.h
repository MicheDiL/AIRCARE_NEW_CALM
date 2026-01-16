/* finestra popup su doppio click

- il popup crea un suo QCustomPlot “big”
- clona i graph e condivide i data container via setData(src->graph(i)->data()) (QSharedPointer)
→ qualsiasi trimming/limit fatto nel plot piccolo agisce sullo stesso container
→ il popup “eredita” automaticamente quei tagli
 Quindi: popup e plot piccolo vedono gli stessi punti in tempo reale

Filosofia adottata per l'interazione con le finestre di popup: follow live quando l'utente non sta esplorando, freeze quando esplora.
*/

#ifndef PLOTZOOMDIALOG_H
#define PLOTZOOMDIALOG_H

#include <QDialog>          // finestra secondaria (popup). È perfetta per un “plot ingrandito”
#include "qcustomplot.h"    // serve perché la classe usa QCustomPlot*
#include <QPointer>
#include <QElapsedTimer>
#include <QMetaObject>
#include <QTimer>

class QToolButton;
class QPushButton;


class PlotZoomDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotZoomDialog(QWidget* parent=nullptr);  // se non passi parent, il dialog è top-level. Nel tuo uso lo crei con parent DeviceDialog, quindi verrà distrutto automaticamente con esso

    /* Questa è l’API principale del popup:
        - gli passi il QCustomPlot “piccolo”
        - il popup si configura per mostrare lo stesso contenuto (idealmente in modo live)*/
    void attachTo(QCustomPlot* source);
    void setGraphVisible(int idx, bool vis);

private:
    // ------ UI -------
    QCustomPlot* _plot = nullptr;   // è il plot interno al popup (grande)

    //
    QToolButton* _btnMeas = nullptr;
    QToolButton* _btnTarg = nullptr;
    QPushButton* _btnLive = nullptr;
    //

    // ---- source plumbing ----
    QPointer<QCustomPlot> _src;                 // plot piccolo sorgente per il plot grande (weak, safe se viene distrutto)
    QMetaObject::Connection _srcConn;           // serve per disconnettere la vecchia connessione quando ri-attacchi il popup ad un altro plot piccolo

    // ---- “explore vs live” ----
    bool _userOverride = false;                 // true = l’utente sta esplorando, non toccare i range
    void setUserOverride(bool on);
    void forceLive();                           // LIVE button
    void updateLiveButtonUi();                  // enable/disable, tooltip ecc.

    void markUserInteraction();                 // come in DeviceDialog: attiva override e timer di “return to live”
    void syncRangesFromSource();                // copia i range X/Y dal plot piccolo -> popup

    // ---- tracer/readout ----
    // “puntatori” che seguono le curve
    QCPItemTracer* _tr = nullptr;
    QCPGraph* _activeGraph = nullptr;   // curva selezionata
    QCPItemText* _readout = nullptr;    // testo che mostra i valori sotto il mouse

    void setTracerVisible(bool on);
    void setupPointerReadout();
    void updatePointerAt(const QPoint& mousePos);   // aggiorna SOLO la curva selezionata

    static bool yAtXInterpolated(QCPGraph* g, double x, double& yOut); // Leggere il valore delle curve nel punto selezionato

    // util
    void buildTopBar();

};

#endif // PLOTZOOMDIALOG_H
