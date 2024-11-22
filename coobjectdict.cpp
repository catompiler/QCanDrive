#define OD_DEFINITION
#include "301/CO_ODinterface.h"
#include "coobjectdict.h"
#include <assert.h>
#include <algorithm>
#include <string.h>


#define OBJ_PTR_T(T, NAME, PTR)\
    T* NAME = static_cast<T*>(PTR)
#define VAR_OBJ(NAME, PTR) OBJ_PTR_T(OD_obj_var_t, NAME, PTR)
#define ARR_OBJ(NAME, PTR) OBJ_PTR_T(OD_obj_array_t, NAME, PTR)
#define REC_OBJ(NAME, PTR) OBJ_PTR_T(OD_obj_record_t, NAME, PTR)
#define V_O(PTR) VAR_OBJ(var, PTR)
#define A_O(PTR) ARR_OBJ(arr, PTR)
#define R_O(PTR) REC_OBJ(rec, PTR)
#define V_M V_O(m_object)
#define A_M A_O(m_object)
#define R_M R_O(m_object)


COObjectDict::COObjectDict(QObject *parent)
    : QObject{parent}
{
    clear();
}

COObjectDict::~COObjectDict()
{
    std::for_each(m_od_entries.begin(), m_od_entries.end(), [](OD_entry_t& od_entry){
        Entry::freeODEntryData(&od_entry);
    });
}

void COObjectDict::clear()
{
    m_od_entries.clear();

    updateOd();

    resetConfig();

    m_cntNmt = 0;
    m_cntEm = 0;
    m_cntSync = 0;
    m_cntSyncProd = 0;
    m_cntStorage = 0;
    m_cntTime = 0;
    m_cntEmProd = 0;
    m_cntHbCons = 0;
    m_cntHbProd = 0;
    m_cntSdoSrv = 0;
    m_cntSdoCli = 0;
    m_cntRpdo = 0;
    m_cntTpdo = 0;
}

void COObjectDict::make()
{
    resetConfig();

    if(!m_od_entries.isEmpty()){

        int last = 1;

        if(Entry(&m_od_entries.last()).isEmpty()){
            last ++;
        }

        bool need_sorting = false;

        for(int i = 0; i < m_od_entries.size() - last; i ++){
            if(m_od_entries[i].index > m_od_entries[i+1].index){
                need_sorting = true;
                break;
            }
        }

        if(need_sorting){
            removeEmptyEntries();
            fixEntriesSorting();
            updateOd();
        }
    }

    if(m_od_entries.isEmpty()) return;

    // add last empty entry.
    if(!Entry(&m_od_entries.last()).isEmpty()){
        addEntry();
    }

    m_config.CNT_NMT = m_cntNmt;
    m_config.ENTRY_H1017 = odEntryByIndex(OD_H1017_PRODUCER_HB_TIME);
    m_config.CNT_HB_CONS = m_cntHbCons;
    m_config.CNT_ARR_1016 = arraySizeByIndex(OD_H1016_CONSUMER_HB_TIME);
    m_config.ENTRY_H1016 = odEntryByIndex(OD_H1016_CONSUMER_HB_TIME);
    m_config.CNT_EM = m_cntEm;
    m_config.ENTRY_H1001 = odEntryByIndex(OD_H1001_ERR_REG);
    m_config.ENTRY_H1014 = odEntryByIndex(OD_H1014_COBID_EMERGENCY);
    m_config.ENTRY_H1015 = odEntryByIndex(OD_H1015_INHIBIT_TIME_EMCY);
    m_config.CNT_ARR_1003 = arraySizeByIndex(OD_H1003_PREDEF_ERR_FIELD);
    m_config.ENTRY_H1003 = odEntryByIndex(OD_H1003_PREDEF_ERR_FIELD);
    m_config.CNT_SDO_SRV = m_cntSdoSrv;
    m_config.ENTRY_H1200 = odEntryByIndex(OD_H1200_SDO_SERVER_1_PARAM);
    m_config.CNT_SDO_CLI = m_cntSdoCli;
    m_config.ENTRY_H1280 = odEntryByIndex(OD_H1280_SDO_CLIENT_1_PARAM);
    m_config.CNT_TIME = m_cntTime;
    m_config.ENTRY_H1012 = odEntryByIndex(OD_H1012_COBID_TIME);
    m_config.CNT_SYNC = m_cntSync;
    m_config.ENTRY_H1005 = odEntryByIndex(OD_H1005_COBID_SYNC);
    m_config.ENTRY_H1006 = odEntryByIndex(OD_H1006_COMM_CYCL_PERIOD);
    m_config.ENTRY_H1007 = odEntryByIndex(OD_H1007_SYNC_WINDOW_LEN);
    m_config.ENTRY_H1019 = odEntryByIndex(OD_H1019_SYNC_CNT_OVERFLOW);
    m_config.CNT_RPDO = m_cntRpdo;
    m_config.ENTRY_H1400 = odEntryByIndex(OD_H1400_RXPDO_1_PARAM);
    m_config.ENTRY_H1600 = odEntryByIndex(OD_H1600_RXPDO_1_MAPPING);
    m_config.CNT_TPDO = m_cntTpdo;
    m_config.ENTRY_H1800 = odEntryByIndex(OD_H1800_TXPDO_1_PARAM);
    m_config.ENTRY_H1A00 = odEntryByIndex(OD_H1A00_TXPDO_1_MAPPING);
    m_config.CNT_LEDS = 0;
    m_config.CNT_GFC = 0;
    m_config.ENTRY_H1300 = NULL;
    m_config.CNT_SRDO = 0;
    m_config.ENTRY_H1301 = NULL;
    m_config.ENTRY_H1381 = NULL;
    m_config.ENTRY_H13FE = NULL;
    m_config.ENTRY_H13FF = NULL;
    m_config.CNT_LSS_SLV = 0;
    m_config.CNT_LSS_MST = 0;
    m_config.CNT_GTWA = 0;
    m_config.CNT_TRACE = 0;
}

