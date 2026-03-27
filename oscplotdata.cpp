#include "oscplotdata.h"
#include "sdoscope.h"
#include <assert.h>
#include <QDebug>


#define TS_DEFAULT 1.0



OscPlotData::OscPlotData(SDOScope* newScope, uint new_ch_n)
    :QwtSeriesData<QPointF>()
{
    m_scope = newScope;
    m_ch_n = new_ch_n;
    m_offset = 0.0;
    m_gain = 1.0;
    // в update().
    //m_Ts = 0.0;
    //m_bounds = QRectF();
    update();
}

OscPlotData::~OscPlotData()
{
}

size_t OscPlotData::size() const
{
    if(m_scope == nullptr) return 0;

    return m_scope->samplesCount();
}

QPointF OscPlotData::sample(size_t i) const
{
    if(m_scope == nullptr) return QPointF();

    return QPointF(m_Ts * i, getValue(i));
}

QRectF OscPlotData::boundingRect() const
{
    return m_bounds;
}

void OscPlotData::setScope(SDOScope* newScope)
{
    m_scope = newScope;
}

void OscPlotData::setChannel(uint new_ch_n)
{
    m_ch_n = new_ch_n;
}

qreal OscPlotData::offset() const
{
    return m_offset;
}

void OscPlotData::setOffset(qreal val)
{
    m_offset = val;
}

qreal OscPlotData::gain() const
{
    return m_gain;
}

void OscPlotData::setGain(qreal val)
{
    m_gain = val;
}

void OscPlotData::update()
{
    // Сброс значений.
    m_Ts = 0.0;
    m_bounds = QRectF();

    // Нет осциллографа - нечего обновлять.
    if(m_scope == nullptr) return;

    // Обновим период дискретизации.
    updateTs();

    // Обновим границы.
    updateBounds();
}

void OscPlotData::updateTs()
{
    assert(m_scope != nullptr);

    // Частота дискретизации.
    uint Fs = m_scope->sampleRate();
    // Если частота дискретизации задана - вычислим время.
    if(Fs != 0){
        m_Ts = 1.0 / Fs;
    }else{ // Если частота не задана.
        // Запасной вариант.
        m_Ts = TS_DEFAULT;
    }
}

void OscPlotData::updateBounds()
{
    assert(m_scope != nullptr);

    uint samples_count = m_scope->samplesCount();
    if(samples_count == 0) return;

    qreal val = getValue(0);

    qreal min_y = val;
    qreal max_y = val;

    for(uint i = 1; i < samples_count; i ++){
        val = getValue(i);
        min_y = std::min(min_y, val);
        max_y = std::max(max_y, val);
    }

    qreal left = 0.0;
    m_bounds.setCoords(left, min_y, left + m_Ts * (samples_count - 1), max_y);

    //qDebug() << m_bounds;
}

qreal OscPlotData::getValue(size_t i) const
{
    SDOScope::Channel* ch = m_scope->channel(m_ch_n);
    if(ch == nullptr) return 0;

    return ch->value(i) * m_gain + m_offset;
}

