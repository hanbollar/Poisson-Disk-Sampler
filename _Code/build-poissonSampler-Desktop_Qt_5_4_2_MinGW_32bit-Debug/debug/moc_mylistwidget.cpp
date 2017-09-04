/****************************************************************************
** Meta object code from reading C++ file 'mylistwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../PoissonSampler/src/mylistwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mylistwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MyListWidget_t {
    QByteArrayData data[7];
    char stringdata[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyListWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyListWidget_t qt_meta_stringdata_MyListWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "MyListWidget"
QT_MOC_LITERAL(1, 13, 16), // "slot_addHalfEdge"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 48, 14), // "slot_addVertex"
QT_MOC_LITERAL(5, 63, 12), // "slot_addFace"
QT_MOC_LITERAL(6, 76, 15) // "slot_selectItem"

    },
    "MyListWidget\0slot_addHalfEdge\0\0"
    "QListWidgetItem*\0slot_addVertex\0"
    "slot_addFace\0slot_selectItem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyListWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    1,   37,    2, 0x0a /* Public */,
       5,    1,   40,    2, 0x0a /* Public */,
       6,    1,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void MyListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MyListWidget *_t = static_cast<MyListWidget *>(_o);
        switch (_id) {
        case 0: _t->slot_addHalfEdge((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: _t->slot_addVertex((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: _t->slot_addFace((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->slot_selectItem((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MyListWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_MyListWidget.data,
      qt_meta_data_MyListWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MyListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MyListWidget.stringdata))
        return static_cast<void*>(const_cast< MyListWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int MyListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
