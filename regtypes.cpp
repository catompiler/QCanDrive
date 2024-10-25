#include "regtypes.h"
#include <QObject>

namespace RegTypes {

static const char* obj_type_name_strs[] = {
    "VAR",
    "ARR",
    "REC"
};

static decltype(sizeof(obj_type_name_strs)) obj_type_name_strs_count = (sizeof(obj_type_name_strs)/sizeof(obj_type_name_strs[0]));

static const char* data_type_name_strs[] = {
    "BOOLEAN",
    "INTEGER8",
    "INTEGER16",
    "INTEGER32",
    "INTEGER64",
    "UNSIGNED8",
    "UNSIGNED16",
    "UNSIGNED32",
    "UNSIGNED64",
    "REAL32",
    "REAL64",
    "VISIBLE_STRING",
    "OCTET_STRING",
    "UNICODE_STRING",
    "DOMAIN"
};

static decltype(sizeof(data_type_name_strs)) data_type_name_strs_count = (sizeof(data_type_name_strs)/sizeof(data_type_name_strs[0]));


static const char* flag_name_strs[] = {
    "REG_FLAG_NONE",
    "REG_FLAG_CONF",
    "REG_FLAG_RO",
};

static decltype(sizeof(flag_name_strs)) flag_name_strs_count = (sizeof(flag_name_strs)/sizeof(flag_name_strs[0]));


static const char* eflag_name_strs[] = {
    "REG_EFLAG_NONE",
};

static decltype(sizeof(eflag_name_strs)) eflag_name_strs_count = (sizeof(eflag_name_strs)/sizeof(eflag_name_strs[0]));



QVector<RegFlag> flags()
{
    static auto flags = {
        RegFlag::NONE,
        RegFlag::CONF,
        RegFlag::RO
    };
    static const QVector<RegFlag> flags_vec(flags);

    return flags_vec;
}

QString flagName(RegFlag flag)
{
    if(static_cast<decltype(sizeof(flag_name_strs))>(flag) >= flag_name_strs_count){
        return QString();
    }

    return QString(flag_name_strs[static_cast<int>(flag)]);
}

QStringList flagsNames()
{
    static const QStringList flag_names_list = QStringList(flag_name_strs, flag_name_strs + flag_name_strs_count);

    return flag_names_list;
}

QVector<RegEFlag> eflags()
{
    static auto eflags = {
        RegEFlag::NONE,
    };
    static const QVector<RegEFlag> eflags_vec(eflags);

    return eflags_vec;
}

QString eflagName(RegEFlag eflag)
{
    if(static_cast<decltype(sizeof(eflag_name_strs))>(eflag) >= eflag_name_strs_count){
        return QString();
    }

    return QString(eflag_name_strs[static_cast<int>(eflag)]);
}

QStringList eflagsNames()
{
    static const QStringList eflag_names_list = QStringList(eflag_name_strs, eflag_name_strs + eflag_name_strs_count);

    return eflag_names_list;
}


QVector<ObjectType> objectTypes()
{
    // std::initializer_list
    static auto obj_types = {
        ObjectType::VAR,
        ObjectType::ARR,
        ObjectType::REC
    };
    static const QVector<ObjectType> obj_types_vec(obj_types);

    return obj_types_vec;
}

QVector<DataType> dataTypes()
{
    // std::initializer_list
    static auto data_types = {
        DataType::BOOLEAN,
        DataType::INTEGER8,
        DataType::INTEGER16,
        DataType::INTEGER32,
        DataType::INTEGER64,
        DataType::UNSIGNED8,
        DataType::UNSIGNED16,
        DataType::UNSIGNED32,
        DataType::UNSIGNED64,
        DataType::REAL32,
        DataType::REAL64,
        DataType::VISIBLE_STRING,
        DataType::OCTET_STRING,
        DataType::UNICODE_STRING,
        DataType::DOMAIN
    };
    static const QVector<DataType> data_types_vec(data_types);

    return data_types_vec;
}

QString typeStr(ObjectType type)
{
    if(static_cast<decltype(sizeof(obj_type_name_strs))>(type) >= obj_type_name_strs_count){
        return QString();
    }

    return QString(obj_type_name_strs[static_cast<int>(type)]);
}

QString dataTypeStr(DataType type)
{
    if(static_cast<decltype(sizeof(data_type_name_strs))>(type) >= data_type_name_strs_count){
        return QString();
    }

    return QString(data_type_name_strs[static_cast<int>(type)]);
}

QStringList objectTypesStringList()
{
    static const QStringList obj_type_str_list = QStringList(obj_type_name_strs, obj_type_name_strs + obj_type_name_strs_count);

    return obj_type_str_list;
}

QStringList dataTypesStringList()
{
    static const QStringList data_type_str_list = QStringList(data_type_name_strs, data_type_name_strs + data_type_name_strs_count);

    return data_type_str_list;
}

QString trueStr()
{
    static const QString true_str = QObject::tr("Да");

    return true_str;
}

QString falseStr()
{
    static const QString false_str = QObject::tr("Нет");

    return false_str;
}

QString boolStr(bool value)
{
    return value ? trueStr() : falseStr();
}

QStringList boolStringList()
{
    static const QStringList bool_list = QStringList({falseStr(), trueStr()});

    return bool_list;
}

bool isBoolean(DataType type)
{
    return type == DataType::BOOLEAN;
}

bool isString(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
    case DataType::REAL32:
    case DataType::REAL64:
        break;
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
        return true;
    case DataType::DOMAIN:
        break;
    }
    return false;
}

bool isNumeric(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
        break;
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
    case DataType::REAL32:
    case DataType::REAL64:
        return true;
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
        break;
    }
    return false;
}

bool isInteger(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
        break;
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
        return true;
    case DataType::REAL32:
    case DataType::REAL64:
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
        break;
    }
    return false;
}

bool isSigned(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
        break;
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
        return true;
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
        break;
    case DataType::REAL32:
    case DataType::REAL64:
        return true;
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
        break;
    }
    return false;
}

bool isUnsigned(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
        break;
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
        return true;
    case DataType::REAL32:
    case DataType::REAL64:
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
        break;
    }
    return false;
}

bool isFloat(DataType type)
{
    switch(type){
    case DataType::BOOLEAN:
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
        break;
    case DataType::REAL32:
    case DataType::REAL64:
        return true;
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
        break;
    }
    return false;
}

}
