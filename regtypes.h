#ifndef REGTYPES_H
#define REGTYPES_H

#include <stdint.h>
#include <QString>
#include <QVector>

//! Тип индекса.
typedef uint16_t reg_index_t;

//! Тип подындекса.
typedef uint8_t reg_subindex_t;


//! Тип объекта.
enum class ObjectType {
    VAR = 0,
    ARR = 1,
    REC = 2
};

// Тип данных.
enum class DataType {
    BOOLEAN = 0,
    INTEGER8 = 1,
    INTEGER16 = 2,
    INTEGER32 = 3,
    INTEGER64 = 4,
    UNSIGNED8 = 5,
    UNSIGNED16 = 6,
    UNSIGNED32 = 7,
    UNSIGNED64 = 8,
    REAL32 = 9,
    REAL64 = 10,
    VISIBLE_STRING = 11,
    OCTET_STRING = 12,
    UNICODE_STRING = 13,
    DOMAIN = 14
};

namespace RegTypes {

//! Получает все типы объектов.
extern QVector<ObjectType> objectTypes();

//! Получает все типы данных.
extern QVector<DataType> dataTypes();

//! Получает строковое представление типа.
extern QString typeStr(ObjectType type);

//! Получает строковое представление типа данных.
extern QString dataTypeStr(DataType type);

}

#endif // REGTYPES_H
