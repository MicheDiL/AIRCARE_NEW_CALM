#include "drawingarea.h"
#include <QPainter>

// ANALISI MATLAB DELLA TRAIETTORIA REGISTRATA
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QPointF>
#include <QDateTime>

// SCANSIONE DELLA TRAIETTORIA
#include <clipper2/clipper.h>
using namespace Clipper2Lib; // spazio dei nomi in cui sono dichiarati tutte le classi, funzioni e tipi della libreria clipper2

#include <QPointF>
#include <QVector2D>
#include <QList>
#include <QLineF>
#include <QtGlobal>   // qFuzzyIsNull
#include <cmath>

//////////////////////////////////// FUNZIONI ESCLUSIVE DEL FILE .CPP ////////////////////////////////////

// 1)
static QPointF computeCentroid(const QList<QPointF>& poly) { // funzione privata al cpp
    // Calcola il baricentro del poligono, cioè la media delle x e delle y
    QPointF c(0,0);
    for (auto &p : poly) c += p; // Somma vettorialmente tutte le coordinate in c
    return c / poly.size(); // Divide il risultato per il numero di punti, ottenendo la media delle x e la media delle y
}

// 2) Sposta ogni punto di “contour” verso il suo centroide di una distanza d

/*static QList<QPointF> insetTowardsCentroid(const QList<QPointF>& contour, float d, const QPointF& C) { // funzione privata al cpp

    QList<QPointF> inner;
    inner.reserve(contour.size());

    //QPointF C = computeCentroid(contour);
    for (auto &p : contour) {
        QVector2D dir(C - p); // Calcola il vettore dir = C – p che punta dal punto p al centroide C
        if (dir.length() < 1e-3f) {
            inner.append(p);  // già troppo vicino al centro
        } else {
            dir.normalize();
            inner.append(p + dir.toPointF() * d); //  normalizza dir e lo scala di d, quindi somma questo offset a p
        }
    }

    return inner;
}*/
// 2) Scale a contour towards center by factor scale
static QList<QPointF> scaleContour(const QList<QPointF>& contour, const QPointF& C, float scale) {

    QList<QPointF> inner;
    inner.reserve(contour.size());

    for (auto &p : contour) {
        QVector2D v(p - C); // v sarebbe un QPointF ma viene convertito in QVector2D per poter fare operazioni vettoriali
        inner.append(C + (v * scale).toPointF()); // .toPointF() converte il QVector2D scalato di nuovo in QPointF
    }
    return inner;

}
// 3) Nuova funzione: calcola quanti anelli ci stanno prima che ogni punto arrivi al centroide

/*static int computeMaxRings(const QList<QPointF>& contour, float insetStep) {
    // 3.1 centroide del contorno
    QPointF C = computeCentroid(contour);

    // 3.2 distanza minima dal bordo al centro
    float minDist = std::numeric_limits<float>::max();
    for (auto &p : contour) {
        float dx = p.x() - C.x();
        float dy = p.y() - C.y();
        float dist = std::hypot(dx, dy);
        minDist = std::min(minDist, dist);
    }

    // 3.3 quante volte posso scalare di insetStep prima di arrivare al centro?
    return static_cast<int>(std::floor(minDist / insetStep));
}*/

