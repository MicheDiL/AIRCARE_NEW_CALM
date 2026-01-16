/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFormLayout *formLayout;
    QVBoxLayout *verticalLayoutMain;
    QGroupBox *grpSerial;
    QGridLayout *gridLayout;
    QGridLayout *gridSerial;
    QLabel *lblConnText;
    QPushButton *btnDisconnect;
    QLabel *lblLed;
    QLabel *label_3;
    QPushButton *btnConnect;
    QComboBox *cmbPort;
    QFrame *frameUsb;
    QVBoxLayout *verticalLayout;
    QLabel *label_usb_img;
    QLabel *label_2;
    QComboBox *cmbBaud;
    QLabel *label;
    QGroupBox *grpTuning;
    QGridLayout *gridLayout_2;
    QCheckBox *chkLive;
    QPushButton *btnApply;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabTuning;
    QWidget *tabDuty;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *FunctionType;
    QLabel *TriggerFunction;
    QComboBox *cmbDutyType;
    QStackedWidget *stkDutyParams;
    QWidget *pageDutySine;
    QWidget *formLayoutWidget_5;
    QFormLayout *formDutySine;
    QLabel *MinDutyForSine;
    QDoubleSpinBox *spnDutyMinForSine;
    QLabel *MaxDutyForSine;
    QDoubleSpinBox *spnDutyMaxForSine;
    QLabel *FreqDutyForSine;
    QDoubleSpinBox *spnDutyFreqForSine;
    QWidget *pageDutyConst;
    QWidget *formLayoutWidget_6;
    QFormLayout *formDutyConst;
    QLabel *ValDuty;
    QDoubleSpinBox *spnDutyVal;
    QWidget *pageDutyTriang;
    QWidget *formLayoutWidget_7;
    QFormLayout *formDutyTriang;
    QLabel *MinDutyForTriang;
    QDoubleSpinBox *spnDutyMinForTriang;
    QLabel *MaxDutyForTriang;
    QDoubleSpinBox *spnDutyMaxForTriang;
    QLabel *StepDutyForTriang;
    QDoubleSpinBox *spnDutyStepForTriang;
    QLabel *StepDurationForTriang;
    QDoubleSpinBox *spnDutyStepDurationForTriang;
    QFormLayout *formDuty;
    QLabel *MinDuty;
    QDoubleSpinBox *spnDutyMin;
    QLabel *MaxDuty;
    QDoubleSpinBox *spnDutyMax;
    QLabel *FreqDuty;
    QDoubleSpinBox *spnDutyFreq;
    QWidget *tabCurrent;
    QWidget *formLayoutWidget_2;
    QFormLayout *formCurrent;
    QLabel *MinCur;
    QDoubleSpinBox *spnCurMin;
    QLabel *MaxCur;
    QDoubleSpinBox *spnCurMax;
    QLabel *FreqCur;
    QDoubleSpinBox *spnCurFreq;
    QWidget *tabPosition;
    QWidget *formLayoutWidget_3;
    QFormLayout *formPosition;
    QDoubleSpinBox *spnPosMin;
    QDoubleSpinBox *spnPosMax;
    QDoubleSpinBox *spnPosFreq;
    QLabel *MinPos;
    QLabel *MaxPos;
    QLabel *FreqPos;
    QWidget *tabPid;
    QWidget *formLayoutWidget_4;
    QFormLayout *formPID;
    QDoubleSpinBox *spnKp;
    QDoubleSpinBox *spnKi;
    QDoubleSpinBox *spnKd;
    QLabel *Kp;
    QLabel *Ki;
    QLabel *Kd;
    QHBoxLayout *horizontalLayout_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(733, 791);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(61, 61, 61, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        QBrush brush2(QColor(91, 91, 91, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        QBrush brush3(QColor(76, 76, 76, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush3);
        QBrush brush4(QColor(30, 30, 30, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush4);
        QBrush brush5(QColor(41, 41, 41, 255));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush4);
        QBrush brush7(QColor(255, 255, 220, 255));
        brush7.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush8(QColor(255, 255, 255, 127));
        brush8.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush6);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush6);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush9(QColor(30, 30, 30, 127));
        brush9.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush9);
#endif
        QBrush brush10(QColor(43, 43, 43, 255));
        brush10.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush10);
