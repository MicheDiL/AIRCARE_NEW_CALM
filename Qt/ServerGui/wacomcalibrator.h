#ifndef WACOMCALIBRATOR_H
#define WACOMCALIBRATOR_H

#include <QDialog>
#include <QPointF>

class DrawingArea;

class wacomcalibrator : public QDialog
{
    Q_OBJECT
public:
    wacomcalibrator(DrawingArea *drawingArea, QWidget *parent = nullptr);
    void start();

private:
    DrawingArea *m_drawingArea;

private slots:
    void onCalibrationDone();
};

#endif // WACOMCALIBRATOR_H
