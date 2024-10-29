#ifndef REGLISTREGSEXPORTER_H
#define REGLISTREGSEXPORTER_H

#include "reglistexporter.h"
#include <QString>


class RegListRegsExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListRegsExporter(QObject *parent = nullptr);
    ~RegListRegsExporter();

    // RegListExporter interface
public:
    bool doExport(const QString& filename, const RegEntryList* reglist) override;

private:
    bool exportRegList(const QString& filename, const RegEntryList* reglist) const;
    bool exportRegIds(const QString& filename, const RegEntryList* reglist) const;

    QString memName(const QString& mem, const QString& group, const QString& var) const;
    QString typeToStr(uint type) const;
    QString flagsToStr(uint flags) const;
    QString indexSubIndexToStr(uint index, uint subindex) const;
};

#endif // REGLISTREGSEXPORTER_H