OD_t* COObjectDict::od()
{
    return &m_od;
}

CO_config_t* COObjectDict::config()
{
    return &m_config;
}

COObjectDict::Entry COObjectDict::addEntry()
{
    m_od_entries.append(OD_entry_t());

    OD_entry_t* od_entry = &m_od_entries.last();
    Entry::initODEntry(od_entry);

    updateOd();

    return Entry(od_entry);
}

void COObjectDict::removeEntry(int i)
{
    if(i < 0 || i >= m_od_entries.size()) return;

    m_od_entries.remove(i);

    updateOd();
}

COObjectDict::Entry COObjectDict::entryAt(int i)
{
    if(i < 0 || i >= m_od_entries.size()) return Entry();

    OD_entry_t* od_entry = &m_od_entries[i];
    return Entry(od_entry);
}

COObjectDict::Entry COObjectDict::entryByIndex(CO::Index entryIndex)
{
    auto it = std::find_if(m_od_entries.begin(), m_od_entries.end(), [&entryIndex](const OD_entry_t& od_entry){
        return od_entry.index == entryIndex;
    });

    if(it == m_od_entries.end()) return Entry();

    return Entry(it);
}

COObjectDict::Entry COObjectDict::add_H1000_DevType()
{
    Entry e = addEntry();
    e.setIndex(OD_H1000_DEV_TYPE);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    e.setDataLength(4);
    e.write<uint32_t>(0);

    m_cntNmt ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1001_ErrReg()
{
    Entry e = addEntry();
    e.setIndex(OD_H1001_ERR_REG);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R | ODA_TPDO);
    e.setDataLength(1);
    e.write<uint8_t>(0);

    m_cntEm  ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1002_ManufStatusReg()
{
    Entry e = addEntry();
    e.setIndex(OD_H1002_MANUF_STATUS_REG);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R | ODA_TRPDO);
    e.setDataLength(4);
    e.write<uint32_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H1003_PredefErrField()
{
    Entry e = addEntry();
    e.setIndex(OD_H1003_PREDEF_ERR_FIELD);
    e.setObjType(ARR);
    e.setSubEntriesCount(17);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 0);
    e.setAttribute(ODA_SDO_R, 1);

    e.write<uint8_t>(0x10, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1005_CobidSync()
{
    Entry e = addEntry();
    e.setIndex(OD_H1005_COBID_SYNC);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint32_t>(0x00000080);

    m_cntSync ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1006_CommCyclPeriod()
{
    Entry e = addEntry();
    e.setIndex(OD_H1006_COMM_CYCL_PERIOD);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint32_t>(0);

    m_cntSyncProd ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1007_SyncWindowLen()
{
    Entry e = addEntry();
    e.setIndex(OD_H1007_SYNC_WINDOW_LEN);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint32_t>(0);
    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1008_ManufDevName()
{
    Entry e = addEntry();
    e.setIndex(OD_H1008_MANUF_DEV_NAME);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H1009_ManufHwVersion()
{
    Entry e = addEntry();
    e.setIndex(OD_H1009_MANUF_HW_VERSION);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H100A_ManufSwVersion()
{
    Entry e = addEntry();
    e.setIndex(OD_H100A_MANUF_SW_VERSION);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H100C_GuardTime()
{
    Entry e = addEntry();
    e.setIndex(OD_H100C_GUARD_TIME);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(2);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint16_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H100D_LifetimeFactor()
{
    Entry e = addEntry();
    e.setIndex(OD_H100D_LIFETIME_FACTOR);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(1);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint8_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H1010_StoreParameters()
{
    Entry e = addEntry();
    e.setIndex(OD_H1010_STORE_PARAMETERS);
    e.setObjType(ARR);
    e.setSubEntriesCount(5);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x04, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0x00000001, i);
    }

    m_cntStorage ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1011_RestoreDefault()
{
    Entry e = addEntry();
    e.setIndex(OD_H1011_RESTORE_DEFAULT);
    e.setObjType(ARR);
    e.setSubEntriesCount(5);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x04);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0x00000001, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1012_CobidTime()
{
    Entry e = addEntry();
    e.setIndex(OD_H1012_COBID_TIME);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint32_t>(0x00000100);

    m_cntTime ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1013_HighResTimestamp()
{
    Entry e = addEntry();
    e.setIndex(OD_H1013_HIGH_RES_TIMESTAMP);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW | ODA_TRPDO);
    e.write<uint32_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H1014_CobidEmergency()
{
    Entry e = addEntry();
    e.setIndex(OD_H1014_COBID_EMERGENCY);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(4);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint32_t>(0x80);

    m_cntEmProd ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1015_InhibitTimeEmcy()
{
    Entry e = addEntry();
    e.setIndex(OD_H1015_INHIBIT_TIME_EMCY);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(2);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint16_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H1016_ConsumerHbTime()
{
    Entry e = addEntry();
    e.setIndex(OD_H1016_CONSUMER_HB_TIME);
    e.setObjType(ARR);
    e.setSubEntriesCount(9);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x08, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0, i);
    }

    m_cntHbCons ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1017_ProducerHbTime()
{
    Entry e = addEntry();
    e.setIndex(OD_H1017_PRODUCER_HB_TIME);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(2);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint16_t>(0);

    m_cntHbProd ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1018_IdentityObject()
{
    Entry e = addEntry();
    e.setIndex(OD_H1018_IDENTITY_OBJECT);
    e.setObjType(REC);
    e.setSubEntriesCount(5);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x04, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setAttribute(ODA_SDO_R, i);
        e.setDataLength(4, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1019_SyncCntOverflow()
{
    Entry e = addEntry();
    e.setIndex(OD_H1019_SYNC_CNT_OVERFLOW);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(1);
    e.setAttribute(ODA_SDO_RW);
    e.write<uint8_t>(0);
    return e;
}

COObjectDict::Entry COObjectDict::add_H1020_VerifyConfig()
{
    Entry e = addEntry();
    e.setIndex(OD_H1020_VERIFY_CONFIG);
    e.setObjType(ARR);
    e.setSubEntriesCount(3);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x02, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0, i);
    }

    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1021_StoreEds()
{
    Entry e = addEntry();
    e.setIndex(OD_H1021_STORE_EDS);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1022_StoreFormat()
{
    Entry e = addEntry();
    e.setIndex(OD_H1022_STORE_FORMAT);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(2);
    e.setAttribute(ODA_SDO_R);
    e.write<uint16_t>(0);
    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1023_OsCmd()
{
    Entry e = addEntry();
    e.setIndex(OD_H1023_OS_CMD);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1024_OsCmdMode()
{
    Entry e = addEntry();
    e.setIndex(OD_H1024_OS_CMD_MODE);
    e.setObjType(VAR);
    e.setSubEntriesCount(1);
    e.setDataLength(1);
    e.setAttribute(ODA_SDO_W);
    e.write<uint8_t>(0);
    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1025_OsDbgInterface()
{
    Entry e = addEntry();
    e.setIndex(OD_H1025_OS_DBG_INTERFACE);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1026_OsPrompt()
{
    Entry e = addEntry();
    e.setIndex(OD_H1026_OS_PROMPT);
    e.setObjType(ARR);
    e.setSubEntriesCount(4);

    e.setDataLength(1, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW | ODA_TRPDO, 1);

    e.write<uint8_t>(0x03, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint8_t>(0, i);
    }

    return e;
}

COObjectDict::Entry COObjectDict::add_H1027_ModuleList()
{
    Entry e = addEntry();
    e.setIndex(OD_H1027_MODULE_LIST);
    e.setObjType(ARR);
    e.setSubEntriesCount(2);

    e.setDataLength(2, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_R, 1);

    e.write<uint8_t>(0x01, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint8_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1028_EmcyConsumer()
{
    Entry e = addEntry();
    e.setIndex(OD_H1028_EMCY_CONSUMER);
    e.setObjType(ARR);
    e.setSubEntriesCount(0x9);

    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x08, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1029_ErrBehavior()
{
    Entry e = addEntry();
    e.setIndex(OD_H1029_ERR_BEHAVIOR);
    e.setObjType(ARR);
    e.setSubEntriesCount(0x3);

    e.setDataLength(1, 1);
    e.setAttribute(ODA_SDO_R, 0);
    e.setAttribute(ODA_SDO_RW, 1);

    e.write<uint8_t>(0x02, 0);
    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.write<uint8_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1200_SdoServer1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1200_SDO_SERVER_1_PARAM);
    e.setObjType(REC);
    e.setSubEntriesCount(3);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x02, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setAttribute(ODA_SDO_R | ODA_TPDO, i);
        e.setDataLength(4, i);
    }
    e.write<uint32_t>(0x600, 1);
    e.write<uint32_t>(0x580, 2);

    m_cntSdoSrv ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1201_SdoServer1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1200_SDO_SERVER_1_PARAM + 1);
    e.setObjType(REC);
    e.setSubEntriesCount(3);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x02, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setAttribute(ODA_SDO_R | ODA_TPDO, i);
        e.setDataLength(4, i);
    }
    e.write<uint32_t>(0x600, 1);
    e.write<uint32_t>(0x580, 2);

    m_cntSdoSrv ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1280_SdoClient1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1280_SDO_CLIENT_1_PARAM);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x03, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()) - 1; i ++){
        e.setSubIndex(i, i);
        e.setAttribute(ODA_SDO_RW | ODA_TRPDO, i);
        e.setDataLength(4, i);
        e.write<uint32_t>(0x80000000, i);
    }
    e.setSubIndex(0x03, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.setDataLength(1, 3);
    e.write<uint8_t>(0x01, 3);

    m_cntSdoCli ++;

    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1300_GfcParam()
{
    Entry e = addEntry();
    e.setIndex(OD_H1300_GFC_PARAM);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H1301_Srdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1301_SRDO_1_PARAM);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H1381_Srdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1381_SRDO_1_MAPPING);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H13FE_SrdoValid()
{
    Entry e = addEntry();
    e.setIndex(OD_H13FE_SRDO_VALID);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

/*COObjectDict::Entry COObjectDict::add_H13FF_SrdoChecksum()
{
    Entry e = addEntry();
    e.setIndex(OD_H13FF_SRDO_CHECKSUM);
    e.setSubEntriesCount(1);
    e.setAttribute(ODA_SDO_R);
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1400_Rxpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1400_RXPDO_1_PARAM);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x05, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0x80000200, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(5, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint32_t>(0, 3);

    m_cntRpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1401_Rxpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1400_RXPDO_1_PARAM + 1);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x05, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0x80000300, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(5, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint32_t>(0, 3);

    m_cntRpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1402_Rxpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1400_RXPDO_1_PARAM + 2);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x05, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0x80000400, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(5, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint32_t>(0, 3);

    m_cntRpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1403_Rxpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1400_RXPDO_1_PARAM + 3);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x05, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0x80000500, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(5, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint32_t>(0, 3);

    m_cntRpdo ++;

    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1404_Rxpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1400_RXPDO_1_PARAM + 4);
    e.setObjType(REC);
    e.setSubEntriesCount(4);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x05, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0x80000000, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(5, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint32_t>(0, 3);

    m_cnt_rpdo ++;

    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1600_Rxpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1600_RXPDO_1_MAPPING);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, 0);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1601_Rxpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1600_RXPDO_1_MAPPING + 1);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, 0);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1602_Rxpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1600_RXPDO_1_MAPPING + 2);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, 0);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1603_Rxpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1600_RXPDO_1_MAPPING + 3);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, 0);
    }
    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1604_Rxpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1600_RXPDO_1_MAPPING + 4);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, 0);
    }
    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1800_Txpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1800_TXPDO_1_PARAM);
    e.setObjType(REC);
    e.setSubEntriesCount(6);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x06, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0xC0000180, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(3, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint16_t>(0, 3);

    e.setSubIndex(5, 4);
    e.setDataLength(2, 4);
    e.setAttribute(ODA_SDO_RW, 4);
    e.write<uint16_t>(0, 4);

    e.setSubIndex(6, 5);
    e.setDataLength(1, 5);
    e.setAttribute(ODA_SDO_RW, 5);
    e.write<uint8_t>(0, 5);

    m_cntTpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1801_Txpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1800_TXPDO_1_PARAM + 1);
    e.setObjType(REC);
    e.setSubEntriesCount(6);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x06, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0xC0000280, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(3, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint16_t>(0, 3);

    e.setSubIndex(5, 4);
    e.setDataLength(2, 4);
    e.setAttribute(ODA_SDO_RW, 4);
    e.write<uint16_t>(0, 4);

    e.setSubIndex(6, 5);
    e.setDataLength(1, 5);
    e.setAttribute(ODA_SDO_RW, 5);
    e.write<uint8_t>(0, 5);

    m_cntTpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1802_Txpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1800_TXPDO_1_PARAM + 2);
    e.setObjType(REC);
    e.setSubEntriesCount(6);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x06, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0xC0000380, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(3, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint16_t>(0, 3);

    e.setSubIndex(5, 4);
    e.setDataLength(2, 4);
    e.setAttribute(ODA_SDO_RW, 4);
    e.write<uint16_t>(0, 4);

    e.setSubIndex(6, 5);
    e.setDataLength(1, 5);
    e.setAttribute(ODA_SDO_RW, 5);
    e.write<uint8_t>(0, 5);

    m_cntTpdo ++;

    return e;
}

COObjectDict::Entry COObjectDict::add_H1803_Txpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1800_TXPDO_1_PARAM + 3);
    e.setObjType(REC);
    e.setSubEntriesCount(6);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x06, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0xC0000480, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(3, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint16_t>(0, 3);

    e.setSubIndex(5, 4);
    e.setDataLength(2, 4);
    e.setAttribute(ODA_SDO_RW, 4);
    e.write<uint16_t>(0, 4);

    e.setSubIndex(6, 5);
    e.setDataLength(1, 5);
    e.setAttribute(ODA_SDO_RW, 5);
    e.write<uint8_t>(0, 5);

    m_cntTpdo ++;

    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1804_Txpdo1Param()
{
    Entry e = addEntry();
    e.setIndex(OD_H1800_TXPDO_1_PARAM + 4);
    e.setObjType(REC);
    e.setSubEntriesCount(6);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_R, 0);
    e.write<uint8_t>(0x06, 0);

    e.setSubIndex(1, 1);
    e.setDataLength(4, 1);
    e.setAttribute(ODA_SDO_RW, 1);
    e.write<uint32_t>(0xC0000000, 1);

    e.setSubIndex(2, 2);
    e.setDataLength(1, 2);
    e.setAttribute(ODA_SDO_RW, 2);
    e.write<uint8_t>(254, 2);

    e.setSubIndex(3, 3);
    e.setDataLength(2, 3);
    e.setAttribute(ODA_SDO_RW, 3);
    e.write<uint16_t>(0, 3);

    e.setSubIndex(5, 4);
    e.setDataLength(2, 4);
    e.setAttribute(ODA_SDO_RW, 4);
    e.write<uint16_t>(0, 4);

    e.setSubIndex(6, 5);
    e.setDataLength(1, 5);
    e.setAttribute(ODA_SDO_RW, 5);
    e.write<uint8_t>(0, 5);

    m_cnt_tpdo ++;

    return e;
}*/

COObjectDict::Entry COObjectDict::add_H1A00_Txpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1A00_TXPDO_1_MAPPING);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1A01_Txpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1A00_TXPDO_1_MAPPING + 1);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1A02_Txpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1A00_TXPDO_1_MAPPING + 2);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}

COObjectDict::Entry COObjectDict::add_H1A03_Txpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1A00_TXPDO_1_MAPPING + 3);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}

/*COObjectDict::Entry COObjectDict::add_H1A04_Txpdo1Mapping()
{
    Entry e = addEntry();
    e.setIndex(OD_H1A00_TXPDO_1_MAPPING + 4);
    e.setObjType(REC);
    e.setSubEntriesCount(9);

    e.setSubIndex(0, 0);
    e.setDataLength(1, 0);
    e.setAttribute(ODA_SDO_RW, 0);
    e.write<uint8_t>(0, 0);

    for(int i = 1; i < static_cast<int>(e.subEntriesCount()); i ++){
        e.setSubIndex(i, i);
        e.setDataLength(4, i);
        e.setAttribute(ODA_SDO_RW, i);
        e.write<uint32_t>(0, i);
    }
    return e;
}*/


void COObjectDict::updateOd()
{
    if(m_od_entries.isEmpty()){
        m_od.list = nullptr;
        m_od.size = 0;
    }else{
        m_od.list = m_od_entries.data();
        m_od.size = m_od_entries.size();
    }
}

void COObjectDict::resetConfig()
{
    memset(&m_config, 0, sizeof(CO_config_t));
}

OD_entry_t* COObjectDict::odEntryByIndex(CO::Index entryIndex)
{
    Entry e = entryByIndex(entryIndex);

    if(!e.isValid()) return nullptr;

    return e.odEntry();
}

uint8_t COObjectDict::arraySizeByIndex(CO::Index entryIndex)
{
    Entry e = entryByIndex(entryIndex);

    if(!e.isValid()) return 0;
    if(e.objType() != ARR) return 0;

    return e.subEntriesCount() - 1;
}

void COObjectDict::removeEmptyEntries()
{
    for(int i = 0; i < m_od_entries.size();){
        Entry e(&m_od_entries[i]);

        if(!e.isEmpty()){
            m_od_entries.remove(i);
        }else{
            i ++;
        }
    }
}

void COObjectDict::fixEntriesSorting()
{
    std::sort(m_od_entries.begin(), m_od_entries.end(), [](const OD_entry_t& le, const OD_entry_t& re){
        return le.index < re.index;
    });
}


COObjectDict::Entry::Entry(OD_entry_t* entry)
{
    m_entry = entry;
}

COObjectDict::Entry::Entry(const Entry& entry)
{
    m_entry = entry.m_entry;
}

COObjectDict::Entry::Entry(Entry&& entry)
{
    m_entry = entry.m_entry;
    entry.m_entry = nullptr;
}

COObjectDict::Entry::~Entry()
{
}

bool COObjectDict::Entry::isValid() const
{
    return m_entry != nullptr;
}

bool COObjectDict::Entry::isEmpty() const
{
    assert(m_entry != nullptr);

    if(m_entry->odObject != nullptr) return false;
    if(m_entry->odObjectType != NO_OBJ) return false;
    if(m_entry->subEntriesCount != 0) return false;
    if(m_entry->extension != nullptr) return false;
    if(m_entry->index != 0) return false;

    return true;
}

int COObjectDict::Entry::subEntry(CO::SubIndex subIndex) const
{
    assert(m_entry != nullptr);

    switch(m_entry->odObjectType){
    case NO_OBJ:
        break;
    case VAR:
        if(subIndex != 0) break;
        return 0;
    case ARR:
        if(subIndex >= m_entry->subEntriesCount) break;
        return subIndex;
    case REC:{
        for(int i = 0; i < m_entry->subEntriesCount; i ++){
            Rec rec(m_entry->odObject, i);
            if(rec.subIndex() == subIndex){
                return i;
            }
        }
    }break;
    }

    return -1;
}

OD_entry_t* COObjectDict::Entry::odEntry()
{
    return m_entry;
}

const OD_entry_t* COObjectDict::Entry::odEntry() const
{
    return m_entry;
}

CO::SubIndex COObjectDict::Entry::subIndex(int subEntry) const
{
    assert(m_entry != nullptr);

    switch(m_entry->odObjectType){
    case NO_OBJ:
    case VAR:
    case ARR:
        break;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        return rec.subIndex();
    }break;
    }

    return static_cast<CO::SubIndex>(subEntry);
}

bool COObjectDict::Entry::setSubIndex(CO::SubIndex subIndex, int subEntry)
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
    case VAR:
    case ARR:
        return false;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        rec.setSubIndex(subIndex);
    }break;
    }

    return true;
}

CO::Index COObjectDict::Entry::index() const
{
    assert(m_entry != nullptr);

    return static_cast<CO::Index>(m_entry->index);
}

void COObjectDict::Entry::setIndex(CO::Index newIndex)
{
    assert(m_entry != nullptr);

    m_entry->index = static_cast<uint16_t>(newIndex);
}

COObjectDict::ObjectType COObjectDict::Entry::objType() const
{
    assert(m_entry != nullptr);

    return static_cast<COObjectDict::ObjectType>(m_entry->odObjectType);
}

void COObjectDict::Entry::setObjType(ObjectType newObjType)
{
    assert(m_entry != nullptr);

    if(m_entry->odObjectType == static_cast<uint8_t>(newObjType)) return;

    freeODEntryData(m_entry);

    switch(newObjType){
    case NO_OBJ:
        m_entry->odObjectType = NO_OBJ;
        m_entry->odObject = nullptr;
        m_entry->subEntriesCount = 0;
        return;
    case VAR:
        m_entry->odObject = Var::createODObj();
        break;
    case ARR:
        m_entry->odObject = Arr::createODObj();
        break;
    case REC:
        m_entry->odObject = Rec::createODObj(1);
        break;
    }
    m_entry->odObjectType = static_cast<uint8_t>(newObjType);
    m_entry->subEntriesCount = 1;
}

OD_extension_t* COObjectDict::Entry::extension() const
{
    assert(m_entry != nullptr);

    return m_entry->extension;
}

void COObjectDict::Entry::setExtension(OD_extension_t* newExtension)
{
    assert(m_entry != nullptr);

    m_entry->extension = newExtension;
}

OD_size_t COObjectDict::Entry::subEntriesCount() const
{
    assert(m_entry != nullptr);

    return m_entry->subEntriesCount;
}

bool COObjectDict::Entry::setSubEntriesCount(OD_size_t count)
{
    assert(m_entry != nullptr);

    if(count == 0) return false;
    if(m_entry->subEntriesCount == count) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
    case VAR:
        return false;
    case ARR:{
        Arr arr(m_entry->odObject);
        arr.setDataElementLength(count - 1);
    }break;
    case REC:
        m_entry->odObject = Rec::resizeODObj(m_entry->odObject, m_entry->subEntriesCount, count);
        break;
    }

    m_entry->subEntriesCount = count;

    return true;
}

OD_size_t COObjectDict::Entry::dataLength(int subEntry) const
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return 0;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        break;
    case VAR:{
        Var var(m_entry->odObject);

        return var.dataLength();
    }
    case ARR:{
        if(subEntry == 0) return 1;

        Arr arr(m_entry->odObject);

        return arr.dataElementSizeof();
    }
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        return rec.dataLength();
    }
    }

    return 0;
}

bool COObjectDict::Entry::setDataLength(OD_size_t newDataLength, int subEntry)
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        return false;
    case VAR:{
        Var var(m_entry->odObject);

        var.setDataLength(newDataLength);
    }break;
    case ARR:{
        if(subEntry == 0) return false;

        Arr arr(m_entry->odObject);

        arr.setDataElementSizeof(newDataLength);
    }break;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        rec.setDataLength(newDataLength);
    }break;
    }

    return true;
}

