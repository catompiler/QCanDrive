#ifndef TRENDSERIESDATA_H
#define TRENDSERIESDATA_H


#include <QwtSeriesData>
#include <QPointF>
#include <QVector>

class SequentialBuffer;


class TrendSeriesData : public QwtSeriesData<QPointF>
{
public:
    TrendSeriesData(SequentialBuffer* newBuffer = nullptr);
    ~TrendSeriesData();

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

#endif // TRENDSERIESDATA_H
