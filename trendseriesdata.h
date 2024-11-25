#ifndef TRENDSERIESDATA_H
#define TRENDSERIESDATA_H


#include <QwtSeriesData>
#include <QPointF>
#include <QVector>

class SequentialBuffer;


class TrendSeriesData : public QwtSeriesData<QPointF>
{
public:
    TrendSeriesData(const SequentialBuffer* newBuffer = nullptr);
    ~TrendSeriesData();

    //SequentialBuffer* buffer();
    const SequentialBuffer* buffer() const;
    void setBuffer(const SequentialBuffer* newBuffer);

    size_t size() const override;
    QPointF sample(size_t i) const override;
    QRectF boundingRect() const override;

private:
    const SequentialBuffer* m_buffer;
};

#endif // TRENDSERIESDATA_H