OD_attr_t COObjectDict::Entry::attribute(int subEntry) const
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return 0;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        break;
    case VAR:{
        Var var(m_entry->odObject);

        return var.attribute();
    }
    case ARR:{
        Arr arr(m_entry->odObject);

        if(subEntry == 0) return arr.attribute0();
        return arr.attribute();
    }
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        return rec.attribute();
    }
    }

    return 0;
}

bool COObjectDict::Entry::setAttribute(OD_attr_t newAttr, int subEntry)
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        return false;
    case VAR:{
        Var var(m_entry->odObject);

        var.setAttribute(newAttr);
    }break;
    case ARR:{
        Arr arr(m_entry->odObject);

        if(subEntry == 0){
            arr.setAttribute0(newAttr);
        }else{
            arr.setAttribute(newAttr);
        }
    }break;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        rec.setAttribute(newAttr);
    }break;
    }

    return true;
}

bool COObjectDict::Entry::read(void* data, OD_size_t dataSize, int subEntry) const
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        break;
    case VAR:{
        Var var(m_entry->odObject);

        return var.read(data, dataSize);
    }break;
    case ARR:{
        if(subEntry == 0) return false;

        Arr arr(m_entry->odObject);

        return arr.read(data, dataSize, subEntry);
    }break;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        return rec.read(data, dataSize);
    }break;
    }

    return false;
}

