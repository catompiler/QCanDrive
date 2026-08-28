#include "filter1.h"
#include <QDateTime>


Filter1::Filter1()
{
    m_T = 0.0;
    m_y = 0.0;
    m_t_z1 = 0;
}

Filter1::~Filter1()
{
}

qreal Filter1::T() const
{
    return m_T;
}

void Filter1::setT(qreal newT)
{
    m_T = newT;
}

qreal Filter1::value() const
{
    return m_y;
}

void Filter1::setValue(qreal newValue)
{
    m_y = newValue;
}

qreal Filter1::filter(qreal newValue, qreal dt)
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();

    // Передан ноль - автоматическая разница во времени.
    if(dt == 0.0){
        dt = static_cast<qreal>(t - m_t_z1) / 1000.0; // ms -> s.
    }

    // Не прошло сколько-нибудь значимое время с прошлого вызова.
    // значение ещё не могло измениться.
    if(dt == 0.0) return m_y;

    /*
     * y = (x + y_z1 * T / dT) / (1 + T / dT);
     */
    const qreal T_dT = m_T / dt;
    m_y = (newValue + m_y * T_dT) / (1 + T_dT);

    m_t_z1 = t;

    return m_y;
}

qreal Filter1::operator()(qreal newValue, qreal dt)
{
    return filter(newValue, dt);
}
