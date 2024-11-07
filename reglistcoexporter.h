#ifndef REGLISTCOEXPORTER_H
#define REGLISTCOEXPORTER_H

#include "reglistexporter.h"

class QTextStream;


class RegListCoExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListCoExporter(QObject *parent = nullptr);
    ~RegListCoExporter();

    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;

    RegListCoExporter& setCOhFileName(const QString& fileName);
    RegListCoExporter& setCOcFileName(const QString& fileName);

private:
    QString m_cohFileName;
    QString m_cocFileName;

    bool exportCoH(const QString& filename, const RegEntryList* regentrylist);
    bool writeCOCounters(QTextStream& out_stream, const RegEntryList* regentrylist);
    bool writeCOArraySizes(QTextStream& out_stream, const RegEntryList* regentrylist);
    bool writeCOexternOd(QTextStream& out_stream);
    bool writeCOShortcuts(QTextStream& out_stream, const RegEntryList* regentrylist);
    bool writeCOShortcutsWithNames(QTextStream& out_stream, const RegEntryList* regentrylist);

    bool exportCoC(const QString& filename, const RegEntryList* regentrylist);
    bool writeAllOdObjConstDefs(QTextStream& out_stream, const RegEntryList* regentrylist);
    bool writeOdObjConstDef(QTextStream& out_stream, const RegEntry* re);
    bool writeOdVarConstDef(QTextStream& out_stream, const RegEntry* re);
    bool writeOdRecConstDef(QTextStream& out_stream, const RegEntry* re);
    bool writeOdArrConstDef(QTextStream& out_stream, const RegEntry* re);
    bool writeOd(QTextStream& out_stream, const RegEntryList* regentrylist);

    QString getOdObjectTypeStr(ObjectType objType) const;
    QString getOdEntryTypeStr(ObjectType objType) const;
    QString getOdEntryFieldName(const RegEntry* re) const;
    QString getOdEntryFieldDecl(const RegEntry* re) const;
};

#endif // REGLISTCOEXPORTER_H
