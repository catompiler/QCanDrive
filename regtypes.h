#ifndef REGTYPES_H
#define REGTYPES_H

#include <stdint.h>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QPair>

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

// Основные флаги.
enum class RegFlags {
    NONE = 0,
    PARAMETER = 1,
};
// Тип основных флагов.
typedef uint32_t reg_flags_t;

// Дополнительные флаги.
enum class RegEFlags {
    NONE = 0,
};
// Тип дополнительных флагов.
typedef uint32_t reg_eflags_t;

namespace RegTypes {

//! Получает все типы объектов.
extern QVector<ObjectType> objectTypes();

//! Получает все типы данных.
extern QVector<DataType> dataTypes();

//! Получает строковое представление типа.
extern QString typeStr(ObjectType type);

//! Получает строковое представление типа данных.
extern QString dataTypeStr(DataType type);

//! Получает строковые представления типов.
extern QStringList objectTypesStringList();

//! Получает обозначение истинного значения.
extern QString trueStr();

//! Получает обозначение ложного значения.
extern QString falseStr();

//! Получает обозначение булева значения.
extern QString boolStr(bool value);

//! Получает список строковых представлений булевых значений.
extern QStringList boolStringList();

// Проверки типа данных.
extern bool isBoolean(DataType type);
extern bool isString(DataType type);
extern bool isNumeric(DataType type);
extern bool isInteger(DataType type);
extern bool isSigned(DataType type);
extern bool isUnsigned(DataType type);
extern bool isFloat(DataType type);

}

#endif // REGTYPES_H
