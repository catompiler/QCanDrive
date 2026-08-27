#include "voltageformatter.h"
#include <QObject>



VoltageFormatter::VoltageFormatter()
    :UnitFormatter()
{
}

VoltageFormatter::~VoltageFormatter()
{
}

QString VoltageFormatter::format(qreal value, char format, int precision)
{
    return VoltageFormatter::formatValue(value, format, precision);
}

QString VoltageFormatter::formatValue(qreal value, char format, int precision)
{
    static const UnitFormatter::UnitRules unitRules = {
        {0, QObject::tr("В"), 0},
        {1e-6, QObject::tr("мкВ"), 1e6},
        {1e-3, QObject::tr("мВ"),  1e3},
        {1e0,  QObject::tr("В"),   1e0},
        {1e3,  QObject::tr("кВ"),  1e-3},
    };

    return UnitFormatter::formatValueByRules(unitRules, value, format, precision);
}
