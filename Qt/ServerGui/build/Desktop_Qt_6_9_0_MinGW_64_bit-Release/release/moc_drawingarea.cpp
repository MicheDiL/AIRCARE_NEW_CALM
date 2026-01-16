/****************************************************************************
** Meta object code from reading C++ file 'drawingarea.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../drawingarea.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drawingarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11DrawingAreaE_t {};
} // unnamed namespace

template <> constexpr inline auto DrawingArea::qt_create_metaobjectdata<qt_meta_tag_ZN11DrawingAreaE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DrawingArea",
        "stateChanged",
        "",
        "DrawingState",
        "newState",
        "penPositionUpdated",
        "position",
        "sendFullTrajectory",
        "QList<QPointF>",
        "trajectory",
        "trajDuration",
        "scaleFactorChanged",
        "newScaleFactor",
        "calibrationDebugMessage",
        "msg",
        "calibrationDone",
        "pixelsPerMm",
        "strokeLengthUpdated",
        "mm"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'stateChanged'
        QtMocHelpers::SignalData<void(DrawingState)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'penPositionUpdated'
        QtMocHelpers::SignalData<void(QPointF)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QPointF, 6 },
        }}),
        // Signal 'sendFullTrajectory'
        QtMocHelpers::SignalData<void(const QList<QPointF> &, int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 }, { QMetaType::Int, 10 },
        }}),
        // Signal 'scaleFactorChanged'
        QtMocHelpers::SignalData<void(float)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 12 },
        }}),
        // Signal 'calibrationDebugMessage'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'calibrationDone'
        QtMocHelpers::SignalData<void(float)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 16 },
        }}),
        // Signal 'strokeLengthUpdated'
        QtMocHelpers::SignalData<void(float)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DrawingArea, qt_meta_tag_ZN11DrawingAreaE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DrawingArea::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DrawingAreaE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DrawingAreaE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11DrawingAreaE_t>.metaTypes,
    nullptr
} };

void DrawingArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DrawingArea *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< std::add_pointer_t<DrawingState>>(_a[1]))); break;
        case 1: _t->penPositionUpdated((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 2: _t->sendFullTrajectory((*reinterpret_cast< std::add_pointer_t<QList<QPointF>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->scaleFactorChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 4: _t->calibrationDebugMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->calibrationDone((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 6: _t->strokeLengthUpdated((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QPointF> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(DrawingState )>(_a, &DrawingArea::stateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(QPointF )>(_a, &DrawingArea::penPositionUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(const QList<QPointF> & , int )>(_a, &DrawingArea::sendFullTrajectory, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(float )>(_a, &DrawingArea::scaleFactorChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(const QString & )>(_a, &DrawingArea::calibrationDebugMessage, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(float )>(_a, &DrawingArea::calibrationDone, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DrawingArea::*)(float )>(_a, &DrawingArea::strokeLengthUpdated, 6))
            return;
    }
}

const QMetaObject *DrawingArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DrawingArea::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DrawingAreaE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DrawingArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DrawingArea::stateChanged(DrawingState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DrawingArea::penPositionUpdated(QPointF _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DrawingArea::sendFullTrajectory(const QList<QPointF> & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void DrawingArea::scaleFactorChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DrawingArea::calibrationDebugMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DrawingArea::calibrationDone(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void DrawingArea::strokeLengthUpdated(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
