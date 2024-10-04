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

}
