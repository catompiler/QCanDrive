#include "timeformatter.h"
#include <QObject>



TimeFormatter::TimeFormatter()
    :UnitFormatter()
{
}

TimeFormatter::~TimeFormatter()
{
}

QString TimeFormatter::format(qreal value, char format, int precision)
{
    return TimeFormatter::formatValue(value, format, precision);
}

QString TimeFormatter::formatValue(qreal value, char format, int precision)
{
    static const UnitFormatter::UnitRules unitRules = {
        {0, QObject::tr("с"), 0}, // Правило для нулевого значения.
        {1e-6, QObject::tr("мкс"), 1e6},
        {1e-3, QObject::tr("мс"),  1e3},
        {1e0,  QObject::tr("с"),   1e0},
    };

    return UnitFormatter::formatValueByRules(unitRules, value, format, precision);
}