/*
#ifndef PENMANAGER_H
#define PENMANAGER_H

#include <QApplication>                 // QApplication, il cuore dell'app Qt, gestisce l'ingresso e uscita della tavoletta dal sistema
#include "drawingarea.h"
#include "mainwindow.h"


class PenManager : public QApplication
{
    Q_OBJECT

public:
    using QApplication::QApplication; //  Usa i costruttori di QApplication

    bool event(QEvent *event) override; // Override per intercettare eventi globali (il rilevamento della tavoletta grafica)

    // Salviamo i riferimenti agli oggetti della GUI in modo che l'applicazione possa aggiornare la GUI (intesa come  la finestra principale o l'area di disegno) quando necessario.
    void setDrawingArea(DrawingArea *area) { m_drawingArea = area; }
    void setMainWindow(MainWindow *window) { m_mainWindow = window; }

private:
    DrawingArea *m_drawingArea = nullptr;   // riferimento alla DrawingArea
    MainWindow *m_mainWindow = nullptr;     // riferimento alla MainWindow
};
#endif // PENMANAGER_H
*/
