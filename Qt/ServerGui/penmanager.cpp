/*
 #include "penmanager.h"
#include <QTabletEvent>
#include <QDebug>

bool PenManager::event(QEvent *event)
{
    if (event->type() == QEvent::TabletEnterProximity) { //  Segnala che il pennino è in prossimità del tablet → Cambia stato in verde
        if (m_mainWindow) {
            //m_mainWindow->updatePenStatus(true);
        }
        return true;
    }
    if (event->type() == QEvent::TabletLeaveProximity) { // Segnala che il pennino non è in prossimità del tablet → Cambia stato in rosso
        if (m_mainWindow) {
            //m_mainWindow->updatePenStatus(false);
        }
        return true;
    }
    // Se l'evento non è di tipo TabletEnterProximity o TabletLeaveProximity, lo passa normalmente a QApplication
    return QApplication::event(event);
}
*/
