#ifndef REGLISTREGSEXPORTER_H
#define REGLISTREGSEXPORTER_H

#include "reglistexporter.h"
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>


class RegListRegsExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListRegsExporter(QObject *parent = nullptr);
    ~RegListRegsExporter();

    // RegListExporter interface
public:
    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;

private:
    struct Reg {
        QString id_name;
        DataType datatype;
        uint count;
        QString data;
        reg_flags_t flags;
        uint base_id;
        QString defval;
        QString comment;
    };
    typedef QMap<uint, Reg> RegList;

    struct RegDataField {
        QString datatype;
        uint count;
        QString name;
        QString defval;
    };

    struct RegData {
        QString name;
        QList<RegDataField> fields;
    };

    RegList genRegList(const RegEntryList* entrylist) const;
    void fixEqualNames(RegList& reglist) const;
    RegData genAndFixRegData(RegList& reglist, const QString& name) const;

    bool exportRegList(const QString& filename, const RegList& reglist) const;
    bool exportRegIds(const QString& filename, const RegList& reglist) const;
    bool exportRegData(const QString& filename, const RegData& regdata) const;
    bool exportRegDataDecl(const QString& filename, const RegData& regdata) const;

    QString regDataVarTypeStr(DataType type) const;
    uint makeRegId(reg_index_t index, reg_subindex_t subindex) const;
    QString makeRegName(const QString& group_name, const QString& reg_name) const;
    QString makeRegIdName(const QString& reg_name) const;
    QString dataTypeToStr(DataType type) const;
    QString flagToStr(uint flag) const;
    QString flagsToStr(uint flags) const;
    QString idToStr(uint id) const;

    QString memName(const QString& mem, const QString& group, const QString& var) const;
};

#endif // REGLISTREGSEXPORTER_H
