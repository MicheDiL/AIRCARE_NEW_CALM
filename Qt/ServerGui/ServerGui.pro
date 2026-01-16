QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# --- inizia integrazione della libreria Clipper2 ---
# aggiunge il path ai file header
INCLUDEPATH += $$PWD/thirdparty/clipper2/include #  Non includere fino alla seconda clipper2, perché i file .h sono già dentro quella cartella. L’#include nel tuo codice farà il resto.

# se Clipper2 avesse file .cpp da compilare, aggiungili qui
SOURCES += \
    $$PWD/thirdparty/clipper2/src/clipper.engine.cpp \
    $$PWD/thirdparty/clipper2/src/clipper.offset.cpp \
    $$PWD/thirdparty/clipper2/src/clipper.rectclip.cpp \
    myudpserver.cpp

# se Clipper2 richiede definizioni di macro, aggiungile:
# DEFINES += CLIPPER_NO_STD_VECTOR   # (esempio)
# --- fine integrazione Clipper2 ---

SOURCES += \
    MyTcpServer.cpp \
    drawingarea.cpp \
    main.cpp \
    mainwindow.cpp \
    penmanager.cpp \
    toastmessage.cpp \
    usbmanager.cpp \
    usbstartconnection.cpp \
    wacomcalibrator.cpp

HEADERS += \
    MyTcpServer.h \
    drawingarea.h \
    mainwindow.h \
    myudpserver.h \
    penmanager.h \
    toastmessage.h \
    usbmanager.h \
    usbstartconnection.h \
    wacomcalibrator.h

FORMS += \
    mainwindow.ui

#RESOURCES += resources.qrc # Questo dice a Qt di includere i file nel .qrc nel build finale.

# Qui diciamo al linker di includere setupapi.lib
win32:LIBS += -lsetupapi

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