// 4) Sample a closed contour at fixed spacing -> same spacing, fewer points as perimeter shrinks
static QList<QPointF> sampleClosedContour(const QList<QPointF>& contour, float spacing) {

    int n = contour.size();
    if (n < 2)
        return {};

    // compute total perimeter including closing segment
    float perim = 0;
    for (int i = 1; i < n; ++i)
        perim += QLineF(contour[i-1], contour[i]).length(); // somma la lunghezza di ogni segmento tra punti consecutivi
    perim += QLineF(contour.last(), contour.first()).length(); //  aggiunge la lunghezza del segmento che chiude il contorno

    int N = static_cast<int>(std::floor(perim / spacing)); // quanti punti possono stare lungo il contorno con una distanza di spacing tra loro
    QList<QPointF> result; // conterrà i punti campionati
    result.reserve(N);
    float accumulated = 0; // lunghezza accumulata man mano che si percorrono i segmenti
    int idx = 0; // indice del segmento corrente

    // traverse and interpolate
    for (int i = 0; i < N; ++i) {
        float target = i * spacing; // distanza a cui dovrebbe trovarsi il punto lungo il contorno
        // advance to segment containing target
        while (true) { // avanza di segmento in segmento
            QPointF p0 = contour[idx];
            QPointF p1 = contour[(idx+1) % n];
            float segLen = QLineF(p0, p1).length();
            if (accumulated + segLen >= target)
            {
                // il punto di distanza target cade su questo segmento
                float t = (target - accumulated) / segLen;
                // linear interpolation
                QPointF pt = QPointF(p0.x() + (p1.x()-p0.x())*t,
                                     p0.y() + (p1.y()-p0.y())*t);
                result.append(pt);
                break;
            }
            // altrimenti salto questo intero segmento
            accumulated += segLen;
            idx = (idx + 1) % n;
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent)
{

    setFocusPolicy  (Qt::StrongFocus);                      // Permette a DrawingArea di ricevere eventi della tastiera (ExpressKeys)
    setAttribute    (Qt::WA_TabletTracking);                // Abilita il rilevamento continuo del pennino anche se non c'è presisone sulla tavoletta
    updateCursor(false);                                    // All'avvio, cursore trasparente

    // Gestione dello Scaling Factor
    scaleTimer = new QTimer(this);         // allochiamo un timer Qt e lo associamo al widget
    scaleTimer->setInterval(100);          // tempo tra un aggiornamento e l’altro (100ms = 10 volte al secondo)

    connect(scaleTimer, &QTimer::timeout, this, [this]() {
        if (scaleDirection == 1) {
            scaleFactor *= 1.02f;          // aumenta gradualmente del 2%
        } else if (scaleDirection == -1) {
            scaleFactor *= 0.98f;          // diminuisce gradualmente del 2%
        }
        emit scaleFactorChanged(scaleFactor);   // comunica il nuovo valore a MainWindow
        update();                               // forza il ridisegno
    });

    //currentState = DrawingState::FreeHand;

    // Inzializzo il Timer per la ripetizione della traiettoria
    playbackTimer = new QTimer(this);
    //playbackTimer->setInterval(30); // Ogni 30 ms viene mostrato un punto della traiettoria, quindi la durata totale del playback è proporzionale al numero di punti

    /*connect(playbackTimer, &QTimer::timeout, this, [this]() {
        if (currentState != DrawingState::Repeating) {
            playbackTimer->stop(); // ferma il timer se non più in stato Repeating
            return;
        }

        if (resampledTrajectory.isEmpty())
            return; // nessun punto da riprodurre

        // Riproduzione "ping-pong"
        if (playbackForward) {
            if (playbackIndex < resampledTrajectory.size()) {
                QPointF p = resampledTrajectory[playbackIndex];
                strokePoints.append(p);
                centeredPenPosition = p;
                emit penPositionUpdated(p);
                ++playbackIndex;
            } else {
                playbackForward = false;  // hai raggiunto la fine → cambia direzione
                --playbackIndex;
            }
        } else {
            if (playbackIndex >= 0) {
                QPointF p = resampledTrajectory[playbackIndex];
                strokePoints.append(p);
                centeredPenPosition = p;
                emit penPositionUpdated(p);
                --playbackIndex;
            } else {
                playbackForward = true; // tornato all’inizio → inverti di nuovo
                ++playbackIndex;
            }
        }

        update(); // forza ridisegno
    });*/

    // Soluzione funzionante

    /*connect(playbackTimer, &QTimer::timeout, this, [this]() {
        if (currentState != DrawingState::Repeating) {
            playbackTimer->stop(); // ferma il timer se non più in stato Repeating
            return;
        }
        else if(currentState == DrawingState::Repeating){
            if (resampledTrajectory.isEmpty())
                return; // nessun punto da riprodurre

            // Riproduzione "ping-pong"
            if (playbackForward) {
                if (playbackIndex < resampledTrajectory.size()) {
                    QPointF p = resampledTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    ++playbackIndex;
                } else {
                    playbackForward = false;  // hai raggiunto la fine → cambia direzione
                    --playbackIndex;
                }
            } else {
                if (playbackIndex >= 0) {
                    QPointF p = resampledTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    --playbackIndex;
                } else {
                    playbackForward = true; // tornato all’inizio → inverti di nuovo
                    ++playbackIndex;
                }
            }

            update(); // forza ridisegno
        }
    });

    flag = false;  // o true, a seconda del primo file che vuoi scrivere
}*/
    connect(playbackTimer, &QTimer::timeout, this, [this]() {
        if (currentState != DrawingState::Repeating && currentState != DrawingState::Scanning) {
            playbackTimer->stop(); // ferma il timer se non più in stato Repeating
            return;
        }
        else if(currentState == DrawingState::Repeating){
            if (resampledTrajectory.isEmpty())
                return; // nessun punto da riprodurre

            // Riproduzione "ping-pong"
            if (playbackForward) {
                if (playbackIndex < resampledTrajectory.size()) {
                    QPointF p = resampledTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    ++playbackIndex;
                } else {
                    playbackForward = false;  // hai raggiunto la fine → cambia direzione
                    --playbackIndex;
                }
            } else {
                if (playbackIndex >= 0) {
                    QPointF p = resampledTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    --playbackIndex;
                } else {
                    playbackForward = true; // tornato all’inizio → inverti di nuovo
                    ++playbackIndex;
                }
            }

            update(); // forza ridisegno

        } else if(currentState == DrawingState::Scanning){
            if (spiralTrajectory.isEmpty())
                return; // nessun punto da riprodurre

            // Riproduzione "ping-pong"
            if (playbackForward) {
                if (playbackIndex < spiralTrajectory.size()) {
                    QPointF p = spiralTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    ++playbackIndex;
                } else {
                    playbackForward = false;  // hai raggiunto la fine → cambia direzione
                    --playbackIndex;
                }
            } else {
                if (playbackIndex >= 0) {
                    QPointF p = spiralTrajectory[playbackIndex];
                    strokePoints.append(p);
                    centeredPenPosition = p;
                    emit penPositionUpdated(p);
                    --playbackIndex;
                } else {
                    playbackForward = true; // tornato all’inizio → inverti di nuovo
                    ++playbackIndex;
                }
            }
            update(); // forza ridisegno
        }
    });

    flag = false;  // o true, a seconda del primo file che vuoi scrivere
}


void DrawingArea::setSendMode(SendMode mode) {
    currentMode = mode;
}

//////////////////////////////////////////////////// FUNZIONI TABLETEVENT ////////////////////////////////////////////////////
// SOLUZIONE FUNZIONANTE

/*void DrawingArea::tabletEvent(QTabletEvent *event)
{
    if (calibrating && event->type() == QEvent::TabletPress) {
        QPointF raw = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centered = (raw - center) * scaleFactor;

        if (calibrationStep == 0) {
            calibrationStart = centered;
            calibrationStep = 1;
            //qDebug() << "[CALIBRAZIONE] Punto A registrato. Tocca punto B (a 100 mm di distanza).";
            logCalibration("[CALIBRAZIONE]:Tocca un punto B adiacente");
            return;
        } else if (calibrationStep == 1) {
            calibrationEnd = centered;
            calibrating = false;
            calibrationStep = 0;

            float distancePx = QLineF(calibrationStart, calibrationEnd).length();
            float pixelsPerMm = distancePx / 11.0f; // 11 mm come riferimento

            qDebug() << "[CALIBRAZIONE]: distanza [pixel]:" << distancePx << " -> pixelsPerMm =" << pixelsPerMm;

            emit calibrationDone(pixelsPerMm);
            this->pixelsPerMm = pixelsPerMm;
            return;
        }
    }


    if (!calibrating && currentState != DrawingState::Repeating && event->pressure() > 0.0f &&
        (event->type() == QEvent::TabletPress || event->type() == QEvent::TabletMove)) {

        updateCursor(true);

        QPointF rawPosition = event->position();
        QPointF center(width() / 2, height() / 2);
        //QPointF centeredPosition = (rawPosition - center) * scaleFactor;
        QPointF centeredPosition;

        if (pixelsPerMm > 0.0f) {
            centeredPosition = ((rawPosition - center) / pixelsPerMm) * scaleFactor;
        } else {
            centeredPosition = (rawPosition - center) * scaleFactor;
        }

        // 1) Quando arriva un nuovo punto lo enqueue nel buffer
        medianBuffer.enqueue(centeredPosition);
        if (medianBuffer.size() > medianWindowSize)
            // se il buffer supera N elementi fai dequeue del più vecchio
            medianBuffer.dequeue();

        // 2) calcola il punto mediano filtrato
        //QPointF filteredPos = computeMedianFilteredPoint();
        // Optando per il filtro mediano radiale bisogna considerare che bisogna applicare il filtro radiale solo dopo aver accumulato almeno medianWindowSize campioni reali altrimenti alcuni dei “filteredPoint” iniziali possono comparire con segni opposti
        QPointF filteredPos;
        if(medianBuffer.size() > medianWindowSize){
            filteredPos = computeRadialMedian();
        } else {
            filteredPos = centeredPosition;
        }

        if (event->buttons() & Qt::RightButton) {
            setState(DrawingState::Recording);

            //strokePoints.append(centeredPosition);
            //recordedTrajectory.append(centeredPosition);
            strokePoints.append(filteredPos);
            recordedTrajectory.append(filteredPos);

        } else if(currentState != DrawingState::Repeating) {
            setState(DrawingState::FreeHand); //  Attenzione: questo else si attiva anche se ad esempio premi il MiddleButton

            //strokePoints.append(centeredPosition);
            strokePoints.append(filteredPos);
        }

        //centeredPenPosition = centeredPosition;
        //emit penPositionUpdated(centeredPosition);
        centeredPenPosition = filteredPos;
        emit penPositionUpdated(filteredPos);

        if (pixelsPerMm > 0.0f) { // Eseguo solo se è stata fatta la calibrazione
            QPointF currentPoint_mm = centeredPosition ;/// pixelsPerMm; // converte in mm
            if (event->type() == QEvent::TabletPress) {
                currentStrokeLength = 0.0f;
                lastStrokePoint_mm = currentPoint_mm;
            } else {
                float segment = QLineF(lastStrokePoint_mm, currentPoint_mm).length();
                currentStrokeLength += segment;
                lastStrokePoint_mm = currentPoint_mm;
            }

            emit strokeLengthUpdated(currentStrokeLength); // notifica alla UI
        }

        if(debug){
            //qDebug() << "Pennino -> Posizione centrata: X=" << centeredPenPosition.x()
            //<< ", Y=" << centeredPenPosition.y(); // debug nella console
            qDebug() << "Pennino -> Posizione centrata: X=" << filteredPos.x()
            << ", Y=" << filteredPos.y(); // debug nella console
        }
        update();
    }
    // Sollevo il pennino dopo un contatto precedente
    else if(!calibrating && event->type() == QEvent::TabletRelease){

        updateCursor(false);
        //strokePoints.clear();      // cancella il tratto corrente
        // Reset della lunghezza tratto
        currentStrokeLength = 0.0f;
        emit strokeLengthUpdated(0.0f);
        update();

        // Se sono in Repeating, ignora completamente il rilascio
        if (currentState == DrawingState::Repeating) {
            return;
        }

        //  Rileva rilascio del pulsante destro → entra in stato Repeating
        if (!(event->buttons() & Qt::RightButton)) {
            strokePoints.clear();      // cancella il tratto corrente
            resampledTrajectory.clear();
            medianBuffer.clear();
            if (currentState == DrawingState::Recording) {
                setState(DrawingState::Repeating);
            }
        } else {
            strokePoints.clear();              // cancella il tratto corrente
            medianBuffer.clear();
            setState(DrawingState::FreeHand);  // ritorna FreeHand
        }
    }
    // RightButton premuto in aria durante stato Repeating
    else if (!calibrating && event->type() == QEvent::TabletMove && event->pressure() == 0.0f) {
        if (currentState == DrawingState::Repeating && (event->buttons() & Qt::RightButton)) { // (event->buttons() & Qt::MiddleButton)
            setState(DrawingState::FreeHand);
            recordedTrajectory.clear();
            resampledTrajectory.clear();
            medianBuffer.clear();
        }
    }
}*/

// SOLUZIONE PROPOSTA DA CHATGPT (usa eventi del Mouse)

/*void DrawingArea::tabletEvent(QTabletEvent *event)
{
    if (calibrating && event->type() == QEvent::TabletPress) {
        QPointF raw = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centered = (raw - center) * scaleFactor;

        if (calibrationStep == 0) {
            calibrationStart = centered;
            calibrationStep = 1;
            logCalibration("[CALIBRAZIONE]:Tocca un punto B adiacente");
            return;
        } else if (calibrationStep == 1) {
            calibrationEnd = centered;
            calibrating = false;
            calibrationStep = 0;

            float distancePx = QLineF(calibrationStart, calibrationEnd).length();
            float pixelsPerMm = distancePx / 11.0f;

            qDebug() << "[CALIBRAZIONE]: distanza [pixel]:" << distancePx << " -> pixelsPerMm =" << pixelsPerMm;

            emit calibrationDone(pixelsPerMm);
            this->pixelsPerMm = pixelsPerMm;
            return;
        }
    }

    if (!calibrating && currentState != DrawingState::Repeating &&
        event->pressure() > 0.0f &&
        (event->type() == QEvent::TabletPress || event->type() == QEvent::TabletMove)) {

        updateCursor(true);

        QPointF rawPosition = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centeredPosition = (pixelsPerMm > 0.0f ?
                                        ((rawPosition - center) / pixelsPerMm) * scaleFactor :
                                        (rawPosition - center) * scaleFactor);

        medianBuffer.enqueue(centeredPosition);
        if (medianBuffer.size() > medianWindowSize)
            medianBuffer.dequeue();

        QPointF filteredPos = (medianBuffer.size() > medianWindowSize ?
                                   computeRadialMedian() : centeredPosition);

        // ➤ MiddleButton = registrazione
        if (middleButtonPressed) {
            strokePoints.append(filteredPos);
            recordedTrajectory.append(filteredPos);
            setState(DrawingState::Recording);
        } else {
            strokePoints.append(filteredPos);
            setState(DrawingState::FreeHand);
        }

        centeredPenPosition = filteredPos;
        emit penPositionUpdated(filteredPos);

        if (pixelsPerMm > 0.0f) {
            QPointF currentPoint_mm = centeredPosition;
            if (event->type() == QEvent::TabletPress) {
                currentStrokeLength = 0.0f;
                lastStrokePoint_mm = currentPoint_mm;
            } else {
                float segment = QLineF(lastStrokePoint_mm, currentPoint_mm).length();
                currentStrokeLength += segment;
                lastStrokePoint_mm = currentPoint_mm;
            }

            emit strokeLengthUpdated(currentStrokeLength);
        }

        if (debug) {
            qDebug() << "Pennino -> Posizione centrata: X=" << filteredPos.x()
            << ", Y=" << filteredPos.y();
        }

        update();
    }

    // ➤ Fine tratto (penna sollevata)
    else if (!calibrating && event->type() == QEvent::TabletRelease) {
        updateCursor(false);
        currentStrokeLength = 0.0f;
        emit strokeLengthUpdated(0.0f);
        update();

        // Ignora rilascio se in modalità Repeating
        if (currentState == DrawingState::Repeating) {
            return;
        }

        // ➤ Se il tasto destro (inferiore) è stato rilasciato → termina Repeating
        if (!rightButtonPressed) {
            strokePoints.clear();
            resampledTrajectory.clear();
            medianBuffer.clear();
            if (currentState == DrawingState::Recording) {
                setState(DrawingState::Repeating);
            }
        } else {
            strokePoints.clear();
            medianBuffer.clear();
            setState(DrawingState::FreeHand);
        }
    }

    // ➤ Penna in aria durante Repeating con tasto destro premuto
    else if (!calibrating && event->type() == QEvent::TabletMove && event->pressure() == 0.0f) {
        if (currentState == DrawingState::Repeating && middleButtonPressed) {
            setState(DrawingState::FreeHand);
            recordedTrajectory.clear();
            resampledTrajectory.clear();
            medianBuffer.clear();
        }
    }
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
        middleButtonPressed = true;

    if (event->button() == Qt::RightButton)
        rightButtonPressed = true;

    QWidget::mousePressEvent(event);
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
        middleButtonPressed = false;

    if (event->button() == Qt::RightButton)
        rightButtonPressed = false;

    QWidget::mouseReleaseEvent(event);
}*/

// SOLUZIONE che usa ExpressKey per gestione del bottone superiore

/*void DrawingArea::tabletEvent(QTabletEvent *event)
{
    if (calibrating && event->type() == QEvent::TabletPress) {
        QPointF raw = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centered = (raw - center) * scaleFactor;

        if (calibrationStep == 0) {
            calibrationStart = centered;
            calibrationStep = 1;
            logCalibration("[CALIBRAZIONE]: Tocca un punto B adiacente");
            return;
        } else if (calibrationStep == 1) {
            calibrationEnd = centered;
            calibrating = false;
            calibrationStep = 0;

            float distancePx = QLineF(calibrationStart, calibrationEnd).length();
            float pixelsPerMm = distancePx / 11.0f;

            qDebug() << "[CALIBRAZIONE]: distanza [pixel]:" << distancePx << " -> pixelsPerMm =" << pixelsPerMm;

            emit calibrationDone(pixelsPerMm);
            this->pixelsPerMm = pixelsPerMm;
            return;
        }
    }

    if (!calibrating && currentState != DrawingState::Repeating && event->pressure() > 0.0f &&
        (event->type() == QEvent::TabletPress || event->type() == QEvent::TabletMove)) {

        updateCursor(true);

        QPointF rawPosition = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centeredPosition = (pixelsPerMm > 0.0f ?
                                        ((rawPosition - center) / pixelsPerMm) * scaleFactor :
                                        (rawPosition - center) * scaleFactor);

        medianBuffer.enqueue(centeredPosition);
        if (medianBuffer.size() > medianWindowSize)
            medianBuffer.dequeue();

        QPointF filteredPos = (medianBuffer.size() > medianWindowSize ?
                                   computeRadialMedian() : centeredPosition);


        strokePoints.append(filteredPos); // eseguito in modalità FREEHAND

        if (recordingKeyPressed) {
            setState(DrawingState::Recording);
            recordedTrajectory.append(filteredPos); // eseguito solo in modalità RECORDING
        } else if (!RecordToRepeat && !RepeatToFree){
            setState(DrawingState::FreeHand);
            RepeatToFree = false;
        }

        centeredPenPosition = filteredPos;
        emit penPositionUpdated(filteredPos);

        if (pixelsPerMm > 0.0f) {
            QPointF currentPoint_mm = centeredPosition;
            if (event->type() == QEvent::TabletPress) {
                currentStrokeLength = 0.0f;
                lastStrokePoint_mm = currentPoint_mm;
            } else {
                float segment = QLineF(lastStrokePoint_mm, currentPoint_mm).length();
                currentStrokeLength += segment;
                lastStrokePoint_mm = currentPoint_mm;
            }

            emit strokeLengthUpdated(currentStrokeLength);
        }

        if (debug) {
            qDebug() << "Pennino -> Posizione centrata: X=" << filteredPos.x()
            << ", Y=" << filteredPos.y();
        }

        update();
    }
    else if (!calibrating && event->type() == QEvent::TabletRelease) {

        updateCursor(false);
        currentStrokeLength = 0.0f;
        emit strokeLengthUpdated(0.0f);
        update();

        if(!recordingKeyPressed){

            if (currentState == DrawingState::Repeating) {
                return;
            }

            //  Rileva rilascio del pulsante destro → entra in stato Repeating
            if (currentState == DrawingState::Recording) {
                strokePoints.clear();      // cancella il tratto corrente
                resampledTrajectory.clear();
                medianBuffer.clear();
                setState(DrawingState::Repeating);
            } else {
                strokePoints.clear();              // cancella il tratto corrente
                medianBuffer.clear();
                setState(DrawingState::FreeHand);  // ritorna FreeHand
            }
        }
    }
    else if (!calibrating && event->type() == QEvent::TabletMove && event->pressure() == 0.0f) {
        if (currentState == DrawingState::Repeating && recordingKeyPressed) { // (event->buttons() & Qt::MiddleButton)
            setState(DrawingState::FreeHand);
            strokePoints.clear();
            recordedTrajectory.clear();
            resampledTrajectory.clear();
            medianBuffer.clear();
            recordingKeyPressed = false;
            RecordToRepeat = false;
            RepeatToFree = true;
        }
        update();
    }
}*/

// SOLUZIONE EXPRESSKEY + RIGHTBUTTON
void DrawingArea::tabletEvent(QTabletEvent *event)
{
    if (calibrating && event->type() == QEvent::TabletPress) {
        QPointF raw = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centered = (raw - center) * scaleFactor;

        if (calibrationStep == 0) {
            calibrationStart = centered;
            calibrationStep = 1;
            logCalibration("[CALIBRAZIONE]: Tocca un punto B adiacente");
            return;
        } else if (calibrationStep == 1) {
            calibrationEnd = centered;
            calibrating = false;
            calibrationStep = 0;

            float distancePx = QLineF(calibrationStart, calibrationEnd).length();
            float pixelsPerMm = distancePx / 11.0f;

            qDebug() << "[CALIBRAZIONE]: distanza [pixel]:" << distancePx << " -> pixelsPerMm =" << pixelsPerMm;

            emit calibrationDone(pixelsPerMm);
            this->pixelsPerMm = pixelsPerMm;
            return;
        }
    }

    if (!calibrating && currentState != DrawingState::Repeating && event->pressure() > 0.0f &&
        (event->type() == QEvent::TabletPress || event->type() == QEvent::TabletMove)) {

        updateCursor(true);

        QPointF rawPosition = event->position();
        QPointF center(width() / 2, height() / 2);
        QPointF centeredPosition = (pixelsPerMm > 0.0f ?
                                        ((rawPosition - center) / pixelsPerMm) * scaleFactor :
                                        (rawPosition - center) * scaleFactor);

        medianBuffer.enqueue(centeredPosition);
        if (medianBuffer.size() > medianWindowSize)
            medianBuffer.dequeue();

        QPointF filteredPos = (medianBuffer.size() > medianWindowSize ?
                                   computeRadialMedian() : centeredPosition);


        strokePoints.append(filteredPos); // eseguito in modalità FREEHAND

        if (recordingKeyPressed){

            // --- Recording
            setState(DrawingState::Recording);
            recordedTrajectory.append(filteredPos); // eseguito solo in modalità RECORDING

        } else if(event->buttons() & Qt::RightButton){

            // --- Scanning
            //setState(DrawingState::Scanning);
            ScanStarted = true;
            scannedTrajectory.append(filteredPos);

        } else if (!RecordToRepeat && !RepeatToFree){

            // --- FreeHand
            setState(DrawingState::FreeHand);
            RepeatToFree = false;
        }

        centeredPenPosition = filteredPos;
        emit penPositionUpdated(filteredPos);

        if (pixelsPerMm > 0.0f) {
            QPointF currentPoint_mm = centeredPosition;
            if (event->type() == QEvent::TabletPress) {
                currentStrokeLength = 0.0f;
                lastStrokePoint_mm = currentPoint_mm;
            } else {
                float segment = QLineF(lastStrokePoint_mm, currentPoint_mm).length();
                currentStrokeLength += segment;
                lastStrokePoint_mm = currentPoint_mm;
            }

            emit strokeLengthUpdated(currentStrokeLength);
        }

        if (debug) {
            qDebug() << "Pennino -> Posizione centrata: X=" << filteredPos.x()
            << ", Y=" << filteredPos.y();
        }

        update();
    }
    // ---------- Sollevo il pennino dopo il disegno ----------
    else if (!calibrating && event->type() == QEvent::TabletRelease) {

        updateCursor(false);
        currentStrokeLength = 0.0f;
        emit strokeLengthUpdated(0.0f);
        update();

        if(!recordingKeyPressed){

            if (currentState == DrawingState::Repeating || currentState == DrawingState::Scanning) {
                return;
            }

            if (currentState == DrawingState::Recording) {
                strokePoints.clear();      // cancella il tratto corrente
                resampledTrajectory.clear();
                medianBuffer.clear();
                setState(DrawingState::Repeating);
            }
            else if(ScanStarted && !(event->buttons() & Qt::RightButton)){
                // FA PARTIRE LA ROUTINE CHE FA LA SCANSIONE DELLA TRAIETTORIA
                /*........*/
                // Genera spirale parametrica che parte dall'ultimo punto
                setState(DrawingState::Scanning);

                strokePoints.clear();              // cancella il tratto corrente
                medianBuffer.clear();

            }
            else {
                strokePoints.clear();              // cancella il tratto corrente
                medianBuffer.clear();
                setState(DrawingState::FreeHand);  // ritorna FreeHand
            }
        }
    }
    else if (!calibrating && event->type() == QEvent::TabletMove && event->pressure() == 0.0f) {
        if (currentState == DrawingState::Repeating && recordingKeyPressed) { // (event->buttons() & Qt::MiddleButton)
            setState(DrawingState::FreeHand);
            strokePoints.clear();
            recordedTrajectory.clear();
            resampledTrajectory.clear();
            medianBuffer.clear();
            recordingKeyPressed = false;
            RecordToRepeat = false;
            RepeatToFree = true;
        } else if(currentState == DrawingState::Scanning && (event->buttons() & Qt::RightButton)){
            //strokePoints.clear();
            //medianBuffer.clear();
            scannedTrajectory.clear();
            spiralTrajectory.clear();
            ScanStarted = false;
            setState(DrawingState::FreeHand);
        }
        update();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawingArea::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // QPainter permette di disegnare su DrawingArea, con smussamento dei bordi.

    painter.translate(width()/2, height()/2); // sposto l'origine (0,0) del QPainter al centro della drawingArea. Ora il punto (0,0) corrisponde al centro t.c tutte le operazioni di disegno vengono automaticamente "traslate"
    // Da questo momento in poi il punto (0, 0) per il QPainter non corrisponde più all'angolo in alto a sinistra del widget, ma diventa il centro del widget

    // Disegna lo sfondo con il contenuto del canvas salvato
    // Disegna il canvas in modo che il suo centro corrisponda all'origine (0,0)
    // Assumendo che canvas abbia dimensioni pari a quelle del widget o comunque si voglia centrarlo:
    painter.drawPixmap(-canvas.width()/2, -canvas.height()/2, canvas);

    // === DEBUG: bordo attorno al canvas ===
    QPen borderPen(Qt::blue); // colore bordo
    borderPen.setWidth(20); // spessore bordo
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush); // nessun riempimento
    painter.drawRect(-canvas.width()/2, -canvas.height()/2, canvas.width(), canvas.height());

    // Poiché i punti sono stati salvati già scalati nel tabletEvent, ora compensi la scala per disegnarli in modo coerente con l'area visiva
    //painter.scale(1.0 / scaleFactor, 1.0 / scaleFactor);  // correggi la scala visiva
    if (pixelsPerMm > 0.0f) {
        // Inverti la conversione fatta in tabletEvent:
        // Se in tabletEvent dividi per pixelsPerMm, qui devi moltiplicare
        painter.scale(pixelsPerMm / scaleFactor, pixelsPerMm / scaleFactor); // riportare le coordinate da mm scalati a pixel del widget
    } else {
        painter.scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // === 1. Disegna la traiettoria grezza ===
    if (!recordedTrajectory.isEmpty()) {
        QPen recordPen(Qt::red);
        recordPen.setWidth(2);
        painter.setPen(recordPen);

        for (int i = 1; i < recordedTrajectory.size(); ++i) {
            painter.drawLine(recordedTrajectory[i - 1], recordedTrajectory[i]);
        }
    }

    // === 2. Disegna la traiettoria ripetuta (filtrata + ricampionata) ===
    if (currentState == DrawingState::Repeating && !recordedTrajectory.isEmpty()) {

        QPen filteredPen(Qt::darkGreen);  // puoi cambiare colore
        filteredPen.setWidth(3);
        painter.setPen(filteredPen);

        for (int i = 1; i < recordedTrajectory.size(); ++i) {
            painter.drawLine(recordedTrajectory[i - 1], recordedTrajectory[i]);
        }

        // === Cursore attivo sulla traiettoria ===
        QPen cursorPen(Qt::red); // crea una penna rossa
        cursorPen.setWidth(6);
        painter.setPen(cursorPen);
        painter.setBrush(Qt::red);

        QPointF cursorPos; //  variabile che conterrà la posizione corrente del cursore da disegnare
        if (playbackIndex >= 0 && playbackIndex < resampledTrajectory.size())
            cursorPos = resampledTrajectory[playbackIndex];
        else if (playbackIndex < 0)
            cursorPos = resampledTrajectory.first();
        else
            cursorPos = resampledTrajectory.last();

        // Disegna un piccolo cerchio rosso (diametro 4px) in corrispondenza del cursore
        painter.drawEllipse(cursorPos, 2, 2);
    }

    // === 3. Disegna la traiettoria corrente se in modalità FreeHand ===
    if (currentState == DrawingState::FreeHand && !strokePoints.isEmpty()) {
        QPen freeHandPen(Qt::black);
        freeHandPen.setWidth(3);
        painter.setPen(freeHandPen);

        for (int i = 1; i < strokePoints.size(); ++i) {
            painter.drawLine(strokePoints[i - 1], strokePoints[i]);
        }
    }

    // === 4. Disegna la traiettoria corrente se in modalità Scanning ===
    if (currentState == DrawingState::Scanning && !spiralTrajectory.isEmpty()) {
        QPen scanPen(Qt::black);
        scanPen.setWidth(2);
        painter.setPen(scanPen);
        for (int i = 1; i < scannedTrajectory.size(); ++i) {
            painter.drawLine(scannedTrajectory[i-1], scannedTrajectory[i]);
        }

        QPen spiralPen(Qt::blue);
        spiralPen.setWidth(2);
        painter.setPen(spiralPen);
        // Disegna la traiettoria percorsa finora
        int maxIndex = std::min(playbackIndex, static_cast<int>(spiralTrajectory.size() - 1));
        for (int i = 1; i <= maxIndex; ++i) {
            painter.drawLine(spiralTrajectory[i - 1], spiralTrajectory[i]);
        }
        //for (int i = 1; i < spiralTrajectory.size(); ++i) {
            //painter.drawLine(spiralTrajectory[i-1], spiralTrajectory[i]);
        //}

        // === Cursore attivo sulla traiettoria ===
        QPen cursorPen(Qt::red); // colore cursore
        cursorPen.setWidth(6); // spessore dei punti disegnati
        painter.setPen(cursorPen);
        painter.setBrush(Qt::red);

        QPointF cursorPos;
        if (playbackIndex >= 0 && playbackIndex < spiralTrajectory.size())
            cursorPos = spiralTrajectory[playbackIndex];
        else if (playbackIndex < 0)
            cursorPos = spiralTrajectory.first();
        else
            cursorPos = spiralTrajectory.last();

        // Disegna un piccolo cerchio rosso (diametro 4px) in corrispondenza del cursore
        painter.drawEllipse(cursorPos, 2, 2);
    }
}

void DrawingArea::resizeEvent(QResizeEvent *event)
{
    QPixmap newCanvas(event->size());                       // Crea una nuova immagine con le nuove dimensioni
    newCanvas.fill(Qt::white);                              // Riempie il nuovo canvas di bianco per evitare glitch grafic

    // Copia il contenuto precedente se esiste
    QPainter painter(&newCanvas);
    painter.drawPixmap(0, 0, canvas);                       // Copia il vecchio contenuto

    canvas = newCanvas;                                     // Aggiorna il canvas con la nuova dimensione
    update();                                               // Ridisegna l'interfaccia
}

void DrawingArea::keyPressEvent(QKeyEvent *event)           // Metodo emette il segnale expressKeyPressed a MainWindow ogni volta che viene premuto un ExpressKey
{

    if(debug){
        qDebug() << "Tasto premuto:" << event->text();         // Stampiamo il tasto nella console
    }

     // Avvia il timer solo se non è già attivo
    if (!scaleTimer->isActive()) {
        switch (event->key()) {

        case Qt::Key_A:

            scaleDirection = 1;    // zoom in
            scaleTimer->start();
            break;

        case Qt::Key_B:

            scaleDirection = -1;   // zoom out
            scaleTimer->start();
            break;

        case Qt::Key_R:
            if(!recordingKeyPressed){
                recordingKeyPressed = true;
            }
            break;

            /*case Qt::Key_C:
            emit expressKeyPressed("ExpressKey - C");
            break;*/
            //default:
            //  QWidget::keyPressEvent(event); // Passa l'evento alla classe base
        }
    }
}

void DrawingArea::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_B) {
        scaleTimer->stop();       // ferma il timer
        scaleDirection = 0;       // azzera la direzionep
    }

    if (event->key() == Qt::Key_R && recordingKeyPressed) {
        recordingKeyPressed = false;
        RecordToRepeat = true;
    }
}

void DrawingArea::startCalibration()
{

    calibrating = true;
    calibrationStep = 0;
    //qDebug() << "[CALIBRAZIONE] Tocca il punto iniziale (A)";
    logCalibration("[CALIBRAZIONE]: Tocca il punto iniziale (A)");
}

void DrawingArea::logCalibration(const QString &msg)
{
    emit calibrationDebugMessage(msg);
}

void DrawingArea::updateCursor(bool drawing)
{
    const int size = 16;  // dimensioni del cursore in pixel (quadrato QPximap 16x16)
    const int radius = 6; // raggio del cerchio da disegnare

    QPixmap pixmap(size, size);     // Crea una superficie di disegno (16x16 pixel)
    pixmap.fill(Qt::transparent);   // Sfondo completamente trasparente

    QPainter painter(&pixmap);      // Oggetto per disegnare sul QPixmap
    painter.setRenderHint(QPainter::Antialiasing); // Attiva l'antialiasing per bordi lisci

    QColor color = drawing ? QColor(0, 0, 0, 255) : QColor(0, 0, 0, 100); // più trasparente se non disegna
    painter.setBrush(color);    // Imposta il colore di riempimento del pennello
    painter.setPen(Qt::NoPen);  // Nessun bordo attorno al cerchio

    // Disegna un cerchio centrato nel QPixmap
    painter.drawEllipse((size - radius*2)/2, (size - radius*2)/2, radius*2, radius*2);

    QCursor customCursor(pixmap, size / 2, size / 2); // Imposta l'hotspot (punto attivo) al centro del cursore

    setCursor(customCursor);              // // Applica il cursore personalizzato al widget DrawingArea
}


void DrawingArea::setState(DrawingState newState){
    if (currentState != newState) {
        currentState = newState; // Cambia lo stato
        emit stateChanged(currentState);  // Emette un segnale con il nuovo stato

        if (newState == DrawingState::Repeating) {
            strokePoints.clear();      // cancella il tratto corrente

            //// ANALISI MATLAB DELLA TRAIETTORIA REGISTRATA
            // Salva la traiettoria registrata in un file CSV
            flag = false;
            saveTrajectoryToTextFile(recordedTrajectory);

            //// SOLUZIONE INTERPOLAZIONE LINARE ////
            // Ricampionamento della traiettoria registrata
            double spacing = 1.0f; // Esempio di spacing in pixels
            resampledTrajectory = resampleTrajectory(recordedTrajectory, spacing);
            recordedTrajectory = resampledTrajectory; // Aggiorna la traiettoria registrata

            // Salva la traiettoria interpolata e ricampionata in un file CSV
            flag = true;
            saveTrajectoryToTextFile(resampledTrajectory);

            /////////////////////////////////////////

            //// SOLUZIONE CATMULL-ROM /////
            // Ricampionamento della traiettoria registrata con una distanza fissa
            //float distance = 5.0f;  // Distanza tra i punti interpolati
            //resampledTrajectory = catmullRomSpline(recordedTrajectory, distance);  // Usa la spline Catmull-Rom
            //recordedTrajectory = resampledTrajectory; // Aggiorna la traiettoria registrata
            ///////////////////////////////

            playbackIndex = recordedTrajectory.size() - 1;         // riparte dalla  fine
            playbackForward = false;    // direzione: indietro

            // Definisco il tempo necessario per ripetere la traiettoria
            int totalPlaybackDurationMs = 3000;  // tempo fisso: 2000ms (2s)
            int N = recordedTrajectory.size();

            if (N > 1) {
                interval = totalPlaybackDurationMs / N;
                playbackTimer->setInterval(interval);
            }

            if (currentMode == SendMode::Batch) {
                emit sendFullTrajectory(recordedTrajectory, totalPlaybackDurationMs);
                if (playbackTimer->isActive())
                    playbackTimer->stop();
                return;
            }

            playbackTimer->start();    // inizia animazione

        } else if (newState == DrawingState::Scanning){


            // 1) Chiudi il contorno, se non già fatto
            if (!scannedTrajectory.isEmpty()
                && scannedTrajectory.first() != scannedTrajectory.last()) {
                scannedTrajectory.append(scannedTrajectory.first());
            }

            /// nuova possibile soluzione

            // 1) Ricampionamento uniforme del bordo esterno
            double spacing = 1.0; // distanza desiderata fra punti successivi lungo il contorno
            QList<QPointF> outer = resampleTrajectory(scannedTrajectory, spacing); // ogni segmento consecutivo è grosso modo lungo spacing
            int contourLength = outer.size(); // NOTA: QList::size() restituisce un int, non float

            // SOLUZIONE CLIPPER2
            QPointF fixedCenter = computeCentroid(outer); // fixedCenter servirà solo a misurare quando il contorno interno diventa troppo piccolo

            // 1) Parametri Clipper2
            constexpr double CLIPPER_SCALE = 100.0;      // fattore per convertire le coordinate float → interi int64_t, per mantenere precisione decimale
            float insetStep = 3.0f;                      // offset per anello in pixel
            double delta = -insetStep * CLIPPER_SCALE;   // valore passato a Clipper2 (in unità intere), negativo per indicare offset interno

            // 2) Preparazione spirale
            int maxRings = 20;
            QList<QPointF> spiral;
            spiral.reserve(outer.size() * maxRings);
            bool reverseDir = true;
            QList<QPointF> current = outer;

            for (int i = 1; i < maxRings; ++i) {
                // a) append corrente (ping-pong)
                if (reverseDir) {
                    for (int j = current.size()-1; j >= 0; --j)
                        spiral.append(current[j]);
                } else {
                    for (auto &pt : current)
                        spiral.append(pt);
                }
                reverseDir = !reverseDir;

                // b) offset interno: Costruzione del poligono per Clipper2

                // ----> costruisci Path64 da `current`
                Path64 subj;
                subj.reserve(current.size());
                for (const QPointF &p : current) {
                    // scala float → int64 per preservare precisione
                    int64_t X = int64_t(std::round(p.x() * CLIPPER_SCALE));
                    int64_t Y = int64_t(std::round(p.y() * CLIPPER_SCALE));
                    // qui usi Point64 invece di IntPoint
                    subj.emplace_back( Point64{ X, Y } ); // Il risultato è subj, un Path64 (alias std::vector<Point64>)
                }

                // ----> lancia l’offset vero e proprio
                ClipperOffset offsetter; // crea un oggetto ClipperOffset e AddPath(...) gli passa il contorno interno come soggetto
                offsetter.AddPath(  subj,
                                  JoinType::Round,    // usa Round per indicare che gli angoli devono essere arrotondati.
                                  EndType::Polygon ); // specifica un contorno chiuso

                // 3) Esegui l’offset con firma corretta: prima delta, poi il vettore di risultati
                Paths64 solution;
                offsetter.Execute( delta, solution );  //  genera in solution il nuovo poligono offsettato di delta unità (negative → verso l’interno)


                // ----> riconverti in QList<QPointF>
                // riconversione utilizzando pt.x e pt.y
                QList<QPointF> nextRing;
                nextRing.reserve(solution.front().size());
                for (const Point64 &pt : solution.front()) {
                    double fx = static_cast<double>(pt.x) / CLIPPER_SCALE;
                    double fy = static_cast<double>(pt.y) / CLIPPER_SCALE;
                    nextRing.append(QPointF(fx, fy));
                }

                // c) mantieni spacing uniforme (opzionale)
                nextRing = sampleClosedContour(nextRing, spacing);

                // d) stop se degenerato
                /*float remaining = QLineF(fixedCenter, nextRing.first()).length();
                if (remaining < 4)
                    break;*/

                saveScanTrajectoryToTextFile(spiral);
                current = nextRing;
            }

            /* SOLUZIONE FUNZIONANTE
            // 2) Calcolo del centroide e del numero di anelli
            //float insetStep = 2.0f;      // distanza di offset per ciascun anello (px)
            int maxRings  = 10;       // numero massimo di anelli
            //int maxRings  = computeMaxRings(outer, insetStep);
            QPointF fixedCenter = computeCentroid(outer);

            // 3) Costruzione della spirale “a anelli
            QList<QPointF> spiral;
            spiral.reserve(outer.size() * maxRings);
            bool reverseDir = true;
            QList<QPointF> current = outer;

            for (int i = 1; i < maxRings; ++i) {

                // a) append corrente (in avanti o indietro)
                if (reverseDir) {
                    for (int j = current.size()-1; j >= 0; --j)
                        spiral.append(current[j]);
                } else {
                    for (auto &pt : current)
                        spiral.append(pt);
                }
                reverseDir = !reverseDir;

                // b) offset interno

                // soluzione 1:
                //QList<QPointF> nextRing = insetTowardsCentroid(current, insetStep, fixedCenter);
                //nextRing = sampleClosedContour(nextRing, spacing);

                // soluzione 2: fattore di scala decrescente: da 1.0 a quasi zero
                float scale = 1.0f - (static_cast<float>(i) / maxRings);
                QList<QPointF> nextRing = scaleContour(current, fixedCenter, scale);
                nextRing = sampleClosedContour(nextRing, spacing); // campiona per mantenere spacing costante

                // c) stop se il ring è degenerato
                //if (nextRing.size() < 30) break;
                float remaining = QLineF(fixedCenter, nextRing.first()).length();
                if (remaining < 4) { // se la distanza fra il centro della figura e l'ultimo ring è minore del diametro del cursore (4px)
                    break;
                }

                saveScanTrajectoryToTextFile(spiral);
                current = nextRing;
            }
            */

            // 4) spiral è la lista dei punti dei soli ring e non contiene i punti del contorno
            if (contourLength <= spiral.size())
                spiral.erase(spiral.begin(), spiral.begin() + (contourLength-1));

            spiralTrajectory = spiral;
            saveScanTrajectoryToTextFile(spiral);


            //update();
            playbackIndex = 0;         // riparte dalla  fine
            playbackForward = true;    // direzione: indietro

            // Definisco il tempo necessario per ripetere la traiettoria
            int totalPlaybackDurationMs = 1000;  // tempo fisso: 2000ms (2s)
            int N = spiralTrajectory.size();

            if (N > 1) {
                interval = totalPlaybackDurationMs / N;
                playbackTimer->setInterval(interval);
            }

            playbackTimer->start();    // inizia animazione
        }
    }
}

/// SCANNING DELLA TRAIETTORIA ///



void DrawingArea::saveScanTrajectoryToTextFile(QList<QPointF>& points) const
{

    // Crea il nome del file con il timestamp
    QString fileName = "C://Users//mdilucchio//OneDrive - Fondazione Istituto Italiano Tecnologia//Desktop//QtLearning//Qt Network Programming//Qt_log/scanned_trajectory.txt";

    //QString fileName = "C://Users//mdilucchio//OneDrive - Fondazione Istituto Italiano Tecnologia//Desktop//QtLearning//Qt Network Programming//Qt_log/recorded_trajectory_" + timestamp + ".txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Scrivi un'intestazione opzionale (può essere utile per identificare i dati)
        out << "X,Y\n"; // Intestazione del file

        // Scrivi ogni punto
        for (const QPointF& point : points) {
            out << point.x() << "," << point.y() << "\n";
        }

        file.close();
    } else {
        qWarning() << "Impossibile aprire il file per la scrittura!";
    }
}