bool COObjectDict::Entry::write(const void* data, OD_size_t dataSize, int subEntry)
{
    assert(m_entry != nullptr);

    if(subEntry < 0 || subEntry >= m_entry->subEntriesCount) return false;

    switch(m_entry->odObjectType){
    case NO_OBJ:
        break;
    case VAR:{
        Var var(m_entry->odObject);

        return var.write(data, dataSize);
    }break;
    case ARR:{
        if(subEntry == 0) return false;

        Arr arr(m_entry->odObject);

        return arr.write(data, dataSize, subEntry);
    }break;
    case REC:{
        Rec rec(m_entry->odObject, subEntry);

        return rec.write(data, dataSize);
    }break;
    }

    return false;
}

OD_entry_t* COObjectDict::Entry::createODEntry()
{
    OD_entry_t* entry = new OD_entry_t;

    initODEntry(entry);

    return entry;
}

void COObjectDict::Entry::destroyODEntry(OD_entry_t* entry)
{
    if(entry){
        freeODEntryData(entry);
        delete entry;
    }
}

void COObjectDict::Entry::initODEntry(OD_entry_t* entry)
{
    resetODEntry(entry);
}

void COObjectDict::Entry::resetODEntry(OD_entry_t* entry)
{
    assert(entry != nullptr);

    entry->index = static_cast<uint16_t>(0);
    entry->odObjectType = static_cast<uint8_t>(0);
    entry->subEntriesCount = 0;
    entry->odObject = nullptr;
    entry->extension = nullptr;
}

