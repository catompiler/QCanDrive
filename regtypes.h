#ifndef REGTYPES_H
#define REGTYPES_H

#include <stdint.h>
#include "first_arg_type.hpp"
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
    NONE              = 0x0,
    RL_HIDE           = 0x1,
    CO_HIDE           = 0x2,
    CO_COUNT          = 0x4,
    RESERVED          = 0x8,
    CON_CNT_NMT       = 0x10,
    CON_CNT_EM        = 0x20,
    CON_CNT_SYNC      = 0x40,
    CON_CNT_SYNC_PROD = 0x80,
    CON_CNT_STORAGE   = 0x100,
    CON_CNT_TIME      = 0x200,
    CON_CNT_EM_PROD   = 0x400,
    CON_CNT_HB_CONS   = 0x800,
    CON_CNT_HB_PROD   = 0x1000,
    CON_CNT_SDO_SRV   = 0x2000,
    CON_CNT_SDO_CLI   = 0x4000,
    CON_CNT_RPDO      = 0x8000,
    CON_CNT_TPDO      = 0x10000,
    CO_SDO_R          = 0x20000,
    CO_SDO_W          = 0x40000,
    CO_TPDO           = 0x80000,
    CO_RPDO           = 0x100000,
    CO_TSRDO          = 0x200000,
    CO_RSRDO          = 0x400000,
};
}
// Тип дополнительных флагов.
typedef uint32_t reg_eflags_t;

// Аттрибуты CO.
namespace COAttribute {
enum Value{
    NONE    = 0x0,
    SDO_R   = 0x1,
    SDO_W   = 0x2,
    SDO_RW  = 0x4,

    TPDO    = 0x8,
    RPDO    = 0x10,
    TRPDO   = 0x20,

    TSRDO   = 0x40,
    RSRDO   = 0x80,
    TRSRDO  = 0x100,

    MB      = 0x200,
    STR     = 0x400
};
}
// Тип аттрибутов CO.
typedef uint32_t co_attributes_t;

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

//! Получает имя аттрибута.
extern QString coAttributeName(COAttribute::Value attr);

//! Получает список имён аттрибутов.
extern QStringList coAttributeNames();

//! Получает имя аттрибута.
extern QString coAttributeFullName(COAttribute::Value attr);

//! Получает список имён аттрибутов.
extern QStringList coAttributeFullNames();

//! Получает атрибуты по расширенным флагам.
extern co_attributes_t eflagsToCoAttributes(reg_eflags_t eflags);

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

//! Получает С тип данных регистра.
QString varRegDataTypeStr(DataType type);

//! Получает С тип данных.
QString varDataTypeStr(DataType type);

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


template <typename T, typename GetNameType>
QStringList getNames(T value, GetNameType getNameStr){

    using NameType = typename first_arg_type<GetNameType>::value;

    auto flags = static_cast<unsigned int>(value);
    unsigned int flag = 0x1;

    QString flag_str;
    QStringList flag_str_list;

    while(flags){
        if(flags & 0x1){
            flag_str = getNameStr(static_cast<NameType>(flag));

            if(flag_str.isEmpty()){
                flag_str = QStringLiteral("0x%1").arg(flag, 0, 16);
            }

            flag_str_list.append(flag_str);
        }
        flag <<= 1;
        flags >>= 1;
    }

    if(flag_str_list.isEmpty()){
        flag_str_list.append(getNameStr(NameType()));
    }

    return flag_str_list;
}

}

#endif // REGTYPES_H
