/*
 * Author: Michele Di Lucchio
 *  17/03/25
    Il modulo drawingarea è un QWidget che si occupa di:
    - rilevare la posizione della penna e disegnare nell'area di disegno
    - rilevare i bottoni della penna
    - rilevare gli ExpressKeys
    - inviare la posizione aggiornata del pennino alla GUI e al server
*/

#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>                                     // E' la base di ogni entità con funzioni di elemento grafico interattivo ovvero un QWidget
#include <QTabletEvent>                                // Receives TabletPress, TabletRelease and TabletMove events
#include <QPainter>                                    // Qt permette di disegnare oggetti grafici 2D (linee, poligoni, testi) con QPainter
#include <QResizeEvent>                                // Evento di ridimensionamento finestra
#include <QList>                                       // Lista per tracciare i punti del tratto
#include <QKeyEvent>                                   // Per intercettare gli eventi della tastiera (Qt interpreta gli ExpressKeys come tasti della tastiera)
#include <QTimer>                                      // Gestione dello Scaling Factor

// FILTRAGGIO DEI PUNTI ONLINE
#include <QQueue>      // per la finestra mobile


enum class DrawingState {                              // Visbilità pubblica e accessibile ovuqnue
    FreeHand,
    Recording,
    Repeating,
    // nuovo stato da implementare
    Scanning
};

class DrawingArea : public QWidget                     // La DrawingArea È un widget che si comporta come superficie di disegno
{
    Q_OBJECT

    public:
    DrawingArea(QWidget *parent = nullptr);

    enum class SendMode {                              // Modalità inivio dati al client TCP
        Live,                                          // Invia i dati punto a punto
        Batch                                          // Invia una lista di punti
    };
    void setSendMode(SendMode mode);

    void startCalibration();                            // Avvia la calibrazione della tavoletta

    protected:
    // I metodi protected possono essere chiamati solo da Qt (tecnica Gestione Eventi) o dalla stessa classe e dalle sue sottoclassi
    void tabletEvent(QTabletEvent *event) override;     // Metodo chiamata automaticamente da Qt per ricevere gli eventi della tavoletta grafica: TabletPress, TabletMove, TabletRelease. Questa funzione gestisce anche i pulsanti del pennino
    void paintEvent(QPaintEvent *event) override;       // Metodo invocato automaticamente da Qt quando il widget deve essere ridisegnato (es. dopo update())
    void resizeEvent(QResizeEvent *event) override;     // Metodo per gestire il ridimensionamento dell'area di disegno quando la finestra cambia dimensione

    // Gestione dello Scaling Factor
    void keyPressEvent(QKeyEvent *event) override;      // Metodo per intercettare se un bottone ExpressKey viene premuto
    void keyReleaseEvent(QKeyEvent *event) override;    // serve per fermare lo zoom

    signals:
    void stateChanged(DrawingState newState);              // Seganla a Mainwindow che deve essere aggiornato l'indicatore verde
    void penPositionUpdated(QPointF position);             // DrawingArea emette la posizione del pennino alla MainWindow

    void sendFullTrajectory(const QList<QPointF> &trajectory, int trajDuration); // segnale che dice: “ho una traiettoria pronta da inviare

    // Gestione dello Scaling Factor
    void scaleFactorChanged(float newScaleFactor);      // Segnale per notificare il cambio di scala alla MainWindow

    // Calibrazione della tavoletta
    void calibrationDebugMessage(const QString &msg);
    void calibrationDone(float pixelsPerMm);            // segnale quando la calibrazione è completa
    void strokeLengthUpdated(float mm);                 // per inviare aggiornamento alla UI

    private:
    bool debug = true;

    // Calibrazione della tavoletta (per ora OPZIONALE)
    bool calibrating = false;
    float pixelsPerMm = -1.0f; // valore di default se non calibrato
    int calibrationStep = 0;
    QPointF calibrationStart, calibrationEnd;
    float currentStrokeLength = 0.0f;         // Lunghezza accumulata in mm
    QPointF lastStrokePoint_mm;              // Ultimo punto utile per calcolo
    void logCalibration(const QString &msg);

