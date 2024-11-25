#include "sequentialbuffer.h"

SequentialBuffer::SequentialBuffer()
{
    m_d = new Data();
    m_d->period = 1.0;
    m_d->startTime = 0.0;
    m_d->addrMode = CIRCULAR;
    reset();
}

SequentialBuffer::SequentialBuffer(const SequentialBuffer& sb)
{
    m_d = new Data();
    m_d->samples = sb.m_d->samples;
    m_d->index = sb.m_d->index;
    m_d->count = sb.m_d->count;
    m_d->period = sb.m_d->period;
    m_d->startTime = sb.m_d->startTime;
    m_d->addrMode = sb.m_d->addrMode;
    m_d->boundingRect = sb.m_d->boundingRect;
}

SequentialBuffer::SequentialBuffer(SequentialBuffer&& sb)
{
    Data* d = sb.m_d;
    sb.m_d = nullptr;
    m_d = d;
}

SequentialBuffer::~SequentialBuffer()
{
    if(m_d) delete m_d;
}

SequentialBuffer::AddressingMode SequentialBuffer::addressingMode() const
{
    return m_d->addrMode;
}

void SequentialBuffer::setAddressingMode(AddressingMode newMode)
{
    m_d->addrMode = newMode;
}

size_t SequentialBuffer::size() const
{
    return m_d->samples.size();
}

void SequentialBuffer::setSize(size_t newSize)
{
    m_d->samples.resize(newSize);
}

qreal SequentialBuffer::samplingPeriod() const
{
    return m_d->period;
}

void SequentialBuffer::setSamplingPeriod(qreal newPeriod)
{
    m_d->period = newPeriod;
}

qreal SequentialBuffer::startTime() const
{
    return m_d->startTime;
}

void SequentialBuffer::setStartTime(qreal newTime)
{
    m_d->startTime = newTime;
}

void SequentialBuffer::clear()
{
    //int sz = m_d->samples.size();
    //m_d->samples.clear();
    //m_d->samples.resize(sz);
    m_d->samples.fill(QPointF());

    reset();
}

void SequentialBuffer::reset()
{
    m_d->index = 0;
    m_d->count = 0;
    m_d->boundingRect = QRectF(0, 0, -1, -1);
}

size_t SequentialBuffer::avail() const
{
    return static_cast<size_t>(m_d->count);
}

void SequentialBuffer::put(const qreal& y, const qreal& dx)
{
    putAndUpd(y, dx);
//    m_d->index = incIndex(m_d->index);
    //    m_d->count = incCount(m_d->count);
}

const QRectF& SequentialBuffer::boundingRect() const
{
    return m_d->boundingRect;
}

const QPointF& SequentialBuffer::get(size_t i) const
{
    static const QPointF zero_point = QPointF();

    if(i >= static_cast<size_t>(m_d->samples.size())) return zero_point;

    return m_d->samples[transIndex(static_cast<int>(i), m_d->index)];
}

QPointF SequentialBuffer::get(size_t i)
{
    if(i >= static_cast<size_t>(m_d->samples.size())) return QPointF();

    return m_d->samples[transIndex(static_cast<int>(i), m_d->index)];
}