void COObjectDict::Entry::freeODEntryData(OD_entry_t* entry)
{
    if(entry && entry->odObject){
        switch(entry->odObjectType){
        case NO_OBJ:
            break;
        case VAR:
            Var::destroyODObj(entry->odObject);
            break;
        case ARR:
            Arr::destroyODObj(entry->odObject);
            break;
        case REC:
            Rec::destroyODObj(entry->odObject, entry->subEntriesCount);
            break;
        }
    }
}

COObjectDict::Entry& COObjectDict::Entry::operator=(const Entry& entry)
{
    m_entry = entry.m_entry;

    return *this;
}

COObjectDict::Entry& COObjectDict::Entry::operator=(Entry&& entry)
{
    OD_entry_t* od_entry = entry.m_entry;
    entry.m_entry = nullptr;
    m_entry = od_entry;

    return *this;
}



COObjectDict::Object::Object()
{
    m_object = nullptr;
}

COObjectDict::Object::Object(ObjectPtr ptr)
{
    m_object = ptr;
}

COObjectDict::Object::Object(Object&& obj)
{
    m_object = obj.m_object;
    obj.m_object = nullptr;
}

COObjectDict::Object::~Object()
{
}



COObjectDict::Var::Var()
    :Object()
{
}

COObjectDict::Var::Var(ObjectPtr ptr)
    :Object(ptr)
{
}

