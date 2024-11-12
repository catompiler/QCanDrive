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

    bool doExport(const QString& filepath, const RegEntryList* regentrylist) override;

    RegListRegsExporter& setListFileName(const QString& fileName);
    RegListRegsExporter& setIdsFileName(const QString& fileName);
    RegListRegsExporter& setDataFileName(const QString& fileName);
    RegListRegsExporter& setUserCodeIds(const QString& userCode);
    RegListRegsExporter& setUserCodeList(const QString& userCode);

private:
    QString m_listFileName;
    QString m_idsFileName;
    QString m_dataFileName;
    QString m_userCodeIds;
    QString m_userCodeList;

    QHash<reg_fullindex_t, QString> m_reg_id_names;

    bool exportRegIds(const QString& filename, const RegEntryList* regentrylist);
    bool exportRegList(const QString& filename, const RegEntryList* regentrylist);

    QString makeRegName(const RegEntry* re, const RegVar* rv, uint index = 0) const;
    QString makeRegIdName(const RegEntry* re, const RegVar* rv, uint index = 0) const;
    QString regDataTypeStr(DataType type) const;
    QString flagToStr(uint flag) const;
    QString flagsToStr(uint flags) const;
    QString idToStr(uint id) const;
};

#endif // REGLISTREGSEXPORTER_H
