#ifndef FILTER1_H
#define FILTER1_H

#include <QtGlobal>


class Filter1
{
public:
    Filter1();
    ~Filter1();

    qreal T() const;
    void setT(qreal newT);

    qreal value() const;
    void setValue(qreal newValue);

    //! Фильтрует значение.
    //! Если dt == 0 - берётся время с прошлого вычисления.
    qreal filter(qreal newValue, qreal dt = 0.0);

    //! Вызывает filter(...).
    qreal operator()(qreal newValue, qreal dt = 0.0);

private:
    qreal m_T;
    qreal m_y;
    qint64 m_t_z1;
};

#endif // FILTER1_H
