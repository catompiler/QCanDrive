#ifndef REGLISTEXPORTER_H
#define REGLISTEXPORTER_H

#include <QObject>
#include <QString>
#include "reglistmodel.h"
#include "regutils.h"



class RegListExporter : public QObject
{
    Q_OBJECT
public:
    explicit RegListExporter(QObject *parent = nullptr);
    virtual ~RegListExporter();

    virtual bool doExport(const QString& filename, const RegEntryList* reglist) = 0;

    RegListExporter& setDataName(const QString& dataName);
    RegListExporter& setEntryNameMap(const RegUtils::EntryNameMap* entryNameMap);
    RegListExporter& setVarNameMap(const RegUtils::VarNameMap* varNameMap);
    RegListExporter& setSyntaxType(RegUtils::SyntaxType newSyntaxType);

protected:
    QString m_dataName;
    RegUtils::SyntaxType m_syntaxType;
    const RegUtils::EntryNameMap* m_entryNameMap;
    const RegUtils::VarNameMap* m_varNameMap;

};

#endif // REGLISTEXPORTER_H
