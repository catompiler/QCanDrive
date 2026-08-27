#ifndef TIMEFORMATTER_H
#define TIMEFORMATTER_H

#include "unitformatter.h"

class TimeFormatter : public UnitFormatter
{
public:
    TimeFormatter();
    ~TimeFormatter();

    //! Форматирует значение. Полиморфный метод.
    virtual QString format(qreal value, char format = 'g', int precision = 6) override;

    //! Форматирует значение. Статический метод.
    static QString formatValue(qreal value, char format = 'g', int precision = 6);
};

#endif // TIMEFORMATTER_H
