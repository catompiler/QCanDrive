#ifndef VOLTAGEFORMATTER_H
#define VOLTAGEFORMATTER_H

#include "unitformatter.h"

class VoltageFormatter : public UnitFormatter
{
public:
    VoltageFormatter();
    ~VoltageFormatter();

    //! Форматирует значение. Полиморфный метод.
    virtual QString format(qreal value, char format = 'g', int precision = 6) override;

    //! Форматирует значение. Статический метод.
    static QString formatValue(qreal value, char format = 'g', int precision = 6);
};

#endif // VOLTAGEFORMATTER_H
