#include "regvar.h"
#include "regarray.h"
#include "regrecord.h"
#include <stdint.h>



RegVar::RegVar(RegObject* parent)
    :RegObject(parent)
{
    m_dataType = DataType::INTEGER32;
    m_minValue = INT32_MIN;
    m_maxValue = INT32_MAX;
    m_defaultValue = 0;
}

RegVar::RegVar(const RegVar& var)
    :RegObject(var)
{
    m_dataType = var.m_dataType;
    m_minValue = var.m_minValue;
    m_maxValue = var.m_maxValue;
    m_defaultValue = var.m_defaultValue;
}

RegVar::~RegVar()
{

}


ObjectType RegVar::type() const
{
    return ObjectType::VAR;
}

void RegVar::setParent(RegArray* newParent)
{
    RegObject::setParent(static_cast<RegObject*>(newParent));
}

void RegVar::setParent(RegRecord* newParent)
{
    RegObject::setParent(static_cast<RegObject*>(newParent));
}

DataType RegVar::dataType() const
{
    return m_dataType;
}

void RegVar::setDataType(DataType newDataType)
{
    m_dataType = newDataType;
}

QVariant RegVar::minValue() const
{
    return m_minValue;
}

void RegVar::setMinValue(const QVariant& newMinValue)
{
    m_minValue = newMinValue;
}

QVariant RegVar::maxValue() const
{
    return m_maxValue;
}

void RegVar::setMaxValue(const QVariant& newMaxValue)
{
    m_maxValue = newMaxValue;
}

QVariant RegVar::defaultValue() const
{
    return m_defaultValue;
}

void RegVar::setDefaultValue(const QVariant& newDefaultValue)
{
    m_defaultValue = newDefaultValue;
}

