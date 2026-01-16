#include "wacomcalibrator.h"
#include "drawingarea.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include "toastmessage.h"  // includi il nuovo widget toast

wacomcalibrator::wacomcalibrator(DrawingArea *drawingArea, QWidget *parent)
    : QDialog(parent), m_drawingArea(drawingArea)
{
    setWindowTitle("Wacom Calibration");
    setMinimumSize(300, 150);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Per calibrare correttamente la tavoletta Wacom esegui le seguenti istruzioni:\n"
                               "1) Vai su Operate\n"
                               "2) Premi su \"Inizia Calibrazione\"\n"
                               "3) Tocca due punti bianchi adiacenti sulla tavoletta");
    label->setStyleSheet("font-family: 'Segoe UI'; font-size: 13px; color: black;");
    layout->addWidget(label);QPushButton *btnStart = new QPushButton("Inizia Calibrazione");
    layout->addWidget(btnStart);

    connect(btnStart, &QPushButton::clicked, this, [this]() {
        this->accept();
        m_drawingArea->startCalibration();
    });

    connect(m_drawingArea, &DrawingArea::calibrationDebugMessage, this, [this](const QString &msg){
        if (parentWidget()) {
            new toastmessage(msg, parentWidget());
        }
    });
    connect(m_drawingArea, &DrawingArea::calibrationDone, this, &wacomcalibrator::onCalibrationDone);
}

void wacomcalibrator::onCalibrationDone()
{
    //qDebug() << "[CALIBRATION] Calibrazione completata. pixelsPerMm =" << pixelsPerMm;
    // Mostra il messaggio in un toast sopra la MainWindow
    if (parentWidget()) {
        new toastmessage(QString("Calibrazione completata!"));
    }
    accept(); // Chiude il dialogo
}
