#include "regvar.h"
#include <stdint.h>



RegVar::RegVar(RegEntry* parent)
    :RegObject(parent)
{
    m_subIndex = 0x0;
    m_baseIndex = 0x0;
    m_baseSubIndex = 0x0;
    m_count = 1;
    m_dataType = DataType::I32;
    m_minValue = INT32_MIN;
    m_maxValue = INT32_MAX;
    m_defaultValue = 0;
    m_flags = 0x0;
    m_eflags = 0x0;
}

RegVar::RegVar(const RegVar& var)
    :RegObject(var)
{
    m_subIndex = var.m_subIndex;
    m_baseIndex = var.m_baseIndex;
    m_baseSubIndex = var.m_baseSubIndex;
    m_count = var.m_count;
    m_dataType = var.m_dataType;
    m_minValue = var.m_minValue;
    m_maxValue = var.m_maxValue;
    m_defaultValue = var.m_defaultValue;
    m_flags = var.m_flags;
    m_eflags = var.m_eflags;
}

RegVar::~RegVar()
{

}


ObjectType RegVar::type() const
{
    return ObjectType::VAR;
}

void RegVar::setParent(RegEntry* newParent)
{
    RegObject::setParent(newParent);
}

reg_subindex_t RegVar::subIndex() const
{
    return m_subIndex;
}

void RegVar::setSubIndex(reg_subindex_t newSubIndex)
{
    m_subIndex = newSubIndex;
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

reg_flags_t RegVar::flags() const
{
    return m_flags;
}

void RegVar::setFlags(reg_flags_t newFlags)
{
    m_flags = newFlags;
}

reg_eflags_t RegVar::eflags() const
{
    return m_eflags;
}

void RegVar::setEFlags(reg_eflags_t newEflags)
{
    m_eflags = newEflags;
}

reg_index_t RegVar::baseIndex() const
{
    return m_baseIndex;
}

void RegVar::setBaseIndex(reg_index_t newBaseIndex)
{
    m_baseIndex = newBaseIndex;
}

reg_subindex_t RegVar::baseSubIndex() const
{
    return m_baseSubIndex;
}

void RegVar::setBaseSubIndex(reg_subindex_t newBaseSubIndex)
{
    m_baseSubIndex = newBaseSubIndex;
}

unsigned int RegVar::count() const
{
    return m_count;
}

void RegVar::setCount(unsigned int newCount)
{
    m_count = newCount;
}

QString RegVar::memAddr() const
{
    return m_memAddr;
}

QString RegVar::memAddr(unsigned int i) const
{
    return QStringLiteral("%1[%2]").arg(m_memAddr).arg(i);
}

void RegVar::setMemAddr(const QString& newMemAddr)
{
    m_memAddr = newMemAddr;
}

