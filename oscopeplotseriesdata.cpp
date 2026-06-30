#include "oscopeplotseriesdata.h"
#include "oscopedata.h"
#include "oscopehorizontal.h"
#include <assert.h>
#include <QDebug>


#define TS_DEFAULT 1.0



OScopePlotSeriesData::OScopePlotSeriesData(OScopeHorizontal* newHori, OScopeData* newOScopeData, uint new_ch_n)
    :QwtSeriesData<QPointF>()
{
    m_hori = newHori;
    m_vert = new OScopeVertical();
    m_oscData = newOScopeData;
    m_ch_n = new_ch_n;

    invalidateBounds();
}

OScopePlotSeriesData::~OScopePlotSeriesData()
{
    delete m_vert;
}

void OScopePlotSeriesData::setHorizontal(OScopeHorizontal* newHori)
{
    m_hori = newHori;
}

void OScopePlotSeriesData::setScope(OScopeData* newOScopeData)
{
    m_oscData = newOScopeData;
}

void OScopePlotSeriesData::setChannel(uint new_ch_n)
{
    m_ch_n = new_ch_n;
}

qreal OScopePlotSeriesData::vDiv() const
{
    return m_vert->vDiv();
}

void OScopePlotSeriesData::setVDiv(qreal newVDiv)
{
    m_vert->setVDiv(newVDiv);

    //invalidateBounds();
}

qreal OScopePlotSeriesData::vOffset() const
{
    return m_vert->vOffset();
}

void OScopePlotSeriesData::setVOffset(qreal newVOffset)
{
    m_vert->setVOffset(newVOffset);

    //invalidateBounds();
}

void OScopePlotSeriesData::invalidateBounds()
{
    cachedBoundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
}

// void OScopePlotSeriesData::update()
// {
//     // Нет осциллографа - нечего обновлять.
//     if(m_oscData == nullptr) return;

//     // Обновим границы.
//     updateBounds();
// }

void OScopePlotSeriesData::updateBounds() const
{
    assert(m_oscData != nullptr);

    uint samples_count = m_oscData->samplesCount();
    if(samples_count == 0) return;

    qreal val = getYValue(0);

    qreal min_y = val;
    qreal max_y = val;

    for(uint i = 1; i < samples_count; i ++){
        val = getYValue(i);
        min_y = std::min(min_y, val);
        max_y = std::max(max_y, val);
    }

    qreal left = getXValue(0);
    qreal right = getXValue(samples_count - 1);
    cachedBoundingRect.setCoords(left, min_y, right, max_y);

    //qDebug() << m_bounds;
}

size_t OScopePlotSeriesData::size() const
{
    if(m_oscData == nullptr) return 0;

    return m_oscData->samplesCount();
}

QPointF OScopePlotSeriesData::sample(size_t i) const
{
    qreal yval = getYValue(i);
    qreal xval = getXValue(i);

    return QPointF(xval, yval);
}

QRectF OScopePlotSeriesData::boundingRect() const
{
    if ( cachedBoundingRect.width() < 0.0 )
        updateBounds();

    return cachedBoundingRect;
}

qreal OScopePlotSeriesData::getYValue(size_t i) const
{
    assert(m_oscData != nullptr);
    //assert(m_vert != nullptr);

    qreal val = m_oscData->sample(m_ch_n, i);

    return (val + m_vert->vOffset()) * m_vert->invVDiv();
}

qreal OScopePlotSeriesData::getXValue(size_t i) const
{
    assert(m_oscData != nullptr);
    assert(m_hori != nullptr);

    qreal val = m_oscData->Ts() * i;

    return (val + m_hori->hOffset()) * m_hori->invHDiv();
}