//// ANALISI MATLAB DELLA TRAIETTORIA REGISTRATA
void DrawingArea::saveTrajectoryToTextFile(const QList<QPointF>& points)
{
    // Ottieni il timestamp corrente nel formato "yyyy-MM-dd_HH-mm-ss"
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");

    // Crea il nome del file con il timestamp
    if(flag){
        fileName = "C://Users//mdilucchio//OneDrive - Fondazione Istituto Italiano Tecnologia//Desktop//QtLearning//Qt Network Programming//Qt_log/resampled_trajectory_" + timestamp + ".txt";
        flag = 0;
    }else{
        fileName = "C://Users//mdilucchio//OneDrive - Fondazione Istituto Italiano Tecnologia//Desktop//QtLearning//Qt Network Programming//Qt_log/recorded_trajectory_" + timestamp + ".txt";
    }

    //QString fileName = "C://Users//mdilucchio//OneDrive - Fondazione Istituto Italiano Tecnologia//Desktop//QtLearning//Qt Network Programming//Qt_log/recorded_trajectory_" + timestamp + ".txt";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Scrivi un'intestazione opzionale (può essere utile per identificare i dati)
        out << "X,Y\n"; // Intestazione del file

        // Scrivi ogni punto
        for (const QPointF& point : points) {
            out << point.x() << "," << point.y() << "\n";
        }

        file.close();
    } else {
        qWarning() << "Impossibile aprire il file per la scrittura!";
    }
}

