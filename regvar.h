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

    QVariant minValue() const;
    void setMinValue(const QVariant& newMinValue);

    QVariant maxValue() const;
    void setMaxValue(const QVariant& newMaxValue);

    QVariant defaultValue() const;
    void setDefaultValue(const QVariant& newDefaultValue);

    bool isParameter() const;
    void setIsParameter(bool newIsParameter);

private:
    DataType m_dataType;
    QVariant m_minValue;
    QVariant m_maxValue;
    QVariant m_defaultValue;
    bool m_isParameter;
};

#endif // REGVAR_H
