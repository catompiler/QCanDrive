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

    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;

private:

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
