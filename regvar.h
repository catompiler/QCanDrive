#ifndef REGVAR_H
#define REGVAR_H

#include <QVariant>
#include "regobject.h"


class RegVar : public RegObject
{
public:
    RegVar();
    ~RegVar();

    // RegObject interface
public:
    ObjectType type() const override;

    DataType dataType() const;
    void setDataType(DataType newDataType);

    QVariant value() const;
    void setValue(const QVariant& newValue);

private:
    DataType m_dataType;
    QVariant m_value;
    QVariant m_minValue;
    QVariant m_maxValue;
    QVariant m_defaultValue;
};

#endif // REGVAR_H
