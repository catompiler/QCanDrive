#ifndef REGLISTCOEXPORTER_H
#define REGLISTCOEXPORTER_H

#include "reglistexporter.h"

class RegListCoExporter : public RegListExporter
{
    Q_OBJECT
public:
    explicit RegListCoExporter(QObject *parent = nullptr);
    ~RegListCoExporter();

    bool doExport(const QString& filename, const RegEntryList* reglist) override;
};

#endif // REGLISTCOEXPORTER_H