//// FILTRAGGIO DEI PUNTI: FILTRO MEDIANO CLASSICO  ////
QPointF DrawingArea::computeMedianFilteredPoint() const
{
    /*
     calcola un punto filtrato QPointF bidimensionale con coordinate qreal (solitamente double)
     usando un filtro mediano a partire da una finestra di punti 2D memorizzati in medianBuffer
    */

    int M = medianBuffer.size();
    if (M == 0)
        return QPointF(0,0);

    QVector<qreal> xs, ys; // due vettori xs e ys contenenti le coordinate reali a virgola mobile X e Y di tutti i punti del buffer

    // Qt alloca in anticipo la memoria necessaria, ma non cambia la dimensione del vettore (che resta a zero finché non si fa append() o push_back()
    xs.reserve(M);
    ys.reserve(M);

    for (auto &p : medianBuffer) {
        xs.append(p.x());
        ys.append(p.y());
    }
    // funzione lambda per mediana: una funzione anonima assegnata alla variabile medianOf per calcolare la mediana degli elementi del vettore v
    auto medianOf = [](QVector<qreal> &v) { // a lambda prende come parametro un riferimento a un vettore QVector<qreal> &v
        std::nth_element(v.begin(), v.begin() + v.size()/2, v.end()); // funzione della libreria standard C++ che riorganizza il vettore in modo che l'elemento nella posizione v.begin() + v.size()/2 sia quello che ci sarebbe se il vettore fosse ordinato. ATTENZIONE: Non ordina l’intero vettore, ma fa solo il lavoro minimo necessario per trovare l’elemento "di mezzo".

        // A questo punto l’elemento al centro (v.size()/2) è il valore mediano se il numero di elementi è dispari
        qreal m = v[v.size()/2];

        // Se la dimensione del vettore è pari la mediana è la media dei due valori centrali
        if (v.size() % 2 == 0) {
            // per even, prendi anche l’altro centrale
            std::nth_element(v.begin(), v.begin() + v.size()/2 - 1, v.end());
            m = (m + v[v.size()/2 - 1]) / 2.0;
        }

        return m; // la lambda restituisce la mediana calcolata
    };

    // Calcola la mediana delle coordinate X e Y separatamente
    qreal mx = medianOf(xs);
    qreal my = medianOf(ys);

    // Restituisce il punto bidimensionale risultante dal filtro mediano
    return QPointF(mx, my);
}

