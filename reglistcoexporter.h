#ifndef REGLISTCOEXPORTER_H
#define REGLISTCOEXPORTER_H

#include "reglistexporter.h"

class QFile;


class RegListCoExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListCoExporter(QObject *parent = nullptr);
    ~RegListCoExporter();

    bool doExport(const QString& filename, const RegEntryList* regentrylist) override;

private:
    bool exportCoH(const QString& filename, const RegEntryList* regentrylist);
    bool writeCOCounters(QFile& file, const RegEntryList* regentrylist);
    bool writeCOArraySizes(QFile& file, const RegEntryList* regentrylist);
    bool writeCOexternOd(QFile& file);
    bool writeCOShortcuts(QFile& file, const RegEntryList* regentrylist);
    bool writeCOShortcutsWithNames(QFile& file, const RegEntryList* regentrylist);
};

#endif // REGLISTCOEXPORTER_H
