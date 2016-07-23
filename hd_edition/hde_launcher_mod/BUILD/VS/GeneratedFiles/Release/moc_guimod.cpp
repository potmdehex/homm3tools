/****************************************************************************
** Meta object code from reading C++ file 'guimod.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../guimod.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guimod.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GuiMod_t {
    QByteArrayData data[12];
    char stringdata[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_GuiMod_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_GuiMod_t qt_meta_stringdata_GuiMod = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 9),
QT_MOC_LITERAL(2, 17, 0),
QT_MOC_LITERAL(3, 18, 14),
QT_MOC_LITERAL(4, 33, 17),
QT_MOC_LITERAL(5, 51, 16),
QT_MOC_LITERAL(6, 68, 16),
QT_MOC_LITERAL(7, 85, 18),
QT_MOC_LITERAL(8, 104, 15),
QT_MOC_LITERAL(9, 120, 6),
QT_MOC_LITERAL(10, 127, 20),
QT_MOC_LITERAL(11, 148, 4)
    },
    "GuiMod\0extendGui\0\0playBtnClicked\0"
    "creditsBtnClicked\0donateBtnClicked\0"
    "browseBtnClicked\0optionLabelClicked\0"
    "rmgCheckToggled\0toggle\0hotkeysLinkActivated\0"
    "link\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GuiMod[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a,
       3,    0,   55,    2, 0x0a,
       4,    0,   56,    2, 0x0a,
       5,    0,   57,    2, 0x0a,
       6,    0,   58,    2, 0x0a,
       7,    0,   59,    2, 0x0a,
       8,    1,   60,    2, 0x0a,
      10,    1,   63,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

void GuiMod::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GuiMod *_t = static_cast<GuiMod *>(_o);
        switch (_id) {
        case 0: _t->extendGui(); break;
        case 1: _t->playBtnClicked(); break;
        case 2: _t->creditsBtnClicked(); break;
        case 3: _t->donateBtnClicked(); break;
        case 4: _t->browseBtnClicked(); break;
        case 5: _t->optionLabelClicked(); break;
        case 6: _t->rmgCheckToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->hotkeysLinkActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject GuiMod::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GuiMod.data,
      qt_meta_data_GuiMod,  qt_static_metacall, 0, 0}
};


const QMetaObject *GuiMod::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GuiMod::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiMod.stringdata))
        return static_cast<void*>(const_cast< GuiMod*>(this));
    return QObject::qt_metacast(_clname);
}

int GuiMod::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
