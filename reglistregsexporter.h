#ifndef REGLISTREGSEXPORTER_H
#define REGLISTREGSEXPORTER_H

#include "reglistexporter.h"
#include "regutils.h"
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <QHash>


class RegListRegsExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListRegsExporter(QObject *parent = nullptr);
    ~RegListRegsExporter();

    // RegListExporter interface
public:
    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;
    bool doExport_old(const QString& filename, const RegEntryList* regentrylist, const RegUtils::VarNameMap* varNameMap = nullptr, const RegUtils::EntryNameMap* entryNameMap = nullptr) /*override*/;

    RegListRegsExporter& setNameMapping(RegUtils::NameMapping::Value mapType);
    RegListRegsExporter& setEntryNameMap(const RegUtils::EntryNameMap* entryNameMap);
    RegListRegsExporter& setVarNameMap(const RegUtils::VarNameMap* varNameMap);
private:
    RegUtils::NameMapping::Value m_mapType;
    const RegUtils::EntryNameMap* m_entryNameMap;
    const RegUtils::VarNameMap* m_varNameMap;

    QHash<reg_fullindex_t, QString> m_reg_id_names;
    QHash<reg_fullindex_t, QString> m_reg_data_names;

    bool exportRegIds(const QString& filename, const RegEntryList* regentrylist);
    bool exportRegList(const QString& filename, const RegEntryList* regentrylist);
    bool exportRegDataDecl(const QString& filename, const RegEntryList* regentrylist);
    bool exportRegData(const QString& filename, const RegEntryList* regentrylist);

    QString makeRegName(const RegEntry* re, const RegVar* rv) const;
    QString makeRegIdName(const RegEntry* re, const RegVar* rv) const;
    QString regDataTypeStr(DataType type) const;
    QString flagToStr(uint flag) const;
    QString flagsToStr(uint flags) const;
    QString idToStr(uint id) const;
    QString makeDataStructName(const QString& baseName, const RegEntry* re = nullptr) const;
    QString makeDataStructTypeName(const QString& name) const;
    QString memName(const QString& group, const QString& var) const;
};

#endif // REGLISTREGSEXPORTER_H
