#include "reglistedsexporter.h"
#include "regentry.h"
#include "regvar.h"
#include "regtypes.h"
#include "regutils.h"
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <algorithm>


// https://microsin.net/programming/arm/cia306-electronic-data-sheet-specification-for-canopen.html


#define DEBUG_OUTPUT 0


#define begSect(NAME) beginSection(out_stream, (NAME))
#define outVal(KEY, ...) writeValue(out_stream, (KEY), ##__VA_ARGS__)
#define endSect() endSection(out_stream)


RegListEdsExporter::RegListEdsExporter(QObject *parent)
    : RegListExporter{parent}
{
    m_granularity = 8;
    m_bauds = COBaud::B125;
}

RegListEdsExporter::~RegListEdsExporter()
{
}

bool RegListEdsExporter::doExport(const QString& filepath, const RegEntryList* reglist)
{
    QDir path(filepath);
    if(!path.exists()) return false;

    if(m_edsFileName.isEmpty()) m_edsFileName = "reg_list.eds";

    QString edsFileName = QFileInfo(m_edsFileName).isAbsolute() ? m_edsFileName : path.filePath(m_edsFileName);

    QFile file(edsFileName);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QTextStream out_stream(&file);

//#if defined(DEBUG_OUTPUT) && DEBUG_OUTPUT == 1
//    QDebug out = qDebug();
//#else
//    QTextStream& out = out_stream;
//#endif

    if(!writeFileInfo(out_stream)) return false;
    if(!writeDeviceInfo(out_stream, reglist)) return false;
    if(!writeDummyUsage(out_stream, reglist)) return false;
    if(!writeComments(out_stream)) return false;
    if(!writeMandatoryObjects(out_stream, reglist)) return false;
    if(!writeOptionalObjects(out_stream, reglist)) return false;
    if(!writeManufacturerObjects(out_stream, reglist)) return false;

    file.close();

    return true;
}