#endif
        MainWindow->setPalette(palette);
        QFont font;
        font.setBold(true);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        formLayout = new QFormLayout(centralwidget);
        formLayout->setObjectName("formLayout");
        verticalLayoutMain = new QVBoxLayout();
        verticalLayoutMain->setObjectName("verticalLayoutMain");
        verticalLayoutMain->setContentsMargins(-1, 0, 0, -1);
        grpSerial = new QGroupBox(centralwidget);
        grpSerial->setObjectName("grpSerial");
        grpSerial->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(grpSerial->sizePolicy().hasHeightForWidth());
        grpSerial->setSizePolicy(sizePolicy);
        grpSerial->setMaximumSize(QSize(16777215, 16777215));
        grpSerial->setSizeIncrement(QSize(0, 0));
        QPalette palette1;
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        QBrush brush11(QColor(127, 127, 127, 255));
        brush11.setStyle(Qt::BrushStyle::SolidPattern);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        QBrush brush12(QColor(170, 170, 170, 255));
        brush12.setStyle(Qt::BrushStyle::SolidPattern);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush13(QColor(0, 0, 0, 127));
        brush13.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush14(QColor(127, 127, 127, 127));
        brush14.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        grpSerial->setPalette(palette1);
        grpSerial->setStyleSheet(QString::fromUtf8("color:white;\n"
"border -width: 3px;"));
        gridLayout = new QGridLayout(grpSerial);
        gridLayout->setObjectName("gridLayout");
        gridSerial = new QGridLayout();
        gridSerial->setSpacing(7);
        gridSerial->setObjectName("gridSerial");
        lblConnText = new QLabel(grpSerial);
        lblConnText->setObjectName("lblConnText");

        gridSerial->addWidget(lblConnText, 2, 2, 1, 1);

        btnDisconnect = new QPushButton(grpSerial);
        btnDisconnect->setObjectName("btnDisconnect");
        btnDisconnect->setEnabled(false);
        QPalette palette2;
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush12);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        QBrush brush15(QColor(212, 212, 212, 255));
        brush15.setStyle(Qt::BrushStyle::SolidPattern);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush15);
        QBrush brush16(QColor(85, 85, 85, 255));
        brush16.setStyle(Qt::BrushStyle::SolidPattern);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush16);
        QBrush brush17(QColor(113, 113, 113, 255));
        brush17.setStyle(Qt::BrushStyle::SolidPattern);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush17);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush12);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush15);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush12);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush15);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush16);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush17);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush12);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush15);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush16);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush12);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush15);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush16);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush17);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush16);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush16);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush12);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush12);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush12);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush18(QColor(85, 85, 85, 127));
        brush18.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush18);
#endif
        QBrush brush19(QColor(221, 221, 221, 255));
        brush19.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette2.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush19);
#endif
        btnDisconnect->setPalette(palette2);
        btnDisconnect->setFont(font);
        btnDisconnect->setStyleSheet(QString::fromUtf8("color:black"));

        gridSerial->addWidget(btnDisconnect, 3, 2, 1, 1);

        lblLed = new QLabel(grpSerial);
        lblLed->setObjectName("lblLed");
        lblLed->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblLed->sizePolicy().hasHeightForWidth());
        lblLed->setSizePolicy(sizePolicy1);
        lblLed->setMinimumSize(QSize(16, 16));
        lblLed->setMaximumSize(QSize(16, 16));
        lblLed->setStyleSheet(QString::fromUtf8("QLabel#lblLed {\n"
"  background-color: rgb(200, 0, 0);\n"
"  border: 3px solid;\n"
"  border-radius: 8px;\n"
"  border-color: white;\n"
"}"));

        gridSerial->addWidget(lblLed, 2, 3, 1, 1);

        label_3 = new QLabel(grpSerial);
        label_3->setObjectName("label_3");
        label_3->setFont(font);

        gridSerial->addWidget(label_3, 2, 1, 1, 1);

        btnConnect = new QPushButton(grpSerial);
        btnConnect->setObjectName("btnConnect");
        QPalette palette3;
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        QBrush brush20(QColor(183, 183, 183, 255));
        brush20.setStyle(Qt::BrushStyle::SolidPattern);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush20);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        QBrush brush21(QColor(219, 219, 219, 255));
        brush21.setStyle(Qt::BrushStyle::SolidPattern);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush21);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush2);
        QBrush brush22(QColor(122, 122, 122, 255));
        brush22.setStyle(Qt::BrushStyle::SolidPattern);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush22);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush20);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush21);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush20);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush21);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush2);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush22);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush20);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush21);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush20);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush21);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush22);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush2);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush20);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush20);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush20);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush23(QColor(91, 91, 91, 127));
        brush23.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush23);
#endif
        QBrush brush24(QColor(238, 238, 238, 255));
        brush24.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette3.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush24);
