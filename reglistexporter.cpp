#include "reglistexporter.h"

RegListExporter::RegListExporter(QObject *parent)
    : QObject{parent}
{
    m_dataName = QStringLiteral("reg_List_Data");
    m_syntaxType = RegUtils::SyntaxType::camelCase;
    m_entryNameMap = nullptr;
    m_varNameMap = nullptr;
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

RegListExporter& RegListExporter::setSyntaxType(RegUtils::SyntaxType newSyntaxType)
{
    m_syntaxType = newSyntaxType;

    return *this;
}