RegListEdsExporter& RegListEdsExporter::setEdsFileName(const QString& filename)
{
    m_edsFileName = filename;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setFileVersion(int ver, int rev)
{
    m_fileVersion = ver;
    m_fileRevision = rev;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setFileDescription(const QString& descr)
{
    m_fileDescr = descr;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setFileAuthor(const QString& author)
{
    m_fileAuthor = author;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setVendorName(const QString& vendorName)
{
    m_vendorName = vendorName;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setProductName(const QString& productName)
{
    m_productName = productName;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setOrderCode(const QString& orderCode)
{
    m_orderCode = orderCode;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setSupportedBauds(co_bauds_t bauds)
{
    m_bauds = bauds;

    return *this;
}

RegListEdsExporter& RegListEdsExporter::setGranularity(int granularity)
{
    m_granularity = granularity;

    return *this;
}

bool RegListEdsExporter::writeFileInfo(QTextStream& out_stream)
{
    QDateTime curDT = QDateTime::currentDateTime();

    bool isAM = true;
    int hour = 0;
    int min = 0;

    QTime time = curDT.time();
    if(time.hour() == 0 || time.hour() < 12){
        isAM = true;
        if(time.hour() == 0) hour = 12;
        else hour = time.hour();
    }else{
        isAM = false;
        if(time.hour() == 12) hour = 12;
        else hour = time.hour() - 12;
    }
    min = time.minute();

    QString time_str = QStringLiteral("%1:%2%3").arg(hour).arg(min)
                       .arg(isAM ? QStringLiteral("am") : QStringLiteral("pm")
                       );
    QString date_str = curDT.toString("MM-dd-yyyy");

    begSect("FileInfo");
    outVal("FileName", QFileInfo(m_edsFileName).fileName());
    outVal("FileVersion", m_fileVersion);
    outVal("FileRevision", m_fileRevision);
    outVal("EDSVersion", "4.0");
    outVal("Description", m_fileDescr);
    outVal("CreationTime", time_str);
    outVal("CreationDate", date_str);
    outVal("CreatedBy", m_fileAuthor);
    outVal("ModificationTime", time_str);
    outVal("ModificationDate", date_str);
    outVal("ModifiedBy", m_fileAuthor);
    endSect();

    return true;
}

bool RegListEdsExporter::writeDeviceInfo(QTextStream& out_stream, const RegEntryList* reglist)
{
    QStringList bauds = {
        QStringLiteral("BaudRate_10"),
        QStringLiteral("BaudRate_20"),
        QStringLiteral("BaudRate_50"),
        QStringLiteral("BaudRate_125"),
        QStringLiteral("BaudRate_250"),
        QStringLiteral("BaudRate_500"),
        QStringLiteral("BaudRate_800"),
        QStringLiteral("BaudRate_1000"),
    };

    bool hasVendorNumber = false; int vendorNumber = 0;
    bool hasProductNumber = false; int productNumber = 0;
    bool hasRevisionNumber = false; int revisionNumber = 0;

    auto ident_re_it = std::find_if(reglist->begin(), reglist->end(), [](const RegEntry* re){
        return re->index() == 0x1018;
    });

    if(ident_re_it != reglist->end()){
        RegEntry* re = *ident_re_it;
        RegVar* rvVn = re->varBySubIndex(0x1); if(rvVn) vendorNumber = rvVn->defaultValue().toInt(&hasVendorNumber);
        RegVar* rvPn = re->varBySubIndex(0x2); if(rvPn) productNumber = rvPn->defaultValue().toInt(&hasProductNumber);
        RegVar* rvRn = re->varBySubIndex(0x3); if(rvRn) revisionNumber = rvRn->defaultValue().toInt(&hasRevisionNumber);
    }

    int nRxPdo = 0;
    int nTxPdo = 0;

    nRxPdo = std::count_if(reglist->begin(), reglist->end(), [](const RegEntry* re){
        return std::any_of(re->begin(), re->end(), [](const RegVar* rv){
            return (rv->eflags() & RegEFlag::CON_CNT_RPDO) != 0;
        });
    });

    nTxPdo = std::count_if(reglist->begin(), reglist->end(), [](const RegEntry* re){
        return std::any_of(re->begin(), re->end(), [](const RegVar* rv){
            return (rv->eflags() & RegEFlag::CON_CNT_TPDO) != 0;
        });
    });

    begSect("DeviceInfo");
    outVal("VendorName", m_vendorName);
    outVal("VendorNumber", (hasVendorNumber ? QVariant(vendorNumber) : QVariant()));
    outVal("ProductName", m_productName);
    outVal("ProductNumber", (hasProductNumber ? QVariant(productNumber) : QVariant()));
    outVal("RevisionNumber", (hasRevisionNumber ? QVariant(revisionNumber) : QVariant()));
    outVal("OrderCode", m_orderCode);
    for(int i = 0; i < bauds.size(); i ++){
        outVal(bauds.at(i), (m_bauds & (1<<i)) ? 1 : 0);
    }
    outVal("SimpleBootUpMaster", 0);
    outVal("SimpleBootUpSlave", 0);
    outVal("Granularity", m_granularity);
    outVal("DynamicChannelsSupported", 0);
    outVal("GroupMessaging", 0);
    outVal("NrOfRXPDO", nRxPdo);
    outVal("NrOfTXPDO", nTxPdo);
    outVal("LSS_Supported", 0);
    endSect();

    return true;
}

bool RegListEdsExporter::writeDummyUsage(QTextStream& out_stream, const RegEntryList* reglist)
{
    Q_UNUSED(reglist);

    begSect("DummyUsage");
    for(int i = 1; i < 8; i ++){
        outVal(QStringLiteral("Dummy%1").arg(i, 4, 16, QChar('0')), 0);
    }
    endSect();

    return true;
}

bool RegListEdsExporter::writeComments(QTextStream& out_stream)
{
    begSect("Comments");
    outVal("Lines", 0);
    endSect();

    return true;
}

bool RegListEdsExporter::writeMandatoryObjects(QTextStream& out_stream, const RegEntryList* reglist)
{
    return writeSupportedObjects(out_stream, "MandatoryObjects", {0x1000, 0x1001, 0x1018}, reglist);
}

bool RegListEdsExporter::writeOptionalObjects(QTextStream& out_stream, const RegEntryList* reglist)
{
    return writeSupportedObjects(out_stream, "OptionalObjects", {{0x1002, 0x1017}, {0x1019, 0x1FFF}, {0x6000, 0xFFFF}}, reglist);
}

bool RegListEdsExporter::writeManufacturerObjects(QTextStream& out_stream, const RegEntryList* reglist)
{
    return writeSupportedObjects(out_stream, "ManufacturerObjects", QVector<QPair<int, int>>{{0x2000, 0x5FFF}}, reglist);
}

bool RegListEdsExporter::writeSupportedObjects(QTextStream& out_stream, const QString& sect_name, const QVector<int>& ids, const RegEntryList* reglist)
{
    QVector<bool> supported(ids.count(), false);

    begSect(sect_name);

    std::transform(ids.begin(), ids.end(), supported.begin(), [&reglist](const int& id) -> bool{
        auto reit = std::find_if(reglist->begin(), reglist->end(), [&id](const RegEntry* re){
            return re->index() == id;
        });

        return reit != reglist->end();
    });

    outVal("SupportedObjects", supported.count(true));

    for(int i = 0; i < ids.count(); i ++){
        if(supported[i]){
            outVal(QString::number(i + 1), QStringLiteral("0x%1").arg(QStringLiteral("%1").arg(ids[i], 4, 16, QChar('0')).toUpper()));
        }
    }

    endSect();

    for(const auto& id: ids){
        auto reit = std::find_if(reglist->begin(), reglist->end(), [&id](const RegEntry* re){
            return re->index() == id;
        });

        if(reit == reglist->end()) continue;

        writeObject(out_stream, *reit);
    }

    return true;
}

bool RegListEdsExporter::writeSupportedObjects(QTextStream& out_stream, const QString& sect_name, const QVector<QPair<int, int> >& ids, const RegEntryList* reglist)
{
    auto inRange = [&ids](reg_index_t index) -> bool{
        for(const auto& id_range: ids){
            if(index >= id_range.first && index <= id_range.second) return true;
        }
        return false;
    };

    begSect(sect_name);

    int nSupp = std::count_if(reglist->begin(), reglist->end(), [&inRange](const RegEntry* re){
        return inRange(re->index());
    });

    outVal("SupportedObjects", nSupp);

    int i = 1;
    std::for_each(reglist->begin(), reglist->end(), [&inRange, &i, this, &out_stream](const RegEntry* re){
        if(inRange(re->index())){
            outVal(QString::number(i ++), QStringLiteral("0x%1").arg(QStringLiteral("%1").arg(static_cast<uint>(re->index()), 4, 16, QChar('0')).toUpper()));
        }
    });

    endSect();

    std::for_each(reglist->begin(), reglist->end(), [&inRange, this, &out_stream](const RegEntry* re){
        if(inRange(re->index())){
            writeObject(out_stream, re);
        }
    });

    return true;
}

bool RegListEdsExporter::writeObject(QTextStream& out_stream, const RegEntry* re)
{
    switch(re->type()){
    case ObjectType::VAR:
        return writeObjectVar(out_stream, re);
    case ObjectType::ARR:
        return writeObjectArr(out_stream, re);
    case ObjectType::REC:
        return writeObjectRec(out_stream, re);
    }

    return false;
}

bool RegListEdsExporter::writeObjectVar(QTextStream& out_stream, const RegEntry* re)
{
    if(re->count() == 0) return false;

    if(re->count() != 1){
        qDebug() << "writing only first VAR sub-entry";
    }

    const RegVar* rv = re->at(0);
    if(rv == nullptr) return false;

    if(!writeObjectVarDataSect(out_stream, re, rv)) return false;

    return true;
}

bool RegListEdsExporter::writeObjectVarDataSect(QTextStream& out_stream, const RegEntry* re, const RegVar* rv, int subIndex)
{
    if(re->type() == ObjectType::VAR){
        begSect(QString::number(static_cast<uint>(re->index()), 16).toUpper());
        outVal("ParameterName", re->name());
    }else{
        if(subIndex < 0) subIndex = rv->subIndex();
        begSect(QStringLiteral("%1sub%2").arg(
                    QString::number(static_cast<uint>(re->index()), 16).toUpper(),
                    QString::number(subIndex, 16).toUpper())
                );
        outVal("ParameterName", rv->name());
    }
    outVal("ObjectType", QStringLiteral("0x%1").arg(QString::number(RegTypes::coObjectType(rv->type()), 16)));
    outVal("DataType", QStringLiteral("0x%1").arg(QString::number(RegTypes::coDataType(rv->dataType()), 16)));
    outVal("AccessType", getAccessTypeStr(rv));
    outVal("DefaultValue", rv->defaultValue().toString());
    outVal("PDOMapping", getPDOMappingStr(rv));
    outVal("LowLimit", rv->minValue().toString());
    outVal("HighLimit", rv->maxValue().toString());
    endSect();

    return true;
}

bool RegListEdsExporter::writeObjectArr(QTextStream& out_stream, const RegEntry* re)
{
    if(re->count() == 0) return false;

    writeObjectArrRecDataSect(out_stream, re);

    bool res = true;
    uint sub_index = 0;
    std::for_each(re->begin(), re->end(), [&res, &re, &sub_index, this, &out_stream](const RegVar* rv){
        for(uint i = 0; i < rv->count(); i ++){
            if(!writeObjectVarDataSect(out_stream, re, rv, sub_index)) res = false;
            sub_index ++;
        }
    });

    return res;
}

bool RegListEdsExporter::writeObjectRec(QTextStream& out_stream, const RegEntry* re)
{
    if(re->count() == 0) return false;

    writeObjectArrRecDataSect(out_stream, re);

    bool res = true;
    std::for_each(re->begin(), re->end(), [&res, &re, this, &out_stream](const RegVar* rv){
        if(!writeObjectVarDataSect(out_stream, re, rv)) res = false;
    });

    return res;
}

bool RegListEdsExporter::writeObjectArrRecDataSect(QTextStream& out_stream, const RegEntry* re)
{
    begSect(QString::number(static_cast<uint>(re->index()), 16).toUpper());
    outVal("ParameterName", re->name());
    outVal("ObjectType", QStringLiteral("0x%1").arg(RegTypes::coObjectType(re->type()), 0, 16));
    outVal("SubNumber", QStringLiteral("0x%1").arg(re->countAll(), 0, 16));
    endSect();

    return true;
}

QString RegListEdsExporter::getAccessTypeStr(const RegVar* rv) const
{
    co_attributes_t attrs = RegTypes::eflagsToCoAttributes(rv->eflags());

    if(attrs & (COAttribute::SDO_RW | COAttribute::TRPDO)){
        return QStringLiteral("rw");
    }else if(attrs & (COAttribute::SDO_W | COAttribute::RPDO)){
        return QStringLiteral("wo");
    }/*else if(attrs & (COAttribute::SDO_R | COAttribute::RPDO)){
        return QStringLiteral("ro");
    }*/

    return QStringLiteral("ro");
}

QString RegListEdsExporter::getPDOMappingStr(const RegVar* rv) const
{
    reg_eflags_t eflags = rv->eflags();

    if((eflags & (RegEFlag::CO_TPDO | RegEFlag::CO_RPDO)) != 0){
        return QStringLiteral("1");
    }

    return QStringLiteral("0");
}

void RegListEdsExporter::beginSection(QTextStream& out_stream, const QString& name)
{
#if defined(DEBUG_OUTPUT) && DEBUG_OUTPUT == 1
    Q_UNUSED(out_stream)
    QDebug out = qDebug();
#else
    QTextStream& out = out_stream;
#endif

    out << "[" << name << "]\n";
}

void RegListEdsExporter::endSection(QTextStream& out_stream)
{
#if defined(DEBUG_OUTPUT) && DEBUG_OUTPUT == 1
    Q_UNUSED(out_stream)
    QDebug out = qDebug();
#else
    QTextStream& out = out_stream;
#endif

    out << "\n";
}

void RegListEdsExporter::writeValue(QTextStream& out_stream, const QString& key, const QVariant& value)
{
#if defined(DEBUG_OUTPUT) && DEBUG_OUTPUT == 1
    Q_UNUSED(out_stream)
    QDebug out = qDebug();
#else
    QTextStream& out = out_stream;
#endif

    out << key << "=" << value.toString() << "\n";
}
