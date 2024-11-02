#include "reglistcoexporter.h"

RegListCoExporter::RegListCoExporter(QObject *parent)
    : RegListExporter{parent}
{
}

RegListCoExporter::~RegListCoExporter()
{
}


bool RegListCoExporter::doExport(const QString& filename, const RegEntryList* reglist)
{
}