#endif
        btnConnect->setPalette(palette3);
        btnConnect->setFont(font);
        btnConnect->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        btnConnect->setStyleSheet(QString::fromUtf8("color:black"));

        gridSerial->addWidget(btnConnect, 3, 1, 1, 1);

        cmbPort = new QComboBox(grpSerial);
        cmbPort->setObjectName("cmbPort");
        QPalette palette4;
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette4.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette4.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette4.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        cmbPort->setPalette(palette4);
        cmbPort->setFont(font);
        cmbPort->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);"));

        gridSerial->addWidget(cmbPort, 0, 2, 1, 1);

        frameUsb = new QFrame(grpSerial);
        frameUsb->setObjectName("frameUsb");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frameUsb->sizePolicy().hasHeightForWidth());
        frameUsb->setSizePolicy(sizePolicy2);
        frameUsb->setMaximumSize(QSize(100, 100));
        frameUsb->setFrameShape(QFrame::Shape::StyledPanel);
        frameUsb->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(frameUsb);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_usb_img = new QLabel(frameUsb);
        label_usb_img->setObjectName("label_usb_img");
        sizePolicy2.setHeightForWidth(label_usb_img->sizePolicy().hasHeightForWidth());
        label_usb_img->setSizePolicy(sizePolicy2);
        label_usb_img->setMaximumSize(QSize(164, 164));
        label_usb_img->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(label_usb_img);


        gridSerial->addWidget(frameUsb, 0, 0, 4, 1);

        label_2 = new QLabel(grpSerial);
        label_2->setObjectName("label_2");
        QPalette palette5;
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette5.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette5.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette5.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        label_2->setPalette(palette5);
        label_2->setFont(font);

        gridSerial->addWidget(label_2, 1, 1, 1, 1);

        cmbBaud = new QComboBox(grpSerial);
        cmbBaud->addItem(QString());
        cmbBaud->addItem(QString());
        cmbBaud->setObjectName("cmbBaud");
        QPalette palette6;
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette6.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette6.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette6.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        cmbBaud->setPalette(palette6);
        cmbBaud->setFont(font);
        cmbBaud->setStyleSheet(QString::fromUtf8("background-color: rgb(61, 61, 61);"));

        gridSerial->addWidget(cmbBaud, 1, 2, 1, 1);

        label = new QLabel(grpSerial);
        label->setObjectName("label");
        QPalette palette7;
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette7.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette7.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette7.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        label->setPalette(palette7);
        QFont font1;
        font1.setBold(true);
        font1.setKerning(true);
        label->setFont(font1);
        label->setAutoFillBackground(false);
        label->setStyleSheet(QString::fromUtf8("color:white"));

        gridSerial->addWidget(label, 0, 1, 1, 1);


        gridLayout->addLayout(gridSerial, 0, 2, 1, 1);


        verticalLayoutMain->addWidget(grpSerial);

        grpTuning = new QGroupBox(centralwidget);
        grpTuning->setObjectName("grpTuning");
        grpTuning->setMaximumSize(QSize(16777215, 16777215));
        grpTuning->setFont(font);
        grpTuning->setCheckable(false);
        gridLayout_2 = new QGridLayout(grpTuning);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        gridLayout_2->setContentsMargins(-1, -1, -1, 11);
        chkLive = new QCheckBox(grpTuning);
        chkLive->setObjectName("chkLive");
        QPalette palette8;
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        QBrush brush25(QColor(157, 157, 157, 255));
        brush25.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush25);
        QBrush brush26(QColor(235, 235, 235, 255));
        brush26.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush26);
        QBrush brush27(QColor(196, 196, 196, 255));
        brush27.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush27);
        QBrush brush28(QColor(78, 78, 78, 255));
        brush28.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush28);
        QBrush brush29(QColor(105, 105, 105, 255));
        brush29.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush29);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush25);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        QBrush brush30(QColor(206, 206, 206, 255));
        brush30.setStyle(Qt::BrushStyle::SolidPattern);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush30);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette8.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush25);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush26);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush27);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush28);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush29);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush25);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush30);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette8.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush28);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush25);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush26);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush27);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush28);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush29);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush28);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush28);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush25);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush25);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush25);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush31(QColor(78, 78, 78, 127));
        brush31.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush31);
#endif
        QBrush brush32(QColor(204, 204, 204, 255));
        brush32.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette8.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush32);
#endif
        chkLive->setPalette(palette8);
        chkLive->setStyleSheet(QString::fromUtf8("color:white;"));

        gridLayout_2->addWidget(chkLive, 2, 0, 1, 1);

        btnApply = new QPushButton(grpTuning);
        btnApply->setObjectName("btnApply");
        QPalette palette9;
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        QBrush brush33(QColor(187, 187, 187, 255));
        brush33.setStyle(Qt::BrushStyle::SolidPattern);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush33);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush19);
        QBrush brush34(QColor(93, 93, 93, 255));
        brush34.setStyle(Qt::BrushStyle::SolidPattern);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush34);
        QBrush brush35(QColor(125, 125, 125, 255));
        brush35.setStyle(Qt::BrushStyle::SolidPattern);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush35);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush33);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush19);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette9.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush33);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush19);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush34);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush35);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush33);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush19);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette9.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush34);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush33);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush19);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush34);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush35);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush34);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush34);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush33);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush33);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush33);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush36(QColor(93, 93, 93, 127));
        brush36.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush36);
#endif
        QBrush brush37(QColor(243, 243, 243, 255));
        brush37.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette9.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush37);
