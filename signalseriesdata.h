#ifndef SIGNALSERIESDATA_H
#define SIGNALSERIESDATA_H


#include <QwtSeriesData>
#include <QPointF>
#include <QVector>

class SequentialBuffer;


class SignalSeriesData : public QwtSeriesData<QPointF>
{
public:
    SignalSeriesData(SequentialBuffer* newBuffer = nullptr);
    ~SignalSeriesData();

    SequentialBuffer* buffer();
    const SequentialBuffer* buffer() const;
    void setBuffer(SequentialBuffer* newBuffer);

    size_t size() const override;
    size_t bufferSize() const;
    void setBufferSize(size_t newSize);

    QPointF sample(size_t i) const override;
    QRectF boundingRect() const override;

    void putSample(const qreal& newY, const qreal& newDx = -1);

private:
    SequentialBuffer* m_buffer;
};

#endif // SIGNALSERIESDATA_H
