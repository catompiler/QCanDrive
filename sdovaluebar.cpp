#include "sdovaluebar.h"
#include "sdovalue.h"
#include <QPaintEvent>
#include <QwtAbstractScaleDraw>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QMessageBox>
#include <QDebug>


//#define SDOVALUEBAR_MESSAGE_ON_WRITE_ERROR 0



SDOValueBar::SDOValueBar(CoValuesHolder* newValsHolder, QWidget* parent)
    :QwtThermo(parent)
{
    m_valsHolder = newValsHolder;
    m_sdoValue = nullptr;
    m_sdoValueType = COValue::Type();
    m_name = QString("");

    updateContentMargins();
}

SDOValueBar::~SDOValueBar()
{
    if(m_sdoValue){
        resetSDOValue();
        delete m_sdoValue;
    }
}

QString SDOValueBar::name() const
{
    return m_name;
}

void SDOValueBar::setName(const QString& newName)
{
    m_name = newName;
}

CoValuesHolder* SDOValueBar::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValueBar::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

QColor SDOValueBar::barBackColor() const
{
    const QPalette& pal = palette();
    return pal.base().color();
}

void SDOValueBar::setBarBackColor(const QColor& newBarBackColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, newBarBackColor);
    setPalette(pal);
}

QColor SDOValueBar::barColor() const
{
    const QPalette& pal = palette();
    return pal.buttonText().color();
}

void SDOValueBar::setBarColor(const QColor& newBarColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::ButtonText, newBarColor);
    setPalette(pal);
}

QColor SDOValueBar::barAlarmColor() const
{
    const QPalette& pal = palette();
    return pal.highlight().color();
}

void SDOValueBar::setBarAlarmColor(const QColor& newBarAlarmColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Highlight, newBarAlarmColor);
    setPalette(pal);
}

QColor SDOValueBar::scaleColor() const
{
    const QPalette& pal = palette();
    return pal.windowText().color();
}

void SDOValueBar::setScaleColor(const QColor& newScaleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, newScaleColor);
    setPalette(pal);
}

QColor SDOValueBar::textColor() const
{
    const QPalette& pal = palette();
    return pal.text().color();
}

void SDOValueBar::setTextColor(const QColor& newTextColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, newTextColor);
    setPalette(pal);
}

int SDOValueBar::barWidth() const
{
    return pipeWidth();
}

void SDOValueBar::setBarWidth(int newBarWidth)
{
    setPipeWidth(newBarWidth);
}

qreal SDOValueBar::penWidth() const
{
    const auto scaleDraw = static_cast<const QwtAbstractScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return 0.0;

    return scaleDraw->penWidthF();
}

void SDOValueBar::setPenWidth(qreal newPenWidth)
{
    auto scaleDraw = static_cast<QwtAbstractScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return;

    scaleDraw->setPenWidthF(newPenWidth);

    scaleChange();
}

Qt::Orientation SDOValueBar::orientation() const
{
    return QwtThermo::orientation();
}

void SDOValueBar::setOrientation(Qt::Orientation newOrientation)
{
    QwtThermo::setOrientation(newOrientation);

    updateContentMargins();
}

QwtThermo::ScalePosition SDOValueBar::scalePosition() const
{
    return QwtThermo::scalePosition();
}

void SDOValueBar::setScalePosition(ScalePosition newPos)
{
    QwtThermo::setScalePosition(newPos);

    updateContentMargins();
}

qreal SDOValueBar::rangeMin() const
{
    return lowerBound();
}

void SDOValueBar::setRangeMin(qreal newRangeMin)
{
    setLowerBound(newRangeMin);
}

qreal SDOValueBar::rangeMax() const
{
    return upperBound();
}

void SDOValueBar::setRangeMax(qreal newRangeMax)
{
    setUpperBound(newRangeMax);
}

bool SDOValueBar::setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin, qreal newMax)
{
    if(m_valsHolder == nullptr) return false;
    if(m_sdoValue) resetSDOValue();

    size_t typeSize = COValue::typeSize(newType);
    if(typeSize == 0) return false;

    m_sdoValue = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, typeSize);
    if(m_sdoValue == nullptr){
        return false;
    }

    m_sdoValueType = newType;

    setScale(newMin, newMax);

    connect(m_sdoValue, &SDOValue::readed, this, &SDOValueBar::sdovalueReaded);

    return true;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueBar::getSDOValue()
{
    return m_sdoValue;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueBar::getSDOValue() const
{
    return m_sdoValue;
}

COValue::Type SDOValueBar::SDOValueType() const
{
    return m_sdoValueType;
}

void SDOValueBar::resetSDOValue()
{
    if(m_valsHolder == nullptr) return;

    if(m_sdoValue != nullptr){
        disconnect(m_sdoValue, &SDOValue::readed, this, &SDOValueBar::sdovalueReaded);

        m_valsHolder->delSdoValue(m_sdoValue);
        m_sdoValue = nullptr;
    }
}

void SDOValueBar::sdovalueReaded()
{
    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    setValue(COValue::valueFrom<qreal>(sdoval->data(), m_sdoValueType, 0.0));
}

void SDOValueBar::updateContentMargins()
{
    auto fm = fontMetrics();
    auto ms = contentsMargins();
    const int fontHeight = fm.height();
    const int vertMarg = fontHeight + fontHeight / 2;

    // Bug in QwtThermo.
    /*if(orientation() == Qt::Horizontal && scalePosition() != QwtThermo::TrailingScale){
        ms.setTop(vertMarg);
        ms.setBottom(0);
    }else*/{
        ms.setTop(0);
        ms.setBottom(vertMarg);
    }

    //qDebug() << "updateContentMargins()" << orientation() << scalePosition() << ms;

    setContentsMargins(ms);
}

void SDOValueBar::paintEvent(QPaintEvent* event)
{
    QwtThermo::paintEvent(event);

    QPainter painter(this);
    painter.setClipRegion( event->region() );
    const QColor color = palette().color( QPalette::Text );
    painter.setPen( color );

    auto sz = size();
    auto fm = fontMetrics();
    int fontHeight = fm.height();
    int vertMarg = fontHeight + fontHeight / 2;

    QRect rect;
    int flags = 0;

    // Bug in QwtThermo.
    /*if(orientation() == Qt::Horizontal && scalePosition() != QwtThermo::TrailingScale){
        rect = QRect(0, 0, sz.width(), vertMarg);
        flags = Qt::AlignTop | Qt::AlignHCenter;
    }else*/{
        rect = QRect(0, sz.height() - vertMarg, sz.width(), vertMarg);
        flags = Qt::AlignBottom | Qt::AlignHCenter;
    }

    //qDebug() << "paintEvent()" << orientation() << scalePosition() << rect << flags << this->rect() << contentsRect() << contentsMargins();

    painter.drawText( rect, flags, m_name );
    //painter.setPen(Qt::red);
    //painter.drawRect(contentsRect());
}

void SDOValueBar::changeEvent(QEvent* event)
{
    QwtThermo::changeEvent(event);

    switch(event->type()){
    default:
        break;
    case QEvent::StyleChange:
    case QEvent::FontChange:
        updateContentMargins();
        break;
    }
}