COObjectDict::Var::Var(Var&& var)
    :Object(std::move(var))
{
}

COObjectDict::Var::~Var()
{
}

COObjectDict::ObjectPtr COObjectDict::Var::createODObj()
{
    OD_obj_var_t* var = new OD_obj_var_t;

    var->attribute = 0;
    var->dataLength = 0;
    var->dataOrig = nullptr;

    return var;
}

void COObjectDict::Var::destroyODObj(ObjectPtr ptr)
{
    V_O(ptr);

    if(var == nullptr) return;

    if(var->dataOrig) delete[] static_cast<uint8_t*>(var->dataOrig);
    delete var;
}

void* COObjectDict::Var::dataOrig()
{
    assert(m_object != nullptr);

    V_M;

    return var->dataOrig;
}

const void* COObjectDict::Var::dataOrig() const
{
    assert(m_object != nullptr);

    V_M;

    return var->dataOrig;
}

OD_attr_t COObjectDict::Var::attribute() const
{
    assert(m_object != nullptr);

    V_M;

    return var->attribute;
}

void COObjectDict::Var::setAttribute(OD_attr_t newAttribute)
{
    assert(m_object != nullptr);

    V_M;

    if(var->dataLength > 1){
        newAttribute |= ODA_MB;
    }else{
        newAttribute &= ~ODA_MB;
    }

    var->attribute = newAttribute;
}

OD_size_t COObjectDict::Var::dataLength() const
{
    assert(m_object != nullptr);

    V_M;

    return var->dataLength;
}

