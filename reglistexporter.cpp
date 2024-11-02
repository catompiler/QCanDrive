#include "reglistexporter.h"

RegListExporter::RegListExporter(QObject *parent)
    : QObject{parent}
{
}

RegListExporter::~RegListExporter()
{
}

RegListExporter& RegListExporter::setDataName(const QString& dataName)
{
    m_dataName = dataName;

    return *this;
}

RegListExporter& RegListExporter::setEntryNameMap(const RegUtils::EntryNameMap* entryNameMap)
{
    m_entryNameMap = entryNameMap;

    return *this;
}

RegListExporter& RegListExporter::setVarNameMap(const RegUtils::VarNameMap* varNameMap)
{
    m_varNameMap = varNameMap;

    return *this;
}