/// FILTRO MEDIANO RADIALE
QPointF DrawingArea::computeRadialMedian() const
{
    int N = medianBuffer.size();
    if (N == 0)
        return QPointF(); // restituisco il punto (0,0) di default

    // 1) Conversione in coordinate polari
    QVector<double> rs, thetas;

     // Qt alloca in anticipo la memoria necessaria, ma non cambia la dimensione del vettore (che resta a zero finché non si fa append() o push_back()
    rs.reserve(N);
    thetas.reserve(N);

    for (const auto &p : medianBuffer) { // Per ogni punto di medianBuffer
        double r = std::hypot(p.x(), p.y()); // calcola la distanza dal centro
        double th = std::atan2(p.y(), p.x()); // restituisce l’angolo 𝜃 ∈ (−𝜋,𝜋]
        // Salva i raggi e gli angoli
        rs.append(r);
        thetas.append(th);
    }

    // 2) Mediana del raggio
    auto linearMedian = [&](QVector<double> v) { // una lambda (funzione inline) che calcola la mediana di un vettore v di numeri reali
        int n = v.size();
        std::nth_element(v.begin(), v.begin() + n/2, v.end()); // trova l’elemento centrale senza ordinare tutto il vettore
        double m = v[n/2];

        // Se il numero di elementi è pari, calcola la media dei due valori centrali.
        if (n % 2 == 0) {
            // mediana media dei due centrali
            std::nth_element(v.begin(), v.begin() + n/2 - 1, v.end());
            m = (m + v[n/2 - 1]) / 2.0;
        }

        return m;
    };

    double r_med = linearMedian(rs); // Calcola la mediana del vettore rs

    // 3) Mediana angolare robusta
    // 3a) ordino thetas
    QVector<double> phi = thetas; // Copia gli angoli in phi
    std::sort(phi.begin(), phi.end()); // Li ordina in ordine crescente da −π a π per preparare il calcolo dei “gap”

    // 3b) trovo il gap massimo
    double maxGap = 0;
    int k = 0;

    // Calcola la differenza (gap) tra ogni coppia consecutiva di angoli ordinati.
    for (int i = 0; i < N - 1; ++i) {
        double gap = phi[i+1] - phi[i];
        if (gap > maxGap) {

            // Trova il gap più grande (max gj) e l’indice k corrispondente
            maxGap = gap;
            k = i;
        }
    }

    // Calcola anche il gap “circolare” tra l’ultimo e il primo angolo (considerando il cerchio da -π a π)
    double wrapGap = (phi[0] + 2*M_PI) - phi[N-1];
    if (wrapGap > maxGap) {
        maxGap = wrapGap;
        k = N - 1; // L’indice k segna dove c’è la massima discontinuità; questo è il punto migliore per “aprire” il cerchio
    }

    // 3c) eliminare la discontinuità
    double alpha = phi[k]; // // Scelgo l’angolo α = ϕk
    // costruiamo psi ruotati
    QVector<double> psi;
    psi.reserve(N);

    // Per ogni angolo ang in phi, calcolo d = ang - alpha; se è negativo, aggiungo 2𝜋
    for (double ang : phi) {
        double d = ang - alpha;
        psi.append(d >= 0 ? d : d + 2*M_PI);
    }

    // 3d) Calcolo la mediana lineare di psi con la stessa lambda linearMedian
    double psi_med = linearMedian(psi);
    // Riporto psi_med in valore angolare standard [-π, π] aggiungendo alpha
    double theta_med = psi_med + alpha;
    if (theta_med > M_PI) // Se supera π, sottraggo 2π per rientrare in (-π, π]
        theta_med -= 2*M_PI;

    // 4) Ricostruzione del punto cartesiano mediano
    double x_med = r_med * std::cos(theta_med);
    double y_med = r_med * std::sin(theta_med);

    return QPointF(x_med, y_med); // Restituisce il punto mediano come QPointF
}