#endif
        btnApply->setPalette(palette9);

        gridLayout_2->addWidget(btnApply, 3, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, 0, 0, 0);
        tabTuning = new QTabWidget(grpTuning);
        tabTuning->setObjectName("tabTuning");
        QPalette palette10;
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush12);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush15);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush16);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush17);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush12);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush15);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette10.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush12);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush15);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush16);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush17);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush12);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush15);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette10.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush16);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush12);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush15);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush16);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush17);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush16);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush16);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush12);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush12);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush12);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush18);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette10.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush19);
#endif
        tabTuning->setPalette(palette10);
        tabDuty = new QWidget();
        tabDuty->setObjectName("tabDuty");
        verticalLayout_2 = new QVBoxLayout(tabDuty);
        verticalLayout_2->setObjectName("verticalLayout_2");
        FunctionType = new QHBoxLayout();
        FunctionType->setObjectName("FunctionType");
        TriggerFunction = new QLabel(tabDuty);
        TriggerFunction->setObjectName("TriggerFunction");

        FunctionType->addWidget(TriggerFunction);

        cmbDutyType = new QComboBox(tabDuty);
        cmbDutyType->addItem(QString());
        cmbDutyType->addItem(QString());
        cmbDutyType->addItem(QString());
        cmbDutyType->setObjectName("cmbDutyType");

        FunctionType->addWidget(cmbDutyType);


        verticalLayout_2->addLayout(FunctionType);

        stkDutyParams = new QStackedWidget(tabDuty);
        stkDutyParams->setObjectName("stkDutyParams");
        pageDutySine = new QWidget();
        pageDutySine->setObjectName("pageDutySine");
        formLayoutWidget_5 = new QWidget(pageDutySine);
        formLayoutWidget_5->setObjectName("formLayoutWidget_5");
        formLayoutWidget_5->setGeometry(QRect(0, 20, 651, 261));
        formDutySine = new QFormLayout(formLayoutWidget_5);
        formDutySine->setObjectName("formDutySine");
        formDutySine->setContentsMargins(0, 0, 0, 0);
        MinDutyForSine = new QLabel(formLayoutWidget_5);
        MinDutyForSine->setObjectName("MinDutyForSine");

        formDutySine->setWidget(0, QFormLayout::ItemRole::LabelRole, MinDutyForSine);

        spnDutyMinForSine = new QDoubleSpinBox(formLayoutWidget_5);
        spnDutyMinForSine->setObjectName("spnDutyMinForSine");
        QPalette palette11;
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette11.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette11.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette11.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMinForSine->setPalette(palette11);
        spnDutyMinForSine->setMaximum(100.000000000000000);
        spnDutyMinForSine->setSingleStep(0.100000000000000);

        formDutySine->setWidget(0, QFormLayout::ItemRole::FieldRole, spnDutyMinForSine);

        MaxDutyForSine = new QLabel(formLayoutWidget_5);
        MaxDutyForSine->setObjectName("MaxDutyForSine");

        formDutySine->setWidget(1, QFormLayout::ItemRole::LabelRole, MaxDutyForSine);

        spnDutyMaxForSine = new QDoubleSpinBox(formLayoutWidget_5);
        spnDutyMaxForSine->setObjectName("spnDutyMaxForSine");
        QPalette palette12;
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette12.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette12.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette12.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMaxForSine->setPalette(palette12);
        spnDutyMaxForSine->setMaximum(100.000000000000000);
        spnDutyMaxForSine->setSingleStep(0.100000000000000);
        spnDutyMaxForSine->setValue(10.000000000000000);

        formDutySine->setWidget(1, QFormLayout::ItemRole::FieldRole, spnDutyMaxForSine);

        FreqDutyForSine = new QLabel(formLayoutWidget_5);
        FreqDutyForSine->setObjectName("FreqDutyForSine");

        formDutySine->setWidget(2, QFormLayout::ItemRole::LabelRole, FreqDutyForSine);

        spnDutyFreqForSine = new QDoubleSpinBox(formLayoutWidget_5);
        spnDutyFreqForSine->setObjectName("spnDutyFreqForSine");
        QPalette palette13;
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette13.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette13.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette13.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyFreqForSine->setPalette(palette13);
        spnDutyFreqForSine->setDecimals(0);
        spnDutyFreqForSine->setMaximum(10000.000000000000000);
        spnDutyFreqForSine->setSingleStep(0.500000000000000);
        spnDutyFreqForSine->setValue(10.000000000000000);

        formDutySine->setWidget(2, QFormLayout::ItemRole::FieldRole, spnDutyFreqForSine);

        stkDutyParams->addWidget(pageDutySine);
        pageDutyConst = new QWidget();
        pageDutyConst->setObjectName("pageDutyConst");
        formLayoutWidget_6 = new QWidget(pageDutyConst);
        formLayoutWidget_6->setObjectName("formLayoutWidget_6");
        formLayoutWidget_6->setGeometry(QRect(0, 10, 653, 91));
        formDutyConst = new QFormLayout(formLayoutWidget_6);
        formDutyConst->setObjectName("formDutyConst");
        formDutyConst->setContentsMargins(0, 0, 0, 0);
        ValDuty = new QLabel(formLayoutWidget_6);
        ValDuty->setObjectName("ValDuty");

        formDutyConst->setWidget(0, QFormLayout::ItemRole::LabelRole, ValDuty);

        spnDutyVal = new QDoubleSpinBox(formLayoutWidget_6);
        spnDutyVal->setObjectName("spnDutyVal");
        QPalette palette14;
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette14.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette14.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette14.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyVal->setPalette(palette14);
        spnDutyVal->setMaximum(100.000000000000000);
        spnDutyVal->setSingleStep(0.100000000000000);
        spnDutyVal->setValue(10.000000000000000);

        formDutyConst->setWidget(0, QFormLayout::ItemRole::FieldRole, spnDutyVal);

        stkDutyParams->addWidget(pageDutyConst);
        pageDutyTriang = new QWidget();
        pageDutyTriang->setObjectName("pageDutyTriang");
        formLayoutWidget_7 = new QWidget(pageDutyTriang);
        formLayoutWidget_7->setObjectName("formLayoutWidget_7");
        formLayoutWidget_7->setGeometry(QRect(0, 20, 653, 123));
        formDutyTriang = new QFormLayout(formLayoutWidget_7);
        formDutyTriang->setObjectName("formDutyTriang");
        formDutyTriang->setContentsMargins(0, 0, 0, 0);
        MinDutyForTriang = new QLabel(formLayoutWidget_7);
        MinDutyForTriang->setObjectName("MinDutyForTriang");

        formDutyTriang->setWidget(0, QFormLayout::ItemRole::LabelRole, MinDutyForTriang);

        spnDutyMinForTriang = new QDoubleSpinBox(formLayoutWidget_7);
        spnDutyMinForTriang->setObjectName("spnDutyMinForTriang");
        QPalette palette15;
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette15.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette15.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette15.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMinForTriang->setPalette(palette15);
        spnDutyMinForTriang->setMaximum(100.000000000000000);
        spnDutyMinForTriang->setSingleStep(0.100000000000000);

        formDutyTriang->setWidget(0, QFormLayout::ItemRole::FieldRole, spnDutyMinForTriang);

        MaxDutyForTriang = new QLabel(formLayoutWidget_7);
        MaxDutyForTriang->setObjectName("MaxDutyForTriang");

        formDutyTriang->setWidget(1, QFormLayout::ItemRole::LabelRole, MaxDutyForTriang);

        spnDutyMaxForTriang = new QDoubleSpinBox(formLayoutWidget_7);
        spnDutyMaxForTriang->setObjectName("spnDutyMaxForTriang");
        QPalette palette16;
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette16.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette16.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette16.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMaxForTriang->setPalette(palette16);
        spnDutyMaxForTriang->setMaximum(100.000000000000000);
        spnDutyMaxForTriang->setSingleStep(0.100000000000000);
        spnDutyMaxForTriang->setValue(10.000000000000000);

        formDutyTriang->setWidget(1, QFormLayout::ItemRole::FieldRole, spnDutyMaxForTriang);

        StepDutyForTriang = new QLabel(formLayoutWidget_7);
        StepDutyForTriang->setObjectName("StepDutyForTriang");

        formDutyTriang->setWidget(2, QFormLayout::ItemRole::LabelRole, StepDutyForTriang);

        spnDutyStepForTriang = new QDoubleSpinBox(formLayoutWidget_7);
        spnDutyStepForTriang->setObjectName("spnDutyStepForTriang");
        QPalette palette17;
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette17.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette17.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette17.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyStepForTriang->setPalette(palette17);
        spnDutyStepForTriang->setDecimals(0);
        spnDutyStepForTriang->setMaximum(100.000000000000000);
        spnDutyStepForTriang->setSingleStep(10.000000000000000);
        spnDutyStepForTriang->setValue(1.000000000000000);

        formDutyTriang->setWidget(2, QFormLayout::ItemRole::FieldRole, spnDutyStepForTriang);

        StepDurationForTriang = new QLabel(formLayoutWidget_7);
        StepDurationForTriang->setObjectName("StepDurationForTriang");

        formDutyTriang->setWidget(3, QFormLayout::ItemRole::LabelRole, StepDurationForTriang);

        spnDutyStepDurationForTriang = new QDoubleSpinBox(formLayoutWidget_7);
        spnDutyStepDurationForTriang->setObjectName("spnDutyStepDurationForTriang");
        spnDutyStepDurationForTriang->setDecimals(1);
        spnDutyStepDurationForTriang->setMaximum(4000.000000000000000);
        spnDutyStepDurationForTriang->setSingleStep(0.100000000000000);
        spnDutyStepDurationForTriang->setValue(1000.000000000000000);

        formDutyTriang->setWidget(3, QFormLayout::ItemRole::FieldRole, spnDutyStepDurationForTriang);

        stkDutyParams->addWidget(pageDutyTriang);

        verticalLayout_2->addWidget(stkDutyParams);

        formDuty = new QFormLayout();
        formDuty->setObjectName("formDuty");
        MinDuty = new QLabel(tabDuty);
        MinDuty->setObjectName("MinDuty");

        formDuty->setWidget(0, QFormLayout::ItemRole::LabelRole, MinDuty);

        spnDutyMin = new QDoubleSpinBox(tabDuty);
        spnDutyMin->setObjectName("spnDutyMin");
        QPalette palette18;
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette18.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette18.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette18.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMin->setPalette(palette18);
        spnDutyMin->setMaximum(100.000000000000000);
        spnDutyMin->setSingleStep(0.100000000000000);

        formDuty->setWidget(0, QFormLayout::ItemRole::FieldRole, spnDutyMin);

        MaxDuty = new QLabel(tabDuty);
        MaxDuty->setObjectName("MaxDuty");

        formDuty->setWidget(1, QFormLayout::ItemRole::LabelRole, MaxDuty);

        spnDutyMax = new QDoubleSpinBox(tabDuty);
        spnDutyMax->setObjectName("spnDutyMax");
        QPalette palette19;
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette19.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette19.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette19.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyMax->setPalette(palette19);
        spnDutyMax->setMaximum(100.000000000000000);
        spnDutyMax->setSingleStep(0.100000000000000);
        spnDutyMax->setValue(10.000000000000000);

        formDuty->setWidget(1, QFormLayout::ItemRole::FieldRole, spnDutyMax);

        FreqDuty = new QLabel(tabDuty);
        FreqDuty->setObjectName("FreqDuty");

        formDuty->setWidget(2, QFormLayout::ItemRole::LabelRole, FreqDuty);

        spnDutyFreq = new QDoubleSpinBox(tabDuty);
        spnDutyFreq->setObjectName("spnDutyFreq");
        QPalette palette20;
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush6);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush11);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush12);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush6);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush6);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette20.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush);
#endif
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush6);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush11);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush12);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush6);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush6);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush13);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette20.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush);
#endif
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush11);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush11);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush12);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush11);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush11);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush14);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette20.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush);
#endif
        spnDutyFreq->setPalette(palette20);
        spnDutyFreq->setDecimals(0);
        spnDutyFreq->setMaximum(10000.000000000000000);
        spnDutyFreq->setSingleStep(0.500000000000000);
        spnDutyFreq->setValue(10.000000000000000);

        formDuty->setWidget(2, QFormLayout::ItemRole::FieldRole, spnDutyFreq);


        verticalLayout_2->addLayout(formDuty);

        tabTuning->addTab(tabDuty, QString());
        tabCurrent = new QWidget();
        tabCurrent->setObjectName("tabCurrent");
        formLayoutWidget_2 = new QWidget(tabCurrent);
        formLayoutWidget_2->setObjectName("formLayoutWidget_2");
        formLayoutWidget_2->setGeometry(QRect(10, 20, 211, 111));
        formCurrent = new QFormLayout(formLayoutWidget_2);
        formCurrent->setObjectName("formCurrent");
        formCurrent->setContentsMargins(0, 0, 0, 0);
        MinCur = new QLabel(formLayoutWidget_2);
        MinCur->setObjectName("MinCur");

        formCurrent->setWidget(0, QFormLayout::ItemRole::LabelRole, MinCur);

        spnCurMin = new QDoubleSpinBox(formLayoutWidget_2);
        spnCurMin->setObjectName("spnCurMin");
        spnCurMin->setDecimals(1);
        spnCurMin->setMinimum(-2000.000000000000000);
        spnCurMin->setMaximum(20000.000000000000000);
        spnCurMin->setSingleStep(0.100000000000000);
        spnCurMin->setValue(-2.000000000000000);

        formCurrent->setWidget(0, QFormLayout::ItemRole::FieldRole, spnCurMin);

        MaxCur = new QLabel(formLayoutWidget_2);
        MaxCur->setObjectName("MaxCur");

        formCurrent->setWidget(1, QFormLayout::ItemRole::LabelRole, MaxCur);

        spnCurMax = new QDoubleSpinBox(formLayoutWidget_2);
        spnCurMax->setObjectName("spnCurMax");
        spnCurMax->setDecimals(1);
        spnCurMax->setMinimum(-2000.000000000000000);
        spnCurMax->setMaximum(2000.000000000000000);
        spnCurMax->setSingleStep(0.100000000000000);
        spnCurMax->setValue(2.000000000000000);

        formCurrent->setWidget(1, QFormLayout::ItemRole::FieldRole, spnCurMax);

        FreqCur = new QLabel(formLayoutWidget_2);
        FreqCur->setObjectName("FreqCur");

        formCurrent->setWidget(2, QFormLayout::ItemRole::LabelRole, FreqCur);

        spnCurFreq = new QDoubleSpinBox(formLayoutWidget_2);
        spnCurFreq->setObjectName("spnCurFreq");
        spnCurFreq->setDecimals(0);
        spnCurFreq->setMaximum(10000.000000000000000);
        spnCurFreq->setSingleStep(0.500000000000000);
        spnCurFreq->setValue(10.000000000000000);

        formCurrent->setWidget(2, QFormLayout::ItemRole::FieldRole, spnCurFreq);

        tabTuning->addTab(tabCurrent, QString());
        tabPosition = new QWidget();
        tabPosition->setObjectName("tabPosition");
        formLayoutWidget_3 = new QWidget(tabPosition);
        formLayoutWidget_3->setObjectName("formLayoutWidget_3");
        formLayoutWidget_3->setGeometry(QRect(10, 20, 211, 111));
        formPosition = new QFormLayout(formLayoutWidget_3);
        formPosition->setObjectName("formPosition");
        formPosition->setContentsMargins(0, 0, 0, 0);
        spnPosMin = new QDoubleSpinBox(formLayoutWidget_3);
        spnPosMin->setObjectName("spnPosMin");
        spnPosMin->setDecimals(3);
        spnPosMin->setMinimum(-10.000000000000000);
        spnPosMin->setMaximum(10.000000000000000);
        spnPosMin->setSingleStep(0.100000000000000);
        spnPosMin->setValue(-1.000000000000000);

        formPosition->setWidget(0, QFormLayout::ItemRole::FieldRole, spnPosMin);

        spnPosMax = new QDoubleSpinBox(formLayoutWidget_3);
        spnPosMax->setObjectName("spnPosMax");
        spnPosMax->setDecimals(3);
        spnPosMax->setMinimum(-10.000000000000000);
        spnPosMax->setMaximum(10.000000000000000);
        spnPosMax->setSingleStep(0.100000000000000);
        spnPosMax->setValue(1.000000000000000);

        formPosition->setWidget(1, QFormLayout::ItemRole::FieldRole, spnPosMax);

        spnPosFreq = new QDoubleSpinBox(formLayoutWidget_3);
        spnPosFreq->setObjectName("spnPosFreq");
        spnPosFreq->setDecimals(0);
        spnPosFreq->setMaximum(10000.000000000000000);
        spnPosFreq->setValue(10.000000000000000);

        formPosition->setWidget(2, QFormLayout::ItemRole::FieldRole, spnPosFreq);

        MinPos = new QLabel(formLayoutWidget_3);
        MinPos->setObjectName("MinPos");

        formPosition->setWidget(0, QFormLayout::ItemRole::LabelRole, MinPos);

        MaxPos = new QLabel(formLayoutWidget_3);
        MaxPos->setObjectName("MaxPos");

        formPosition->setWidget(1, QFormLayout::ItemRole::LabelRole, MaxPos);

        FreqPos = new QLabel(formLayoutWidget_3);
        FreqPos->setObjectName("FreqPos");

        formPosition->setWidget(2, QFormLayout::ItemRole::LabelRole, FreqPos);

        tabTuning->addTab(tabPosition, QString());
        tabPid = new QWidget();
        tabPid->setObjectName("tabPid");
        formLayoutWidget_4 = new QWidget(tabPid);
        formLayoutWidget_4->setObjectName("formLayoutWidget_4");
        formLayoutWidget_4->setGeometry(QRect(10, 20, 211, 111));
        formPID = new QFormLayout(formLayoutWidget_4);
        formPID->setObjectName("formPID");
        formPID->setContentsMargins(0, 0, 0, 0);
        spnKp = new QDoubleSpinBox(formLayoutWidget_4);
        spnKp->setObjectName("spnKp");
        spnKp->setDecimals(3);
        spnKp->setMaximum(1023.000000000000000);
        spnKp->setSingleStep(0.010000000000000);

        formPID->setWidget(0, QFormLayout::ItemRole::FieldRole, spnKp);

        spnKi = new QDoubleSpinBox(formLayoutWidget_4);
        spnKi->setObjectName("spnKi");
        spnKi->setDecimals(3);
        spnKi->setMaximum(1023.000000000000000);
        spnKi->setSingleStep(0.010000000000000);

        formPID->setWidget(1, QFormLayout::ItemRole::FieldRole, spnKi);

        spnKd = new QDoubleSpinBox(formLayoutWidget_4);
        spnKd->setObjectName("spnKd");
        spnKd->setDecimals(3);
        spnKd->setMaximum(1023.000000000000000);
        spnKd->setSingleStep(0.010000000000000);

        formPID->setWidget(2, QFormLayout::ItemRole::FieldRole, spnKd);

        Kp = new QLabel(formLayoutWidget_4);
        Kp->setObjectName("Kp");

        formPID->setWidget(0, QFormLayout::ItemRole::LabelRole, Kp);

        Ki = new QLabel(formLayoutWidget_4);
        Ki->setObjectName("Ki");

        formPID->setWidget(1, QFormLayout::ItemRole::LabelRole, Ki);

        Kd = new QLabel(formLayoutWidget_4);
        Kd->setObjectName("Kd");

        formPID->setWidget(2, QFormLayout::ItemRole::LabelRole, Kd);

        tabTuning->addTab(tabPid, QString());

        horizontalLayout->addWidget(tabTuning);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");

        gridLayout_2->addLayout(horizontalLayout_3, 1, 0, 1, 1);


        verticalLayoutMain->addWidget(grpTuning);


        formLayout->setLayout(0, QFormLayout::ItemRole::SpanningRole, verticalLayoutMain);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 733, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabTuning->setCurrentIndex(0);
        stkDutyParams->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        grpSerial->setTitle(QString());
        lblConnText->setText(QCoreApplication::translate("MainWindow", "Disconnected", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        lblLed->setText(QString());
        label_3->setText(QCoreApplication::translate("MainWindow", "Connection State:", nullptr));
        btnConnect->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        label_usb_img->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "Baud:", nullptr));
        cmbBaud->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        cmbBaud->setItemText(1, QCoreApplication::translate("MainWindow", "115200", nullptr));

        label->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        grpTuning->setTitle(QString());
        chkLive->setText(QCoreApplication::translate("MainWindow", "Send live", nullptr));
        btnApply->setText(QCoreApplication::translate("MainWindow", "Apply settings", nullptr));
        TriggerFunction->setText(QCoreApplication::translate("MainWindow", "Target:", nullptr));
        cmbDutyType->setItemText(0, QCoreApplication::translate("MainWindow", "Sin", nullptr));
        cmbDutyType->setItemText(1, QCoreApplication::translate("MainWindow", "Constant", nullptr));
        cmbDutyType->setItemText(2, QCoreApplication::translate("MainWindow", "StepTriangle", nullptr));

        MinDutyForSine->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        spnDutyMinForSine->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        MaxDutyForSine->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        spnDutyMaxForSine->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        FreqDutyForSine->setText(QCoreApplication::translate("MainWindow", "Frequency", nullptr));
        spnDutyFreqForSine->setSuffix(QCoreApplication::translate("MainWindow", " Hz", nullptr));
        ValDuty->setText(QCoreApplication::translate("MainWindow", "Value:", nullptr));
        spnDutyVal->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        MinDutyForTriang->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        spnDutyMinForTriang->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        MaxDutyForTriang->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        spnDutyMaxForTriang->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        StepDutyForTriang->setText(QCoreApplication::translate("MainWindow", "Step:", nullptr));
        spnDutyStepForTriang->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        StepDurationForTriang->setText(QCoreApplication::translate("MainWindow", "Step Duration:", nullptr));
        spnDutyStepDurationForTriang->setSuffix(QCoreApplication::translate("MainWindow", " ms", nullptr));
        MinDuty->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        spnDutyMin->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        MaxDuty->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        spnDutyMax->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        FreqDuty->setText(QCoreApplication::translate("MainWindow", "Frequency", nullptr));
        spnDutyFreq->setSuffix(QCoreApplication::translate("MainWindow", " Hz", nullptr));
        tabTuning->setTabText(tabTuning->indexOf(tabDuty), QCoreApplication::translate("MainWindow", "Duty", nullptr));
        MinCur->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        spnCurMin->setSuffix(QCoreApplication::translate("MainWindow", " mA", nullptr));
        MaxCur->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        spnCurMax->setSuffix(QCoreApplication::translate("MainWindow", " mA", nullptr));
        FreqCur->setText(QCoreApplication::translate("MainWindow", "Frequency:", nullptr));
        spnCurFreq->setSuffix(QCoreApplication::translate("MainWindow", " Hz", nullptr));
        tabTuning->setTabText(tabTuning->indexOf(tabCurrent), QCoreApplication::translate("MainWindow", "Current", nullptr));
        spnPosMin->setSuffix(QCoreApplication::translate("MainWindow", " \302\260", nullptr));
        spnPosMax->setSuffix(QCoreApplication::translate("MainWindow", " \302\260", nullptr));
        spnPosFreq->setSuffix(QCoreApplication::translate("MainWindow", " Hz", nullptr));
        MinPos->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        MaxPos->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        FreqPos->setText(QCoreApplication::translate("MainWindow", "Frequency:", nullptr));
        tabTuning->setTabText(tabTuning->indexOf(tabPosition), QCoreApplication::translate("MainWindow", "Position", nullptr));
        Kp->setText(QCoreApplication::translate("MainWindow", "Kp:", nullptr));
        Ki->setText(QCoreApplication::translate("MainWindow", "Ki:", nullptr));
        Kd->setText(QCoreApplication::translate("MainWindow", "Kd:", nullptr));
        tabTuning->setTabText(tabTuning->indexOf(tabPid), QCoreApplication::translate("MainWindow", "PID", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
