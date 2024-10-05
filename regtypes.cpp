#include "regtypes.h"

namespace RegTypes {

QString typeStr(ObjectType type)
{
    static const char* strs[] = {
        "VAR",
        "ARR",
        "REC"
    };

    if(static_cast<decltype(sizeof(strs))>(type) >= (sizeof(strs)/sizeof(strs[0]))){
        return QString();
    }

    return QString(strs[static_cast<int>(type)]);
}

QString dataTypeStr(DataType type)
{
    static const char* strs[] = {
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

    if(static_cast<decltype(sizeof(strs))>(type) >= (sizeof(strs)/sizeof(strs[0]))){
        return QString();
    }

    return QString(strs[static_cast<int>(type)]);
}

QVector<ObjectType> objectTypes()
{
    // std::initializer_list
    static const auto obj_types = {
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
    static const auto data_types = {
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

}
