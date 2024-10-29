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
    I32 = 0,
    I16 = 1,
    I8 = 2,
    U32 = 3,
    U16 = 4,
    U8 = 5,
    IQ24 = 6,
    IQ15 = 7,
    IQ7 = 8,
};

// Основные флаги.
namespace RegFlag {
enum Value {
    NONE = 0,
    CONF = 1,
    READONLY = 2,
};
}
// Тип основных флагов.
typedef uint32_t reg_flags_t;

// Дополнительные флаги.
namespace RegEFlag {
enum Value {
    NONE = 0,
    RL_HIDE = 1,
    CO_HIDE = 2,
    CO_COUNT = 4,
    CON_NMT = 8,
};
}
// Тип дополнительных флагов.
typedef uint32_t reg_eflags_t;

namespace RegTypes {

//! Получает все флаги.
extern QVector<RegFlag::Value> flags();

//! Получает имя флага.
extern QString flagName(RegFlag::Value flag);

//! Получает список имён флагов.
extern QStringList flagsNames();

//! Получает имя флага.
extern QString flagFullName(RegFlag::Value flag);

//! Получает список имён флагов.
extern QStringList flagsFullNames();

//! Получает все флаги.
extern QVector<RegEFlag::Value> eflags();

//! Получает имя флага.
extern QString eflagName(RegEFlag::Value eflag);

//! Получает список имён флагов.
extern QStringList eflagsNames();

//! Получает имя флага.
extern QString eflagFullName(RegEFlag::Value eflag);

//! Получает список имён флагов.
extern QStringList eflagsFullNames();

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

//! Получает строковые представления типов данных.
extern QStringList dataTypesStringList();

//! Получает обозначение истинного значения.
extern QString trueStr();

//! Получает обозначение ложного значения.
extern QString falseStr();

//! Получает обозначение булева значения.
extern QString boolStr(bool value);

//! Получает список строковых представлений булевых значений.
extern QStringList boolStringList();

// Проверки типа данных.
extern int sizeBytes(DataType type);
extern bool isBoolean(DataType type);
extern bool isString(DataType type);
extern bool isNumeric(DataType type);
extern bool isInteger(DataType type);
extern bool isSigned(DataType type);
extern bool isUnsigned(DataType type);
extern bool isFractional(DataType type);

}

#endif // REGTYPES_H
