#include "trendseriesdata.h"
#include "sequentialbuffer.h"



TrendSeriesData::TrendSeriesData(const SequentialBuffer* newBuffer)
    :QwtSeriesData<QPointF>()
{
    m_buffer = newBuffer;
}

TrendSeriesData::~TrendSeriesData()
{
}

/*SequentialBuffer* TrendSeriesData::buffer()
{
    return m_buffer;
}*/

const SequentialBuffer* TrendSeriesData::buffer() const
{
    return m_buffer;
}

void TrendSeriesData::setBuffer(const SequentialBuffer* newBuffer)
{
    m_buffer = newBuffer;
}

size_t TrendSeriesData::size() const
{
    if(m_buffer == nullptr) return 0;
    return m_buffer->avail();
}

QPointF TrendSeriesData::sample(size_t i) const
{
    if(m_buffer == nullptr) return QPointF();
    return m_buffer->get(i);
}

QRectF TrendSeriesData::boundingRect() const
{
    if(m_buffer == nullptr) return QRectF(0, 0, -1, -1);
    return m_buffer->boundingRect();
}

