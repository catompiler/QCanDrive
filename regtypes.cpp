#include "regtypes.h"
#include <QObject>
#include <QtAlgorithms>

namespace RegTypes {

static const char* obj_type_name_strs[] = {
    "VAR",
    "ARR",
    "REC"
};

static decltype(sizeof(obj_type_name_strs)) obj_type_name_strs_count = (sizeof(obj_type_name_strs)/sizeof(obj_type_name_strs[0]));

static const char* data_type_name_strs[] = {
    "I32",
    "I16",
    "I8",
    "U32",
    "U16",
    "U8",
    "IQ24",
    "IQ15",
    "IQ7",
};

static decltype(sizeof(data_type_name_strs)) data_type_name_strs_count = (sizeof(data_type_name_strs)/sizeof(data_type_name_strs[0]));


static const char* flag_name_strs[] = {
    "NONE",
    "CONF",
    "READONLY",
};

static decltype(sizeof(flag_name_strs)) flag_name_strs_count = (sizeof(flag_name_strs)/sizeof(flag_name_strs[0]));


static const char* flag_full_name_strs[] = {
    "REG_FLAG_NONE",
    "REG_FLAG_CONF",
    "REG_FLAG_READONLY",
};

static decltype(sizeof(flag_full_name_strs)) flag_full_name_strs_count = (sizeof(flag_full_name_strs)/sizeof(flag_full_name_strs[0]));


static const char* eflag_name_strs[] = {
    "NONE",
    "RL_HIDE",
    "CO_HIDE",
    "CO_COUNT",
    "RESERVED",
    "CON_CNT_NMT",
    "CON_CNT_EM",
    "CON_CNT_SYNC",
    "CON_CNT_SYNC_PROD",
    "CON_CNT_STORAGE",
    "CON_CNT_TIME",
    "CON_CNT_EM_PROD",
    "CON_CNT_HB_CONS",
    "CON_CNT_HB_PROD",
    "CON_CNT_SDO_SRV",
    "CON_CNT_SDO_CLI",
    "CON_CNT_RPDO",
    "CON_CNT_TPDO",
    "CO_SDO_R",
    "CO_SDO_W",
    "CO_TPDO",
    "CO_RPDO",
    "CO_TSRDO",
    "CO_RSRDO",
};

static decltype(sizeof(eflag_name_strs)) eflag_name_strs_count = (sizeof(eflag_name_strs)/sizeof(eflag_name_strs[0]));


static const char* eflag_full_name_strs[] = {
    "REG_EFLAG_NONE",
    "REG_EFLAG_RL_HIDE",
    "REG_EFLAG_CO_HIDE",
    "REG_EFLAG_CO_COUNT",
    "REG_EFLAG_RESERVED",
    "REG_EFLAG_CON_CNT_NMT",
    "REG_EFLAG_CON_CNT_EM",
    "REG_EFLAG_CON_CNT_SYNC",
    "REG_EFLAG_CON_CNT_SYNC_PROD",
    "REG_EFLAG_CON_CNT_STORAGE",
    "REG_EFLAG_CON_CNT_TIME",
    "REG_EFLAG_CON_CNT_EM_PROD",
    "REG_EFLAG_CON_CNT_HB_CONS",
    "REG_EFLAG_CON_CNT_HB_PROD",
    "REG_EFLAG_CON_CNT_SDO_SRV",
    "REG_EFLAG_CON_CNT_SDO_CLI",
    "REG_EFLAG_CON_CNT_RPDO",
    "REG_EFLAG_CON_CNT_TPDO",
    "REG_EFLAG_CO_SDO_R",
    "REG_EFLAG_CO_SDO_W",
    "REG_EFLAG_CO_TPDO",
    "REG_EFLAG_CO_RPDO",
    "REG_EFLAG_CO_TSRDO",
    "REG_EFLAG_CO_RSRDO",
};

static decltype(sizeof(eflag_full_name_strs)) eflag_full_name_strs_count = (sizeof(eflag_full_name_strs)/sizeof(eflag_full_name_strs[0]));


static const char* co_attribute_name_strs[] = {
    "NONE",
    "SDO_R",
    "SDO_W",
    "SDO_RW",
    "TPDO",
    "RPDO",
    "TRPDO",
    "TSRDO",
    "RSRDO",
    "TRSRDO",
    "MB",
    "STR"
};

static decltype(sizeof(co_attribute_name_strs)) co_attribute_name_strs_count = (sizeof(co_attribute_name_strs)/sizeof(co_attribute_name_strs[0]));


static const char* co_attribute_full_name_strs[] = {
    "0",
    "ODA_SDO_R",
    "ODA_SDO_W",
    "ODA_SDO_RW",
    "ODA_TPDO",
    "ODA_RPDO",
    "ODA_TRPDO",
    "ODA_TSRDO",
    "ODA_RSRDO",
    "ODA_TRSRDO",
    "ODA_MB",
    "ODA_STR"
};

static decltype(sizeof(co_attribute_name_strs)) co_attribute_full_name_strs_count = (sizeof(co_attribute_full_name_strs)/sizeof(co_attribute_full_name_strs[0]));


QVector<RegFlag::Value> flags()
{
    static auto flags = {
        RegFlag::NONE,
        RegFlag::CONF,
        RegFlag::READONLY
    };
    static const QVector<RegFlag::Value> flags_vec(flags);

    return flags_vec;
}

QString flagName(RegFlag::Value flag)
{
    int flag_index = 8 - qCountLeadingZeroBits(static_cast<quint8>(flag));
    if(static_cast<decltype(sizeof(flag_name_strs))>(flag_index) >= flag_name_strs_count){
        return QString();
    }

    return QString(flag_name_strs[static_cast<int>(flag_index)]);
}

QStringList flagsNames()
{
    static const QStringList flag_names_list = QStringList(flag_name_strs, flag_name_strs + flag_name_strs_count);

    return flag_names_list;
}

QString flagFullName(RegFlag::Value flag)
{
    int flag_index = 8 - qCountLeadingZeroBits(static_cast<quint8>(flag));
    if(static_cast<decltype(sizeof(flag_full_name_strs))>(flag_index) >= flag_full_name_strs_count){
        return QString();
    }

    return QString(flag_full_name_strs[static_cast<int>(flag_index)]);
}

QStringList flagsFullNames()
{
    static const QStringList flag_full_names_list = QStringList(flag_full_name_strs, flag_full_name_strs + flag_full_name_strs_count);

    return flag_full_names_list;
}

QVector<RegEFlag::Value> eflags()
{
    static auto eflags = {
        RegEFlag::NONE,
        RegEFlag::RL_HIDE,
        RegEFlag::CO_HIDE,
        RegEFlag::CO_COUNT,
        RegEFlag::RESERVED,
        RegEFlag::CON_CNT_NMT,
        RegEFlag::CON_CNT_EM,
        RegEFlag::CON_CNT_SYNC,
        RegEFlag::CON_CNT_SYNC_PROD,
        RegEFlag::CON_CNT_STORAGE,
        RegEFlag::CON_CNT_TIME,
        RegEFlag::CON_CNT_EM_PROD,
        RegEFlag::CON_CNT_HB_CONS,
        RegEFlag::CON_CNT_HB_PROD,
        RegEFlag::CON_CNT_SDO_SRV,
        RegEFlag::CON_CNT_SDO_CLI,
        RegEFlag::CON_CNT_RPDO,
        RegEFlag::CON_CNT_TPDO,
        RegEFlag::CO_SDO_R,
        RegEFlag::CO_SDO_W,
        RegEFlag::CO_TPDO,
        RegEFlag::CO_RPDO,
        RegEFlag::CO_TSRDO,
        RegEFlag::CO_RSRDO,
    };
    static const QVector<RegEFlag::Value> eflags_vec(eflags);

    return eflags_vec;
}

QString eflagName(RegEFlag::Value eflag)
{
    int eflag_index = 32 - qCountLeadingZeroBits(static_cast<quint32>(eflag));
    if(static_cast<decltype(sizeof(eflag_name_strs))>(eflag_index) >= eflag_name_strs_count){
        return QString();
    }

    return QString(eflag_name_strs[static_cast<int>(eflag_index)]);
}

QStringList eflagsNames()
{
    static const QStringList eflag_names_list = QStringList(eflag_name_strs, eflag_name_strs + eflag_name_strs_count);

    return eflag_names_list;
}

QString eflagFullName(RegEFlag::Value eflag)
{
    int eflag_index = 32 - qCountLeadingZeroBits(static_cast<quint32>(eflag));
    if(static_cast<decltype(sizeof(eflag_full_name_strs))>(eflag_index) >= eflag_full_name_strs_count){
        return QString();
    }

    return QString(eflag_full_name_strs[static_cast<int>(eflag_index)]);
}

QStringList eflagsFullNames()
{
    static const QStringList eflag_full_names_list = QStringList(eflag_full_name_strs, eflag_full_name_strs + eflag_full_name_strs_count);

    return eflag_full_names_list;
}

QString coAttributeName(COAttribute::Value attr)
{
    int attr_index = 16 - qCountLeadingZeroBits(static_cast<quint16>(attr));
    if(static_cast<decltype(sizeof(co_attribute_name_strs))>(attr_index) >= co_attribute_name_strs_count){
        return QString();
    }

    return QString(co_attribute_name_strs[static_cast<int>(attr_index)]);
}

QStringList coAttributeNames()
{
    static const QStringList co_attr_names_list = QStringList(co_attribute_name_strs, co_attribute_name_strs + co_attribute_name_strs_count);

    return co_attr_names_list;
}

QString coAttributeFullName(COAttribute::Value attr)
{
    int attr_index = 16 - qCountLeadingZeroBits(static_cast<quint16>(attr));
    if(static_cast<decltype(sizeof(co_attribute_full_name_strs))>(attr_index) >= co_attribute_full_name_strs_count){
        return QString();
    }

    return QString(co_attribute_full_name_strs[static_cast<int>(attr_index)]);
}

QStringList coAttributeFullNames()
{
    static const QStringList co_attr_full_names_list = QStringList(co_attribute_full_name_strs, co_attribute_full_name_strs + co_attribute_full_name_strs_count);

    return co_attr_full_names_list;
}

co_attributes_t eflagsToCoAttributes(reg_eflags_t eflags)
{
    co_attributes_t attrs = COAttribute::NONE;

    if(eflags & (RegEFlag::CO_TPDO | RegEFlag::CO_RPDO)){
        attrs |= COAttribute::TRPDO;
    }else if(eflags & RegEFlag::CO_TPDO){
        attrs |= COAttribute::TPDO;
    }else if(eflags & RegEFlag::CO_RPDO){
        attrs |= COAttribute::RPDO;
    }

    if(eflags & (RegEFlag::CO_TSRDO | RegEFlag::CO_RSRDO)){
        attrs |= COAttribute::TRSRDO;
    }else if(eflags & RegEFlag::CO_TSRDO){
        attrs |= COAttribute::TSRDO;
    }else if(eflags & RegEFlag::CO_RSRDO){
        attrs |= COAttribute::RSRDO;
    }

    if(eflags & (RegEFlag::CO_SDO_W | RegEFlag::CO_SDO_R)){
        attrs |= COAttribute::SDO_RW;
    }else if(eflags & RegEFlag::CO_SDO_W){
        attrs |= COAttribute::SDO_W;
    }else if(eflags & RegEFlag::CO_SDO_R){
        attrs |= COAttribute::SDO_R;
    }

    return attrs;
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
        DataType::I32,
        DataType::I16,
        DataType::I8,
        DataType::U32,
        DataType::U16,
        DataType::U8,
        DataType::IQ24,
        DataType::IQ15,
        DataType::IQ7,
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

QString varDataTypeStr(DataType type)
{
    switch(type){
    case DataType::I32:
        return QString("reg_i32_t");
    case DataType::I16:
        return QString("reg_i16_t");
    case DataType::I8:
        return QString("reg_i8_t");
    case DataType::U32:
        return QString("reg_u32_t");
    case DataType::U16:
        return QString("reg_u16_t");
    case DataType::U8:
        return QString("reg_u8_t");
    case DataType::IQ24:
        return QString("reg_iq24_t");
    case DataType::IQ15:
        return QString("reg_iq15_t");
    case DataType::IQ7:
        return QString("reg_iq7_t");
    }

    return QString("long");
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

int sizeBytes(DataType type)
{
    switch(type){
    case DataType::I32:
        return 4;
    case DataType::I16:
        return 2;
    case DataType::I8:
        return 1;
    case DataType::U32:
        return 4;
    case DataType::U16:
        return 2;
    case DataType::U8:
        return 1;
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        return 4;
    }
    return 0;
}

bool isBoolean(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        break;
    }
    return false;
}

bool isString(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        break;
    }
    return false;
}

bool isNumeric(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        return true;
    }
    return false;
}

bool isInteger(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        return true;
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        break;
    }
    return false;
}

bool isSigned(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
        return true;
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        break;
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        return true;
    }
    return false;
}

bool isUnsigned(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
        break;
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        return true;
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        break;
    }
    return false;
}

bool isFractional(DataType type)
{
    switch(type){
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        break;
    case DataType::IQ24:
    case DataType::IQ15:
    case DataType::IQ7:
        return true;
    }
    return false;
}

}
