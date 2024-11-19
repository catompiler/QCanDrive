#ifndef COOBJECTDICT_H
#define COOBJECTDICT_H

#include <QObject>
#include "CANopen.h"
#include <QVector>
#include <stdint.h>
#include <stddef.h>



class COObjectDict : public QObject
{
    Q_OBJECT
public:

    using Index = uint16_t;
    using SubIndex = uint8_t;
    using ObjectPtr = void*;

    enum ObjectType {
        NO_OBJ = 0,
        VAR    = 1,
        ARR    = 2,
        REC    = 3
    };

    enum DataType {
        NO_DATA = 0,
        I8      = 1,
        I16     = 2,
        I32     = 3,
        U8      = 4,
        U16     = 5,
        U32     = 6
    };

    class Object {
    public:
        Object();
        explicit Object(ObjectPtr ptr);
        Object(const Object& obj) = delete;
        Object(Object&& obj);
        ~Object();

        static ObjectPtr createODObj(ObjectType objType);
        static void destroyODObj(ObjectType objType, ObjectPtr ptr);

    protected:
        ObjectPtr m_object;
    };

    class Var: public Object
    {
    public:
        Var();
        explicit Var(ObjectPtr ptr);
        Var(const Var& var) = delete;
        Var(Var&& var);
        ~Var();

        static ObjectPtr createODObj();
        static void destroyODObj(ObjectPtr ptr);

        void* dataOrig();
        const void* dataOrig() const;

        OD_attr_t attribute() const;
        void setAttribute(OD_attr_t newAttribute);

        OD_size_t dataLength() const;
        void setDataLength(OD_size_t newDataLength);

        bool read(void* data, OD_size_t dataSize) const;
        bool write(const void* data, OD_size_t dataSize);
    };

    class Arr: public Object
    {
    public:
        Arr();
        explicit Arr(ObjectPtr ptr);
        Arr(const Arr& arr) = delete;
        Arr(Arr&& arr);
        ~Arr();

        static ObjectPtr createODObj();
        static void destroyODObj(ObjectPtr ptr);

        void* dataOrig0();
        const void* dataOrig0() const;

        void* dataOrig();
        const void* dataOrig() const;

        OD_attr_t attribute0() const;
        void setAttribute0(OD_attr_t newAttribute);

        OD_attr_t attribute() const;
        void setAttribute(OD_attr_t newAttribute);

        OD_size_t dataElementLength() const;
        void setDataElementLength(OD_size_t newDataElementLength);

        OD_size_t dataElementSizeof() const;
        void setDataElementSizeof(OD_size_t newDataElementSizeof);

        bool read(void* data, OD_size_t dataSize, int subEntry) const;
        bool write(const void* data, OD_size_t dataSize, int subEntry);
    };

    class Rec: public Object
    {
    public:
        Rec();
        explicit Rec(ObjectPtr ptr);
        explicit Rec(ObjectPtr ptr, OD_size_t subEntry);
        Rec(const Rec& rec) = delete;
        Rec(Rec&& rec);
        ~Rec();

        static ObjectPtr createODObj(OD_size_t count);
        static ObjectPtr resizeODObj(ObjectPtr ptr, OD_size_t oldCount, OD_size_t newCount);
        static void destroyODObj(ObjectPtr ptr, OD_size_t count);

        void* dataOrig();
        const void* dataOrig() const;

        SubIndex subIndex() const;
        void setSubIndex(SubIndex newSubIndex);

        OD_attr_t attribute() const;
        void setAttribute(OD_attr_t newAttribute);

        OD_size_t dataLength() const;
        void setDataLength(OD_size_t newDataLength);

        bool read(void* data, OD_size_t dataSize) const;
        bool write(const void* data, OD_size_t dataSize);
    };

    class Entry{
    public:
        explicit Entry(OD_entry_t* entry = nullptr);
        Entry(const Entry& entry);
        Entry(Entry&& entry);
        ~Entry();

        bool isValid() const;
        bool isEmpty() const;

        int subEntry(SubIndex subIndex) const;

        OD_entry_t* odEntry();
        const OD_entry_t* odEntry() const;

        SubIndex subIndex(int subEntry = 0) const;
        bool setSubIndex(SubIndex subIndex, int subEntry = 0);

        Index index() const;
        void setIndex(Index newIndex);

        ObjectType objType() const;
        void setObjType(ObjectType newObjType);

        OD_extension_t* extension() const;
        void setExtension(OD_extension_t* newExtension);

        OD_size_t subEntriesCount() const;
        bool setSubEntriesCount(OD_size_t count);

        OD_size_t dataLength(int subEntry = 0) const;
        bool setDataLength(OD_size_t newDataLength, int subEntry = 0);

        OD_attr_t attribute(int subEntry = 0) const;
        bool setAttribute(OD_attr_t newAttr, int subEntry = 0);

        bool read(void* data, OD_size_t dataSize, int subEntry = 0) const;
        bool write(const void* data, OD_size_t dataSize, int subEntry = 0);

        template <typename T>
        bool read(T* data, int subEntry = 0) const;
        template <typename T>
        bool write(const T* data, int subEntry = 0);

        template <typename T>
        T read(int subEntry = 0, const T& defval = T(), bool* is_ok = nullptr) const;
        template <typename T>
        bool write(const T& data, int subEntry = 0);

        static OD_entry_t* createODEntry();
        static void destroyODEntry(OD_entry_t* entry);
        static void initODEntry(OD_entry_t* entry);
        static void resetODEntry(OD_entry_t* entry);
        static void freeODEntryData(OD_entry_t* entry);

