#ifndef UNITFORMATTER_H
#define UNITFORMATTER_H

#include <QVector>
#include <QString>


class UnitFormatter
{
public:

    struct UnitRule {
        qreal threshold; //!< порог, с которого включаем эту размерность
        QString unit;    //!< строка единицы (уже локализованная)
        qreal scale;     //!< множитель для перевода
    };

    using UnitRules = QVector<UnitRule>;

    UnitFormatter();
    virtual ~UnitFormatter();

    //! Форматирует значение.
    virtual QString format(qreal value, char format = 'g', int precision = 6) = 0;

    //! Общая функция форматирования значения согласно правилам выбора приставки.
    static QString formatValueByRules(const UnitRules& unitRules, qreal value, char format = 'g', int precision = 6);

protected:
    static const UnitRule* findRule(const UnitRules& unitRules, qreal value);
};

#endif // UNITFORMATTER_H