/// SCANNING DELLA TRAIETTORIA


//// SOLUZIONE INTERPOLAZIONE LINEARE ////
// SOLUZIONE MICHELE

/*QList<QPointF> DrawingArea::resampleTrajectory(const QList<QPointF>& trajectory, float spacing) {

    // ha lo scopo di interpolare linearmente una traiettoria e di ricampionarla in modo che i punti risultanti siano equidistanti tra loro

    if (trajectory.size() < 2) {
        return trajectory; // Non possiamo interpolare con meno di 2 punti
    }

    QPointF newPreviousPoint = trajectory[0]; // punto di partenza da cui interpolare
    float residualDistance = 0; // tiene traccia della distanza “in avanzo” dal segmento precedente
    resampledTrajectory.append(newPreviousPoint);

    for (int i = 1; i < trajectory.size(); ++i) { // Loop su tutti i segmenti della traiettoria
        QPointF currentPoint = trajectory[i];
        QLineF segment(newPreviousPoint, currentPoint);
        float segmentLength = segment.length(); // Per ogni coppia (newPreviousPoint, currentPoint) si calcola la lunghezza del segmento

        if (segmentLength == 0.0f || segmentLength < spacing) {
            continue; // Segmento nullo, salta
        }

        QPointF direction = (currentPoint - newPreviousPoint) / segmentLength; // Calcola il versore direzionale del segmento
        float distanceRemaining = segmentLength;

        // Interpolazione e ricampionamento
        while (distanceRemaining >= spacing) {
            float step = spacing - residualDistance;
            QPointF newPoint = newPreviousPoint + direction * (step);
            resampledTrajectory.append(newPoint);
            residualDistance = 0;

            // Aggiorna lo stato per continuare dal nuovo punto
            newPreviousPoint = newPoint; // Si aggiorna il punto di partenza
            distanceRemaining = QLineF(newPreviousPoint, currentPoint).length(); // distanza rimanente al currentPoint

            direction = (currentPoint - newPreviousPoint) / distanceRemaining; // calcola il nuovo versore
        }

        // Dopo aver fatto tutto il possibile con questo segmento, accumula la distanza residua
        residualDistance = distanceRemaining; //  distanza rimanente da usare nel prossimo segmento
        newPreviousPoint = currentPoint;

    }

    return resampledTrajectory;
}*/

