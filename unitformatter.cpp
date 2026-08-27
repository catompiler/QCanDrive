#include "unitformatter.h"
#include <algorithm>


UnitFormatter::UnitFormatter()
{
}

UnitFormatter::~UnitFormatter()
{
}

QString UnitFormatter::formatValueByRules(const UnitRules& unitRules, qreal value, char format, int precision)
{
    const UnitRule* rule = findRule(unitRules, value);

    if(rule == nullptr) return QString::number(value, format, precision);

    return QStringLiteral("%1 %2").arg(value * rule->scale, 0, format, precision).arg(rule->unit);
}

const UnitFormatter::UnitRule* UnitFormatter::findRule(const UnitRules& unitRules, qreal value)
{
    if(unitRules.isEmpty()) return nullptr;

    auto it = std::lower_bound(unitRules.begin(), unitRules.end(), qAbs(value), [](const UnitRule& rule, const qreal& val){
        return rule.threshold < val;
    });

    if(it == unitRules.end()) return &unitRules.last();
    if(it == unitRules.begin()) return &unitRules.first();

    std::advance(it, -1);

    return &(*it);
}
