#ifndef SEQUENTIALBUFFER_H
#define SEQUENTIALBUFFER_H

#include <QVector>
#include <QPointF>
#include <QRectF>


/**
 * @brief Циклический буфер упорядоченных по X точек.
 */
class SequentialBuffer
{
public:

    enum AddressingMode {
        CIRCULAR = 0,
        LINEAR = 1
    };

    SequentialBuffer();
    SequentialBuffer(const SequentialBuffer& sb);
    SequentialBuffer(SequentialBuffer&& sb);
    ~SequentialBuffer();

    AddressingMode addressingMode() const;
    void setAddressingMode(AddressingMode newMode);

    size_t size() const;
    void setSize(size_t newSize);

    qreal samplingPeriod() const;
    void setSamplingPeriod(qreal newPeriod);

    qreal startTime() const;
    void setStartTime(qreal newTime);

    // Очищает данные.
    void clear();
    // Сбрасывает информацию о полученных данных.
    void reset();

    // Число семплов, записанных в буфер.
    size_t avail() const;

    const QPointF& get(size_t i) const;
    QPointF get(size_t i);
    // отрицательно приращение x
    // использует установленный период дискретизации.
    void put(const qreal& y, const qreal& dx = -1);

    const QRectF& boundingRect() const;

private:

    class Data {
    public:
        QVector<QPointF> samples;
        int index;
        int count;
        qreal period;
        qreal startTime;
        AddressingMode addrMode;
        QRectF boundingRect;
    };

    Data* m_d;

    void putAndUpd(const qreal& new_y, const qreal& new_dx);
    int transIndex(int i, int ref_i) const;
    int decIndex(int i) const;
    int incIndex(int i) const;
    int incCount(int cnt) const;
    int decCount(int cnt) const;
    void updateVerticalBounds() const;
    void recalBounds();
};

#endif // SEQUENTIALBUFFER_H
