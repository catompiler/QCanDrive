#include "signalseriesdata.h"
#include "sequentialbuffer.h"



SignalSeriesData::SignalSeriesData(SequentialBuffer* newBuffer)
    :QwtSeriesData<QPointF>()
{
    m_buffer = newBuffer;
}

SignalSeriesData::~SignalSeriesData()
{
    if(m_buffer) delete m_buffer;
}

SequentialBuffer* SignalSeriesData::buffer()
{
    return m_buffer;
}

const SequentialBuffer* SignalSeriesData::buffer() const
{
    return m_buffer;
}

void SignalSeriesData::setBuffer(SequentialBuffer* newBuffer)
{
    m_buffer = newBuffer;
}

size_t SignalSeriesData::size() const
{
    if(m_buffer == nullptr) return 0;
    return m_buffer->avail();
}

size_t SignalSeriesData::bufferSize() const
{
    if(m_buffer == nullptr) return 0;
    return m_buffer->size();
}

void SignalSeriesData::setBufferSize(size_t newSize)
{
    if(m_buffer == nullptr) return;

    m_buffer->setSize(newSize);
}

QPointF SignalSeriesData::sample(size_t i) const
{
    if(m_buffer == nullptr) return QPointF();
    return m_buffer->get(i);
}

QRectF SignalSeriesData::boundingRect() const
{
    if(m_buffer == nullptr) return QRectF(0, 0, -1, -1);
    return m_buffer->boundingRect();
}

void SignalSeriesData::clear()
{
    if(m_buffer == nullptr) return;
    m_buffer->clear();
}

void SignalSeriesData::putSample(const qreal& newY, const qreal& newDx)
{
    if(m_buffer == nullptr) return;

    m_buffer->put(newY, newDx);
}

