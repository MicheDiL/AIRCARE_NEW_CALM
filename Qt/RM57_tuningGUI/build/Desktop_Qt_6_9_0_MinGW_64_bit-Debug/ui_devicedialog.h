/********************************************************************************
** Form generated from reading UI file 'devicedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICEDIALOG_H
#define UI_DEVICEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_DeviceDialog
{
public:
    QVBoxLayout *vboxMain;
    QGridLayout *gridMain;
    QGroupBox *grpPosition;
    QVBoxLayout *vboxPosition;
    QCustomPlot *plotPosition;
    QGroupBox *grpVoltages;
    QVBoxLayout *vboxVoltages;
    QCustomPlot *plotVoltages;
    QGroupBox *grpDuty;
    QVBoxLayout *vboxDuty;
    QCustomPlot *plotDuty;
    QGroupBox *grpCurrents;
    QVBoxLayout *vboxCurrents;
    QCustomPlot *plotCurrents;

    void setupUi(QDialog *DeviceDialog)
    {
        if (DeviceDialog->objectName().isEmpty())
            DeviceDialog->setObjectName("DeviceDialog");
        DeviceDialog->resize(847, 628);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush1);
        QBrush brush2(QColor(127, 127, 127, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush1);
        QBrush brush4(QColor(255, 255, 220, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush4);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush5(QColor(0, 0, 0, 127));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush5);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush1);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush5);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush1);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush);
        QBrush brush6(QColor(127, 127, 127, 127));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush6);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush1);
#endif
        DeviceDialog->setPalette(palette);
        vboxMain = new QVBoxLayout(DeviceDialog);
        vboxMain->setObjectName("vboxMain");
        gridMain = new QGridLayout();
        gridMain->setSpacing(10);
        gridMain->setObjectName("gridMain");
        gridMain->setContentsMargins(10, 10, 10, 10);
        grpPosition = new QGroupBox(DeviceDialog);
        grpPosition->setObjectName("grpPosition");
        vboxPosition = new QVBoxLayout(grpPosition);
        vboxPosition->setObjectName("vboxPosition");
        plotPosition = new QCustomPlot(grpPosition);
        plotPosition->setObjectName("plotPosition");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plotPosition->sizePolicy().hasHeightForWidth());
        plotPosition->setSizePolicy(sizePolicy);
        plotPosition->setMinimumSize(QSize(0, 200));

        vboxPosition->addWidget(plotPosition);


        gridMain->addWidget(grpPosition, 1, 1, 1, 1);

        grpVoltages = new QGroupBox(DeviceDialog);
        grpVoltages->setObjectName("grpVoltages");
        vboxVoltages = new QVBoxLayout(grpVoltages);
        vboxVoltages->setObjectName("vboxVoltages");
        plotVoltages = new QCustomPlot(grpVoltages);
        plotVoltages->setObjectName("plotVoltages");
        sizePolicy.setHeightForWidth(plotVoltages->sizePolicy().hasHeightForWidth());
        plotVoltages->setSizePolicy(sizePolicy);
        plotVoltages->setMinimumSize(QSize(0, 200));

        vboxVoltages->addWidget(plotVoltages);


        gridMain->addWidget(grpVoltages, 1, 0, 1, 1);

        grpDuty = new QGroupBox(DeviceDialog);
        grpDuty->setObjectName("grpDuty");
        vboxDuty = new QVBoxLayout(grpDuty);
        vboxDuty->setObjectName("vboxDuty");
        plotDuty = new QCustomPlot(grpDuty);
        plotDuty->setObjectName("plotDuty");
        sizePolicy.setHeightForWidth(plotDuty->sizePolicy().hasHeightForWidth());
        plotDuty->setSizePolicy(sizePolicy);
        plotDuty->setMinimumSize(QSize(0, 200));

        vboxDuty->addWidget(plotDuty);


        gridMain->addWidget(grpDuty, 0, 0, 1, 1);

        grpCurrents = new QGroupBox(DeviceDialog);
        grpCurrents->setObjectName("grpCurrents");
        vboxCurrents = new QVBoxLayout(grpCurrents);
        vboxCurrents->setObjectName("vboxCurrents");
        plotCurrents = new QCustomPlot(grpCurrents);
        plotCurrents->setObjectName("plotCurrents");
        sizePolicy.setHeightForWidth(plotCurrents->sizePolicy().hasHeightForWidth());
        plotCurrents->setSizePolicy(sizePolicy);
        plotCurrents->setMinimumSize(QSize(0, 200));

        vboxCurrents->addWidget(plotCurrents);


        gridMain->addWidget(grpCurrents, 0, 1, 1, 1);


        vboxMain->addLayout(gridMain);


        retranslateUi(DeviceDialog);

        QMetaObject::connectSlotsByName(DeviceDialog);
    } // setupUi

    void retranslateUi(QDialog *DeviceDialog)
    {
        DeviceDialog->setWindowTitle(QCoreApplication::translate("DeviceDialog", "Dialog", nullptr));
        grpPosition->setTitle(QCoreApplication::translate("DeviceDialog", "POSITIONS", nullptr));
        grpVoltages->setTitle(QCoreApplication::translate("DeviceDialog", "VOLTAGES", nullptr));
        grpDuty->setTitle(QCoreApplication::translate("DeviceDialog", "DUTY CYCLE", nullptr));
        grpCurrents->setTitle(QCoreApplication::translate("DeviceDialog", "CURRENTS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeviceDialog: public Ui_DeviceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICEDIALOG_H