void SequentialBuffer::putAndUpd(const qreal& new_y, const qreal& new_dx)
{
    int i = m_d->index;
    int cnt = m_d->count;

    QVector<QPointF>& m_samples = m_d->samples;
    QRectF& bounds = m_d->boundingRect;

    if(cnt == 0){
        QPointF p = QPointF(m_d->startTime, new_y);

        m_samples[i] = p;

        m_d->index = incIndex(i);
        m_d->count = incCount(cnt);

        // Обновим границу.
        bounds = QRectF(p, p);
        return;
    }

    int sz = m_samples.size();

    // Координаты границы.
    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = bounds.bottom();
    qreal bottom = bounds.top();

    // Координаты добавляемой точки.
    qreal dx = (new_dx < 0) ? m_d->period : new_dx;
    qreal x = right + dx;
    qreal y = new_y;
    // Добавляемая точка.
    QPointF p(x, y);

    // Проверка новой точки по оси Y.
    if(y > top) top = y;
    if(y < bottom) bottom = y;

    // Массив не заполнен.
    if(cnt < sz){
        //left = left;
        right = x;

        m_samples[i] = p;

        m_d->index = incIndex(i);
        m_d->count = incCount(cnt);

        // Обновим границу.
        bounds.setCoords(left, bottom, right, top);
        return;
    }

    // Координаты старой точки.
    //int old_x;
    int old_y;
    {
        // Старая точка.
        QPointF& old_p = m_samples[i];
        //old_x = old_p.x();
        old_y = old_p.y();
    }

    // Добавим точку.
    m_samples[i] = p;

    m_d->index = incIndex(i);
    m_d->count = incCount(cnt);

    // Первая точка.
    QPointF& first_p = m_samples[m_d->index];

    // Новые границы по горизонтали.
    left = first_p.x();
    right = p.x();

    // Обновим границу.
    bounds.setCoords(left, bottom, right, top);

    //Если старая точка была граничной по вертикали.
    if((old_y <= bottom && old_y < y) || (old_y >= top && old_y > y)){

        // Пересчитаем вертикальные границы.
        updateVerticalBounds();
        return;
    }
}

int SequentialBuffer::transIndex(int i, int ref_i) const
{
    if(m_d->addrMode == LINEAR) return i;

    int sz = m_d->samples.size();

    if(m_d->count < sz) return i;

    int ci = i + ref_i;
    if(ci >= sz) ci -= sz;

    return ci;
}

int SequentialBuffer::decIndex(int i) const
{
    int sz = m_d->samples.size();

    int ri = i - 1;
    if(ri < 0) ri += sz;

    return ri;
}

int SequentialBuffer::incIndex(int i) const
{
    int sz = m_d->samples.size();

    int ri = i + 1;
    if(ri >= sz) ri -= sz;

    return ri;
}

int SequentialBuffer::incCount(int cnt) const
{
    int sz = m_d->samples.size();

    if(cnt < sz) return cnt + 1;
    return sz;
}

int SequentialBuffer::decCount(int cnt) const
{
    if(cnt > 0) return cnt - 1;
    return 0;
}

void SequentialBuffer::updateVerticalBounds() const
{
    const QVector<QPointF>& m_samples = m_d->samples;
    QRectF& bounds = m_d->boundingRect;

    if(m_samples.isEmpty()){
        bounds = QRectF(0, 0, -1, -1);
        return;
    }

    int cnt = m_d->count;
    const QPointF& p_first = m_samples.first();

    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = p_first.y();
    qreal bottom = p_first.y();

    for(int i = 1; i < cnt; i ++){
        const QPointF& p = m_samples[i];
        qreal y = p.y();

        top = std::max(top, y);
        bottom = std::min(bottom, y);
    }

    bounds.setCoords(left, bottom, right, top);
}

void SequentialBuffer::recalBounds()
{
    const QVector<QPointF>& m_samples = m_d->samples;

    if(m_samples.isEmpty()){
        m_d->boundingRect = QRectF(0, 0, -1, -1);
        return;
    }

    int cnt = m_d->count;
    const QPointF& p_first = m_samples.first();

    if(cnt == 1){
        m_d->boundingRect = QRectF(p_first, p_first);
        return;
    }

    qreal left = p_first.x();
    qreal right = left;
    qreal bottom = p_first.y();
    qreal top = bottom;

    for(int i = 1; i < cnt; i ++){
        const QPointF& p = m_samples[i];
        qreal x = p.x();
        qreal y = p.y();

        left = std::min(left, x);
        right = std::max(right, x);
        bottom = std::min(bottom, y);
        top = std::max(top, y);
    }

    m_d->boundingRect.setCoords(left, top, right, bottom);
}

