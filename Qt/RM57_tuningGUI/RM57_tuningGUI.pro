QT       += core gui serialport # serialport è l'estenzione necessaria per una comunicazione seriale (COM)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport    # printsupport è l'estenzione necessaria per usare la libreria qcustomplot

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Commenta le seguenti due righe se non vuoi utilizzare il MCU simulato
#DEFINES += MCU_SIM
#DEFINES += MCU_SIM_RUN=1    # metti 0 per compilare ma NON lanciare mai il thread sim

contains(DEFINES, MCU_SIM) {
    HEADERS += mcusimworker.h
    SOURCES += mcusimworker.cpp
}

SOURCES += \
    devicedialog.cpp \
    main.cpp \
    mainwindow.cpp \
    plotzoomdialog.cpp \
    qcustomplot.cpp \
    serialworker.cpp

HEADERS += \
    devicedialog.h \
    mainwindow.h \
    plotzoomdialog.h \
    protocol.h \
    qcustomplot.h \
    serialworker.h

FORMS += \
    devicedialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.txt
