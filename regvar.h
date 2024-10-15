#ifndef REGVAR_H
#define REGVAR_H

#include <QVariant>
#include "regobject.h"

class RegArray;
class RegRecord;


class RegVar : public RegObject
{
public:
    explicit RegVar(RegObject* parent = nullptr);
    RegVar(const RegVar& var);
    ~RegVar();

    ObjectType type() const override;

    // Установка родителя.
    void setParent(RegArray* newParent);
    void setParent(RegRecord* newParent);

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

private:
    reg_subindex_t m_subIndex;
    DataType m_dataType;
    QVariant m_minValue;
    QVariant m_maxValue;
    QVariant m_defaultValue;
    reg_flags_t m_flags;
};

#endif // REGVAR_H