        Entry& operator=(const Entry& entry);
        Entry& operator=(Entry&& entry);

    private:
        OD_entry_t* m_entry;
    };

    explicit COObjectDict(QObject *parent = nullptr);
    ~COObjectDict();

    void clear();
    void make();

    OD_t* od();
    CO_config_t* config();

    Entry addEntry();
    void removeEntry(int i);
    Entry entryAt(int i);
    Entry entryByIndex(Index entryIndex);

    Entry add_H1000_DevType();
    Entry add_H1001_ErrReg();
    Entry add_H1002_ManufStatusReg();
    Entry add_H1003_PredefErrField();
    //Entry add_H1004_Rsv();
    Entry add_H1005_CobidSync();
    Entry add_H1006_CommCyclPeriod();
    Entry add_H1007_SyncWindowLen();
    //Entry add_H1008_ManufDevName(); // VISIBLE_STRING
    //Entry add_H1009_ManufHwVersion(); // VISIBLE_STRING
    //Entry add_H100A_ManufSwVersion(); // VISIBLE_STRING
    //Entry add_H100B_Rsv();
    Entry add_H100C_GuardTime();
    Entry add_H100D_LifetimeFactor();
    //Entry add_H100E_Rsv();
    //Entry add_H100F_Rsv();
    Entry add_H1010_StoreParameters();
    Entry add_H1011_RestoreDefault();
    Entry add_H1012_CobidTime();
    Entry add_H1013_HighResTimestamp();
    Entry add_H1014_CobidEmergency();
    Entry add_H1015_InhibitTimeEmcy();
    Entry add_H1016_ConsumerHbTime();
    Entry add_H1017_ProducerHbTime();
    Entry add_H1018_IdentityObject();
    Entry add_H1019_SyncCntOverflow();
    Entry add_H1020_VerifyConfig();
    //Entry add_H1021_StoreEds(); // DOMAIN
    Entry add_H1022_StoreFormat();
    //Entry add_H1023_OsCmd(); // DOMAIN
    Entry add_H1024_OsCmdMode();
    //Entry add_H1025_OsDbgInterface(); // DOMAIN
    Entry add_H1026_OsPrompt();
    Entry add_H1027_ModuleList();
    Entry add_H1028_EmcyConsumer();
    Entry add_H1029_ErrBehavior();
    Entry add_H1200_SdoServer1Param();
    Entry add_H1201_SdoServer1Param();
    Entry add_H1280_SdoClient1Param();
    //Entry add_H1300_GfcParam(); // Not in CON EDS Editor.
    //Entry add_H1301_Srdo1Param(); // Not in CON EDS Editor.
    //Entry add_H1381_Srdo1Mapping(); // Not in CON EDS Editor.
    //Entry add_H13FE_SrdoValid(); // Not in CON EDS Editor.
    //Entry add_H13FF_SrdoChecksum(); // Not in CON EDS Editor.
    Entry add_H1400_Rxpdo1Param();
    Entry add_H1401_Rxpdo1Param();
    Entry add_H1402_Rxpdo1Param();
    Entry add_H1403_Rxpdo1Param();
    //Entry add_H1404_Rxpdo1Param();
    Entry add_H1600_Rxpdo1Mapping();
    Entry add_H1601_Rxpdo1Mapping();
    Entry add_H1602_Rxpdo1Mapping();
    Entry add_H1603_Rxpdo1Mapping();
    //Entry add_H1604_Rxpdo1Mapping();
    Entry add_H1800_Txpdo1Param();
    Entry add_H1801_Txpdo1Param();
    Entry add_H1802_Txpdo1Param();
    Entry add_H1803_Txpdo1Param();
    //Entry add_H1804_Txpdo1Param();
    Entry add_H1A00_Txpdo1Mapping();
    Entry add_H1A01_Txpdo1Mapping();
    Entry add_H1A02_Txpdo1Mapping();
    Entry add_H1A03_Txpdo1Mapping();
    //Entry add_H1A04_Txpdo1Mapping();
    //Entry add_H1_();


signals:

private:
    QVector<OD_entry_t> m_od_entries;
    OD_t m_od;
    CO_config_t m_config;

    uint m_cntNmt;
    uint m_cntEm;
    uint m_cntSync;
    uint m_cntSyncProd;
    uint m_cntStorage;
    uint m_cntTime;
    uint m_cntEmProd;
    uint m_cntHbCons;
    uint m_cntHbProd;
    uint m_cntSdoSrv;
    uint m_cntSdoCli;
    uint m_cntRpdo;
    uint m_cntTpdo;

    void updateOd();
    void resetConfig();
    OD_entry_t* odEntryByIndex(Index entryIndex);
    uint8_t arraySizeByIndex(Index entryIndex);
    void removeEmptyEntries();
    void fixEntriesSorting();
};


template <typename T>
bool COObjectDict::Entry::read(T* data, int subEntry) const
{
    return read(data, sizeof(T), subEntry);
}

template <typename T>
bool COObjectDict::Entry::write(const T* data, int subEntry)
{
    return write(data, sizeof(T), subEntry);
}

template <typename T>
T COObjectDict::Entry::read(int subEntry, const T& defval, bool* is_ok) const
{
    T res;

    if(read(&res, subEntry)){
        if(is_ok) *is_ok = true;
        return res;
    }

    if(is_ok) *is_ok = false;
    return defval;
}

template <typename T>
bool COObjectDict::Entry::write(const T& data, int subEntry)
{
    if(write(&data, subEntry)){
        return true;
    }

    return false;
}

#endif // COOBJECTDICT_H