// SOLUZIONE LAPO

QList<QPointF> DrawingArea::resampleTrajectory(const QList<QPointF>& trajectory, float spacing) {

    // ha lo scopo di interpolare linearmente una traiettoria e di ricampionarla in modo che i punti risultanti siano equidistanti tra loro

    if (trajectory.size() < 2) {
        return trajectory; // Non possiamo interpolare con meno di 2 punti
    }

    QPointF newPreviousPoint = trajectory[0]; // punto di partenza da cui interpolare
    QPointF newPoint = newPreviousPoint;
    float residualDistance = 0; // tiene traccia della distanza “in avanzo” dal segmento precedente

    QList<QPointF> newTraj;
    newTraj.append(newPreviousPoint);
    //resampledTrajectory.append(newPreviousPoint);

    for (int i = 1; i < trajectory.size(); ++i) { // Loop su tutti i segmenti della traiettoria
        QPointF currentPoint = trajectory[i];
        QLineF segment(newPreviousPoint, currentPoint);
        float segmentLength = segment.length(); // Per ogni coppia (newPreviousPoint, currentPoint) si calcola la lunghezza del segmento

        if (segmentLength == 0.0f || segmentLength < spacing) {
            continue; // Segmento nullo, salta ad un nuovo currentpoint
        }

        QPointF direction = (currentPoint - newPreviousPoint) / segmentLength; // Calcola il versore direzionale del segmento
        float distanceRemaining = segmentLength;

        // Interpolazione LINEARE e ricampionamento
        while (distanceRemaining >= spacing) {
            float step = spacing;
            newPoint = newPoint + direction * (step);
            //resampledTrajectory.append(newPoint);
            newTraj.append(newPoint);
            residualDistance = 0;

            // Aggiorna lo stato per continuare dal nuovo punto
            newPreviousPoint = newPoint; // Si aggiorna il punto di partenza
            distanceRemaining = QLineF(newPreviousPoint, currentPoint).length(); // distanza rimanente al currentPoint

            direction = (currentPoint - newPreviousPoint) / distanceRemaining; // calcola il nuovo versore
        }

        // Dopo aver fatto tutto il possibile con questo segmento, accumula la distanza residua
        residualDistance = distanceRemaining; //  distanza rimanente da usare nel prossimo segmento
        newPreviousPoint = newPoint;

    }

    return newTraj;
    //return resampledTrajectory;
}

