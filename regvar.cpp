#include "regvar.h"

RegVar::RegVar()
{

}

RegVar::~RegVar()
{

}


ObjectType RegVar::type() const
{
    return ObjectType::VAR;
}

DataType RegVar::dataType() const
{
    return m_dataType;
}

void RegVar::setDataType(DataType newDataType)
{
    m_dataType = newDataType;
}

QVariant RegVar::value() const
{
    return m_value;
}

void RegVar::setValue(const QVariant& newValue)
{
    m_value = newValue;
}