    // Gestione degli stati del pennino
    DrawingState currentState;                          // verrà inizializzato nel costruttore
    void setState(DrawingState newState);               // cambia stato solo se è effettivamente diverso

    // SOLUZIONE PRPOPOSTA DA CHATGPT: Separazione della responsabilità
    // gestione del movimento e disegno → QTabletEvent
    // gestione dei pulsanti della penna → QMouseEvent
    // Questo è essenziale perché QTabletEvent ha informazioni di pressione, posizione precisa, orientamento, ma non sempre riporta correttamente tutti i pulsanti. QMouseEvent, invece, è più limitato ma riporta correttamente tutti i tasti del mouse/penna.
    //void mousePressEvent(QMouseEvent *event) override;
    //void mouseReleaseEvent(QMouseEvent *event) override;
    // Stato dei pulsanti in variabili globali
    //bool middleButtonPressed = false;
    //bool rightButtonPressed = false;
    bool recordingKeyPressed = false;
    bool RecordToRepeat = false;
    bool RepeatToFree = false;

    // Gestione delle posizioni del pennino
    QPixmap canvas;                                     // Memorizza il disegno per evitare di perderlo quando la finestra viene ridimensionata
    QList<QPointF> strokePoints;                        // Lista che memorizza più punti, permettendo di disegnare una linea fluida
    QPointF centeredPenPosition = QPointF(0, 0);        // Variabile per salvare la posizione del pennino rispetto al SDR centrato con la drawingarea

    // Gestione dello Scaling Factor
    float scaleFactor = 1.0f;                           // Fattore di scala iniziale per le coordinate del pennino
    QTimer *scaleTimer = nullptr;                       // timer per lo zoom continuo
    int scaleDirection = 0;                             // +1 = zoom in (A), -1 = zoom out (B)

    // Gestione del cursore a schermo
    void updateCursor(bool drawing);                    // Metodo per cambiare dinamicamente il cursore

    // FILTRAGGIO DEI PUNTI: FILTRO MEDIANO CIRCOLARE
    QQueue<QPointF> medianBuffer;
    int medianWindowSize { 11 };  // ad esempio finestra di 5 campioni
    QPointF computeMedianFilteredPoint() const; // funzione che non prende parametri, non modifica l’oggetto in cui si trova e restituisce un punto (x, y) calcolato come media mediana da un insieme di punti
    QPointF computeRadialMedian() const;

    // INTERPOLAZIONE LINEARE & RICAMPIONAMENTO
    QList<QPointF> resampledTrajectory;
    QList<QPointF> resampleTrajectory(const QList<QPointF>& trajectory, float spacing); // ricampiona i punti della traiettoria in base alla distanza spacing
    QPointF interpolate(const QPointF& start, const QPointF& end, float t); // interpolazione lineare tra due punti 2D

    // INTERPOLAZIONE CATMULL-ROM
    //QList<QPointF> catmullRomSpline(const QList<QPointF>& trajectory, float distance);
    //QPointF catmullRom(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3, float t);
    //QList<double> CatmullsegmentLength;

    // REGISRTAZIONE DELLE TRAIETTORIA
    QList<QPointF> recordedTrajectory;                  // Lista dei punti registrati da riprodurre
    QTimer *playbackTimer = nullptr;                    // Timer per animare i punti
    float interval;
    int playbackIndex = 0;                              // Indice corrente
    bool playbackForward = true;                        // Direzione della riproduzione: true se stai andando avanti, false se stai tornando indietro
    SendMode currentMode = SendMode::Live;

    // ANALISI MATLAB DELLA TRAIETTORIA REGISTRATA
    void saveTrajectoryToTextFile(const QList<QPointF>& points);
    QString fileName;
    bool flag; // serve per salvare i file CSV

    // SCANNING DELLA TRAIETTORIA
    QList<QPointF> scannedTrajectory;
    QList<QPointF> spiralTrajectory;
    bool ScanStarted = false;

    void saveScanTrajectoryToTextFile(QList<QPointF>& points) const;

};

#endif // DRAWINGAREA_H