void COObjectDict::Var::setDataLength(OD_size_t newDataLength)
{
    assert(m_object != nullptr);

    V_M;

    if(var->dataLength == newDataLength) return;

    if(var->dataOrig != nullptr){
        delete[] static_cast<uint8_t*>(var->dataOrig);
    }

    if(newDataLength != 0){
        var->dataOrig = static_cast<void*>(new uint8_t[newDataLength]);
        memset(var->dataOrig, 0, newDataLength);
    }else{
        var->dataOrig = nullptr;
    }
    var->dataLength = newDataLength;

    setAttribute(var->attribute);
}

bool COObjectDict::Var::read(void* data, OD_size_t dataSize) const
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    V_M;

    if(var->dataOrig == nullptr) return false;
    if(var->dataLength != dataSize) return false;

    memcpy(data, var->dataOrig, dataSize);

    return true;
}

bool COObjectDict::Var::write(const void* data, OD_size_t dataSize)
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    V_M;

    if(var->dataOrig == nullptr) return false;
    if(var->dataLength != dataSize) return false;

    memcpy(var->dataOrig, data, dataSize);

    return true;
}



COObjectDict::Arr::Arr()
    :Object()
{
}

COObjectDict::Arr::Arr(ObjectPtr ptr)
    :Object(ptr)
{
}

COObjectDict::Arr::Arr(Arr&& arr)
    :Object(std::move(arr))
{
}

COObjectDict::Arr::~Arr()
{
}

COObjectDict::ObjectPtr COObjectDict::Arr::createODObj()
{
    OD_obj_array_t* arr = new OD_obj_array_t;

    arr->dataOrig0 = new uint8_t(0);
    arr->dataOrig = nullptr;
    arr->attribute0 = 0;
    arr->attribute = 0;
    arr->dataElementLength = 0;
    arr->dataElementSizeof = 0;

    return arr;
}

void COObjectDict::Arr::destroyODObj(ObjectPtr ptr)
{
    A_O(ptr);

    if(arr == nullptr) return;

    if(arr->dataOrig) delete[] static_cast<uint8_t*>(arr->dataOrig);
    delete arr->dataOrig0;
    delete arr;
}

void* COObjectDict::Arr::dataOrig0()
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataOrig0;
}

const void* COObjectDict::Arr::dataOrig0() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataOrig0;
}

void* COObjectDict::Arr::dataOrig()
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataOrig;
}

const void* COObjectDict::Arr::dataOrig() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataOrig;
}

OD_attr_t COObjectDict::Arr::attribute0() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->attribute0;
}

void COObjectDict::Arr::setAttribute0(OD_attr_t newAttribute)
{
    assert(m_object != nullptr);

    A_M;

    newAttribute &= ~ODA_MB;

    arr->attribute0 = newAttribute;
}

OD_attr_t COObjectDict::Arr::attribute() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->attribute;
}

void COObjectDict::Arr::setAttribute(OD_attr_t newAttribute)
{
    assert(m_object != nullptr);

    A_M;

    if(arr->dataElementSizeof > 1){
        newAttribute |= ODA_MB;
    }else{
        newAttribute &= ~ODA_MB;
    }

    arr->attribute = newAttribute;
}

OD_size_t COObjectDict::Arr::dataElementLength() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataElementLength;
}

void COObjectDict::Arr::setDataElementLength(OD_size_t newDataElementLength)
{
    assert(m_object != nullptr);

    A_M;

    if(arr->dataElementLength == newDataElementLength) return;

    void* oldDataOrig = arr->dataOrig;

    OD_size_t size = newDataElementLength * arr->dataElementSizeof;

    if(size != 0){
        arr->dataOrig = static_cast<void*>(new uint8_t[size]);
        memset(arr->dataOrig, 0, size);

        if(oldDataOrig != nullptr){
            OD_size_t min_size = std::min(arr->dataElementLength, newDataElementLength);
            memcpy(arr->dataOrig, oldDataOrig, min_size);
        }

    }else{
        arr->dataOrig = nullptr;
    }
    arr->dataElementLength = newDataElementLength;

    if(arr->dataOrig0){
        *static_cast<uint8_t*>(arr->dataOrig0) =
                static_cast<uint8_t>(newDataElementLength);
    }

    if(oldDataOrig != nullptr){
        delete[] static_cast<uint8_t*>(oldDataOrig);
    }
}

OD_size_t COObjectDict::Arr::dataElementSizeof() const
{
    assert(m_object != nullptr);

    A_M;

    return arr->dataElementSizeof;
}

void COObjectDict::Arr::setDataElementSizeof(OD_size_t newDataElementSizeof)
{
    assert(m_object != nullptr);

    A_M;

    if(arr->dataElementSizeof == newDataElementSizeof) return;

    if(arr->dataOrig != nullptr){
        delete[] static_cast<uint8_t*>(arr->dataOrig);
    }

    OD_size_t size = arr->dataElementLength * newDataElementSizeof;

    if(size != 0){
        arr->dataOrig = static_cast<void*>(new uint8_t[size]);
        memset(arr->dataOrig, 0, size);
    }else{
        arr->dataOrig = nullptr;
    }
    arr->dataElementSizeof = newDataElementSizeof;

    setAttribute(arr->attribute);
}

bool COObjectDict::Arr::read(void* data, OD_size_t dataSize, int subEntry) const
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    A_M;

    if(subEntry < 0 || static_cast<OD_size_t>(subEntry) > arr->dataElementLength) return false;
    if(subEntry == 0){
        if(arr->dataOrig0 == nullptr) return false;
        if(dataSize != 1) return false;

        memcpy(data, arr->dataOrig0, dataSize);
    }else{
        if(arr->dataOrig == nullptr) return false;
        if(arr->dataElementSizeof != dataSize) return false;

        OD_size_t arrSubEntry = subEntry - 1;
        uint8_t* arrData = static_cast<uint8_t*>(arr->dataOrig);
        uint8_t* dataSubEntry = &arrData[arrSubEntry * arr->dataElementSizeof];

        memcpy(data, dataSubEntry, dataSize);
    }

    return true;
}

