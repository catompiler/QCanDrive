#ifndef REGVAR_H
#define REGVAR_H

#include <QVariant>
#include "regobject.h"

class RegEntry;


class RegVar : public RegObject
{
public:
    explicit RegVar(RegEntry* parent = nullptr);
    RegVar(const RegVar& var);
    ~RegVar();

    ObjectType type() const override;

    // Установка родителя.
    void setParent(RegEntry* newParent);

    reg_subindex_t subIndex() const;
    void setSubIndex(reg_subindex_t newSubIndex);

    DataType dataType() const;
    void setDataType(DataType newDataType);

    QVariant minValue() const;
    void setMinValue(const QVariant& newMinValue);

    QVariant maxValue() const;
    void setMaxValue(const QVariant& newMaxValue);

    QVariant defaultValue() const;
    void setDefaultValue(const QVariant& newDefaultValue);

    reg_flags_t flags() const;
    void setFlags(reg_flags_t newFlags);

    reg_eflags_t eflags() const;
    void setEFlags(reg_eflags_t newEflags);

    reg_index_t baseIndex() const;
    void setBaseIndex(reg_index_t newBaseIndex);

    reg_subindex_t baseSubIndex() const;
    void setBaseSubIndex(reg_subindex_t newBaseSubIndex);

    unsigned int count() const;
    void setCount(unsigned int newCount);

private:
    reg_subindex_t m_subIndex;
    reg_index_t m_baseIndex;
    reg_subindex_t m_baseSubIndex;
    unsigned int m_count;
    DataType m_dataType;
    QVariant m_minValue;
    QVariant m_maxValue;
    QVariant m_defaultValue;
    reg_flags_t m_flags;
    reg_eflags_t m_eflags;
};

#endif // REGVAR_H
