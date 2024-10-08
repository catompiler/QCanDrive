#include "regvar.h"
#include <stdint.h>


RegVar::RegVar()
{
    m_dataType = DataType::INTEGER32;
    m_minValue = INT32_MIN;
    m_maxValue = INT32_MAX;
    m_defaultValue = 0;
    m_isParameter = false;
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

bool RegVar::isParameter() const
{
    return m_isParameter;
}

void RegVar::setIsParameter(bool newIsParameter)
{
    m_isParameter = newIsParameter;
}
