#ifndef REGLISTDATAEXPORTER_H
#define REGLISTDATAEXPORTER_H

#include "reglistexporter.h"

class RegListDataExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListDataExporter(QObject *parent = nullptr);
    ~RegListDataExporter();

    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;

    RegListDataExporter& setDeclFileName(const QString& fileName);
    RegListDataExporter& setImplFileName(const QString& fileName);

private:
    QString m_declFileName;
    QString m_implFileName;

    bool exportRegDataDecl(const QString& filename, const RegEntryList* regentrylist);
    bool exportRegData(const QString& filename, const RegEntryList* regentrylist);

    QString makeDataStructName(const RegEntry* re) const;
    QString makeDataStructTypeName(const QString& name) const;
};

#endif // REGLISTDATAEXPORTER_H
