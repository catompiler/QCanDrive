#include "trendseriesdata.h"
#include "sequentialbuffer.h"



TrendSeriesData::TrendSeriesData(SequentialBuffer* newBuffer)
    :QwtSeriesData<QPointF>()
{
    m_buffer = newBuffer;
}

TrendSeriesData::~TrendSeriesData()
{
    if(m_buffer) delete m_buffer;
}

SequentialBuffer* TrendSeriesData::buffer()
{
    return m_buffer;
}

const SequentialBuffer* TrendSeriesData::buffer() const
{
    return m_buffer;
}

void TrendSeriesData::setBuffer(SequentialBuffer* newBuffer)
{
    m_buffer = newBuffer;
}

size_t TrendSeriesData::size() const
{
    if(m_buffer == nullptr) return 0;
    return m_buffer->avail();
}

size_t TrendSeriesData::bufferSize() const
{
    if(m_buffer == nullptr) return 0;
    return m_buffer->size();
}

void TrendSeriesData::setBufferSize(size_t newSize)
{
    if(m_buffer == nullptr) return;

    m_buffer->setSize(newSize);
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

void TrendSeriesData::putSample(const qreal& newY, const qreal& newDx)
{
    if(m_buffer == nullptr) return;

    m_buffer->put(newY, newDx);
}