/// SOLUZIONE INTERPOLAZIONE CATMULL-ROM ///

/*QList<QPointF> DrawingArea::catmullRomSpline(const QList<QPointF>& trajectory, float distance) {
    //  prende la traiettoria registrata e la suddivide in nuovi punti, con una distanza costante d tra i punti interpolati lungo la curva
    QList<QPointF> splinePoints;

    if (trajectory.size() < 4) return splinePoints; // Almeno 4 punti necessari per formare una spline

    // Aggiungi i primi 2 punti (non interpolabili)
    splinePoints.append(trajectory[0]);
    splinePoints.append(trajectory[1]);

    // Per ogni gruppo di 4 punti consecutivi
    for (int i = 1; i < trajectory.size() - 2; ++i) {
        QPointF p0 = trajectory[i - 1]; // Punto precedente
        QPointF p1 = trajectory[i]; // Punto corrente
        QPointF p2 = trajectory[i + 1]; // Punto successivo
        QPointF p3 = trajectory[i + 2]; // Punto dopo il successivo

        // Calcola la lunghezza del segmento tra P1 e P2
        float segmentLength = QLineF(p1, p2).length();
        int numNewPoints = int(segmentLength / distance);  // Calcola il numero di punti da generare lungo il segmento

        // Calcolare i punti interpolati tra P1 e P2 con la spline Catmull-Rom
        for (int j = 1; j < numNewPoints; ++j) {
            float t = float(j) / float(numNewPoints);  // Calcola la posizione relativa lungo il segmento
            QPointF newPoint = catmullRom(p0, p1, p2, p3, t);  // Calcola il nuovo punto usando la spline
            splinePoints.append(newPoint);  // Aggiungi il nuovo punto alla traiettoria
        }

        splinePoints.append(p2);  // Aggiungi il punto finale del segmento
    }

    splinePoints.append(trajectory[trajectory.size() - 2]);
    splinePoints.append(trajectory[trajectory.size() - 1]);

    QPointF previousPoint = splinePoints[0];
    for (int i = 1; i < splinePoints.size(); ++i) {
        QPointF currentPoint = splinePoints[i];
        CatmullsegmentLength.append(QLineF(previousPoint, currentPoint).length()); // per debug
        previousPoint = currentPoint;
    }
    return splinePoints;
}
QPointF DrawingArea::catmullRom(const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3, float t) {
    // Formula della spline Catmull-Rom : calcola un punto su una curva definita da 4 punti consecutivi
    float x = 0.5 * ((2 * p1.x()) + (-p0.x() + p2.x()) * t + (2 * p0.x() - 5 * p1.x() + 4 * p2.x() - p3.x()) * t * t + (-p0.x() + 3 * p1.x() - 3 * p2.x() + p3.x()) * t * t * t);
    float y = 0.5 * ((2 * p1.y()) + (-p0.y() + p2.y()) * t + (2 * p0.y() - 5 * p1.y() + 4 * p2.y() - p3.y()) * t * t + (-p0.y() + 3 * p1.y() - 3 * p2.y() + p3.y()) * t * t * t);
    return QPointF(x, y);
}*/