bool COObjectDict::Arr::write(const void* data, OD_size_t dataSize, int subEntry)
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    A_M;

    if(subEntry < 0 || static_cast<OD_size_t>(subEntry) > arr->dataElementLength) return false;
    if(subEntry == 0){
        if(arr->dataOrig0 == nullptr) return false;
        if(dataSize != 1) return false;

        memcpy(arr->dataOrig0, data, dataSize);
    }else{
        if(arr->dataOrig == nullptr) return false;
        if(arr->dataElementSizeof != dataSize) return false;

        OD_size_t arrSubEntry = subEntry - 1;
        uint8_t* arrData = static_cast<uint8_t*>(arr->dataOrig);
        uint8_t* dataSubEntry = &arrData[arrSubEntry * arr->dataElementSizeof];

        memcpy(dataSubEntry, data, dataSize);
    }

    return true;
}



COObjectDict::Rec::Rec()
    :Object()
{
}

COObjectDict::Rec::Rec(ObjectPtr ptr)
    :Object(ptr)
{
}

COObjectDict::Rec::Rec(ObjectPtr ptr, OD_size_t subEntry)
    :Object(&static_cast<OD_obj_record_t*>(ptr)[subEntry])
{

}

COObjectDict::Rec::Rec(Rec&& rec)
    :Object(std::move(rec))
{
}

COObjectDict::Rec::~Rec()
{
}

COObjectDict::ObjectPtr COObjectDict::Rec::createODObj(OD_size_t count)
{
    if(count == 0) return nullptr;

    OD_obj_record_t* rec = new OD_obj_record_t[count];

    for(OD_size_t i = 0; i < count; i ++){
        rec[i].dataOrig = nullptr;
        rec[i].subIndex = static_cast<uint8_t>(i);
        rec[i].attribute = 0;
        rec[i].dataLength = 0;
    }

    return rec;
}

COObjectDict::ObjectPtr COObjectDict::Rec::resizeODObj(ObjectPtr ptr, OD_size_t oldCount, OD_size_t newCount)
{
    if(ptr == nullptr) return ptr;
    if(newCount == 0) return ptr;
    if(oldCount == newCount) return ptr;

    ObjectPtr newPtr = createODObj(newCount);
    if(newPtr == nullptr) return nullptr;

    size_t min_count = std::min(oldCount, newCount);
    if(min_count) memcpy(newPtr, ptr, min_count * sizeof(OD_obj_record_t));

    R_O(ptr);
    for(OD_size_t i = newCount; i < oldCount; i ++){
        if(rec[i].dataOrig) delete[] static_cast<uint8_t*>(rec[i].dataOrig);
    }

    delete[] rec;

    return newPtr;
}

void COObjectDict::Rec::destroyODObj(ObjectPtr ptr, OD_size_t count)
{
    R_O(ptr);

    if(rec == nullptr) return;

    for(OD_size_t i = 0; i < count; i ++){
        if(rec[i].dataOrig) delete[] static_cast<uint8_t*>(rec[i].dataOrig);
    }
    delete[] rec;
}

void* COObjectDict::Rec::dataOrig()
{
    assert(m_object != nullptr);

    R_M;

    return rec->dataOrig;
}

const void* COObjectDict::Rec::dataOrig() const
{
    assert(m_object != nullptr);

    R_M;

    return rec->dataOrig;
}

CO::SubIndex COObjectDict::Rec::subIndex() const
{
    assert(m_object != nullptr);

    R_M;

    return rec->subIndex;
}

void COObjectDict::Rec::setSubIndex(CO::SubIndex newSubIndex)
{
    assert(m_object != nullptr);

    R_M;

    rec->subIndex = newSubIndex;
}

OD_attr_t COObjectDict::Rec::attribute() const
{
    assert(m_object != nullptr);

    R_M;

    return rec->attribute;
}

void COObjectDict::Rec::setAttribute(OD_attr_t newAttribute)
{
    assert(m_object != nullptr);

    R_M;

    if(rec->dataLength > 1){
        newAttribute |= ODA_MB;
    }else{
        newAttribute &= ~ODA_MB;
    }

    rec->attribute = newAttribute;
}

OD_size_t COObjectDict::Rec::dataLength() const
{
    assert(m_object != nullptr);

    R_M;

    return rec->dataLength;
}

void COObjectDict::Rec::setDataLength(OD_size_t newDataLength)
{
    assert(m_object != nullptr);

    R_M;

    if(rec->dataLength == newDataLength) return;

    if(rec->dataOrig != nullptr){
        delete[] static_cast<uint8_t*>(rec->dataOrig);
    }

    if(newDataLength != 0){
        rec->dataOrig = static_cast<void*>(new uint8_t[newDataLength]);
        memset(rec->dataOrig, 0, newDataLength);
    }else{
        rec->dataOrig = nullptr;
    }
    rec->dataLength = newDataLength;

    setAttribute(rec->attribute);
}

bool COObjectDict::Rec::read(void* data, OD_size_t dataSize) const
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    R_M;

    if(rec->dataOrig == nullptr) return false;
    if(rec->dataLength != dataSize) return false;

    memcpy(data, rec->dataOrig, dataSize);

    return true;
}

bool COObjectDict::Rec::write(const void* data, OD_size_t dataSize)
{
    assert(m_object != nullptr);

    if(data == nullptr) return false;
    if(dataSize == 0) return false;

    R_M;

    if(rec->dataOrig == nullptr) return false;
    if(rec->dataLength != dataSize) return false;

    memcpy(rec->dataOrig, data, dataSize);

    return true;
}

