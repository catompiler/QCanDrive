#ifndef REGLISTEDSEXPORTER_H
#define REGLISTEDSEXPORTER_H

#include "reglistexporter.h"
#include "regtypes.h"
#include <QVector>
#include <QPair>


class QTextStream;
class QVariant;


class RegListEdsExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListEdsExporter(QObject *parent = nullptr);
    ~RegListEdsExporter();

    bool doExport(const QString& filename, const RegEntryList* reglist) override;

    RegListEdsExporter& setEdsFileName(const QString& filename);
    RegListEdsExporter& setFileVersion(int ver, int rev);
    RegListEdsExporter& setFileDescription(const QString& descr);
    RegListEdsExporter& setFileAuthor(const QString& author);
    RegListEdsExporter& setVendorName(const QString& vendorName);
    RegListEdsExporter& setProductName(const QString& productName);
    RegListEdsExporter& setOrderCode(const QString& orderCode);
    RegListEdsExporter& setSupportedBauds(co_bauds_t bauds);
    RegListEdsExporter& setGranularity(int granularity);

private:
    QString m_edsFileName;
    int m_fileVersion;
    int m_fileRevision;
    QString m_fileDescr;
    QString m_fileAuthor;
    QString m_vendorName;
    QString m_productName;
    QString m_orderCode;
    co_bauds_t m_bauds;
    int m_granularity;

    bool writeFileInfo(QTextStream& out_stream);
    bool writeDeviceInfo(QTextStream& out_stream, const RegEntryList* reglist);
    bool writeDummyUsage(QTextStream& out_stream, const RegEntryList* reglist);
    bool writeComments(QTextStream& out_stream);
    bool writeMandatoryObjects(QTextStream& out_stream, const RegEntryList* reglist);
    bool writeOptionalObjects(QTextStream& out_stream, const RegEntryList* reglist);
    bool writeManufacturerObjects(QTextStream& out_stream, const RegEntryList* reglist);
    bool writeSupportedObjects(QTextStream& out_stream, const QString& sect_name, const QVector<int>& ids, const RegEntryList* reglist);
    bool writeSupportedObjects(QTextStream& out_stream, const QString& sect_name, const QVector<QPair<int, int>>& ids, const RegEntryList* reglist);

    bool writeObject(QTextStream& out_stream, const RegEntry* re);
    bool writeObjectVar(QTextStream& out_stream, const RegEntry* re);
    bool writeObjectVarDataSect(QTextStream& out_stream, const RegEntry* re, const RegVar* rv, int subIndex = -1);
    bool writeObjectArr(QTextStream& out_stream, const RegEntry* re);
    bool writeObjectRec(QTextStream& out_stream, const RegEntry* re);
    bool writeObjectArrRecDataSect(QTextStream& out_stream, const RegEntry* re);

    QString getAccessTypeStr(const RegVar* rv) const;
    QString getPDOMappingStr(const RegVar* rv) const;

    void beginSection(QTextStream& out_stream, const QString& name);
    void endSection(QTextStream& out_stream);
    void writeValue(QTextStream& out_stream, const QString& key, const QVariant& value = QVariant());
};

#endif // REGLISTEDSEXPORTER_H
