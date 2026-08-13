#ifndef OSCOPEDATA_H
#define OSCOPEDATA_H

#include <QtGlobal>


class OScopeData
{
public:
    OScopeData();
    virtual ~OScopeData();

    //! Период дискретизации.
    virtual qreal Ts() const = 0;

    //! Число точек.
    virtual size_t samplesCount() const = 0;

    //! Количество семплов истории (до времени t = 0).
    //! (Номер семпла в момент срабатывания триггера (t = 0)).
    virtual size_t historySize() const = 0;

    //! Число каналов.
    virtual uint channelsCount() const = 0;

    //! Получает точку канала.
    virtual qreal sample(uint ch_n, size_t i) = 0;
};

#endif // OSCOPEDATA_H
