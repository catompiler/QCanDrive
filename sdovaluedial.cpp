#include "sdovaluedial.h"
#include "sdovalue.h"
#include <QwtDialSimpleNeedle>
#include <QwtRoundScaleDraw>
#include <QwtAbstractScaleDraw>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>



SDOValueDial::SDOValueDial(CoValuesHolder* newValsHolder, QWidget* parent)
    :QwtDial(parent)
{
    m_valsHolder = newValsHolder;
    m_sdoValue = nullptr;
    m_sdoValueType = COValue::Type();

    m_name = QString();
    m_precision = 0;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto scaleDraw = new QwtRoundScaleDraw();
    scaleDraw->enableComponent(QwtRoundScaleDraw::Backbone, false);
    setScaleDraw(scaleDraw);

    auto needle = new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow);
    setNeedle(needle);

    setWrapping(false);
    setReadOnly(true);

    setOrigin(135.0);
    setScaleArc(0.0, 270.0);

    //setMode(QwtDial::RotateNeedle);
    //setFrameShadow(QwtDial::Raised);
    //setTotalSteps(100);
}

SDOValueDial::~SDOValueDial()
{
    if(m_sdoValue){
        resetSDOValue();
        delete m_sdoValue;
    }
}

QString SDOValueDial::name() const
{
    return m_name;
}

void SDOValueDial::setName(const QString& newName)
{
    m_name = newName;
}

CoValuesHolder* SDOValueDial::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValueDial::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

QColor SDOValueDial::outsideBackColor() const
{
    const QPalette& pal = palette();
    return pal.window().color();
}

void SDOValueDial::setOutsideBackColor(const QColor& newBackOuterColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, newBackOuterColor);
    setPalette(pal);
}

QColor SDOValueDial::insideBackColor() const
{
    const QPalette& pal = palette();
    return pal.base().color();
}

void SDOValueDial::setInsideBackColor(const QColor& newBackInnerColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, newBackInnerColor);
    setPalette(pal);
}

QColor SDOValueDial::insideScaleBackColor() const
{
    const QPalette& pal = palette();
    return pal.windowText().color();
}

void SDOValueDial::setInsideScaleBackColor(const QColor& newBackScaleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, newBackScaleColor);
    setPalette(pal);
}

QColor SDOValueDial::textScaleColor() const
{
    const QPalette& pal = palette();
    return pal.text().color();
}

void SDOValueDial::setTextScaleColor(const QColor& newTextScaleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, newTextScaleColor);
    setPalette(pal);
}

QColor SDOValueDial::needleColor() const
{
    const auto ndl = static_cast<const QwtDialSimpleNeedle*>(needle());
    if(ndl == nullptr) return QColor();

    const QPalette& pal = ndl->palette();
    return pal.mid().color();
}

void SDOValueDial::setNeedleColor(const QColor& newNeedleColor)
{
    auto ndl = static_cast<QwtDialSimpleNeedle*>(needle());
    if(ndl == nullptr) return;

    QPalette pal = ndl->palette();
    pal.setColor(QPalette::Mid, newNeedleColor);
    //pal.setColor(QPalette::Base, newNeedleColor);
    ndl->setPalette(pal);
}

qreal SDOValueDial::penWidth() const
{
    const auto scaleDraw = static_cast<const QwtRoundScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return 0.0;

    return scaleDraw->penWidthF();
}

void SDOValueDial::setPenWidth(qreal newPenWidth)
{
    auto scaleDraw = static_cast<QwtRoundScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return;

    scaleDraw->setPenWidthF(newPenWidth);

    scaleChange();
}

uint SDOValueDial::precision() const
{
    return m_precision;
}

void SDOValueDial::setPrecision(uint newPrecision)
{
    m_precision = newPrecision;
}

qreal SDOValueDial::rangeMin() const
{
    return lowerBound();
}

void SDOValueDial::setRangeMin(qreal newRangeMin)
{
    setLowerBound(newRangeMin);
}

qreal SDOValueDial::rangeMax() const
{
    return upperBound();
}

void SDOValueDial::setRangeMax(qreal newRangeMax)
{
    setUpperBound(newRangeMax);
}

bool SDOValueDial::setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin, qreal newMax)
{
    if(m_valsHolder == nullptr) return false;
    if(m_sdoValue) resetSDOValue();

    size_t typeSize = COValue::typeSize(newType);
    if(typeSize == 0) return false;

    m_sdoValue = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, typeSize);
    if(m_sdoValue == nullptr) return false;

    m_sdoValueType = newType;

    setScale(newMin, newMax);

    connect(m_sdoValue, &SDOValue::readed, this, &SDOValueDial::sdovalueReaded);

    return true;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueDial::getSDOValue()
{
    return m_sdoValue;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueDial::getSDOValue() const
{
    return m_sdoValue;
}

COValue::Type SDOValueDial::SDOValueType() const
{
    return m_sdoValueType;
}

void SDOValueDial::resetSDOValue()
{
    if(m_valsHolder == nullptr) return;
    if(m_sdoValue == nullptr) return;

    disconnect(m_sdoValue, &SDOValue::readed, this, &SDOValueDial::sdovalueReaded);

    m_valsHolder->delSdoValue(m_sdoValue);
    m_sdoValue = nullptr;
}

void SDOValueDial::sdovalueReaded()
{
    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    setValue(COValue::valueFrom<qreal>(sdoval->data(), m_sdoValueType, 0.0));
}


// From Qwt Dials example.
void SDOValueDial::drawScaleContents(QPainter* painter, const QPointF& center, double radius) const
{
    const int fontHeight = fontMetrics().height();

    QRect inrect = innerRect();
    qreal innerRadius = static_cast<qreal>(qMin(inrect.width(), inrect.height())) * 0.5;

    painter->save();

    const QColor color = palette().color( QPalette::Text );
    painter->setPen( color );

    QRectF rect( 0.0, 0.0, 2.0 * radius, radius + innerRadius + 0.5 * fontHeight );
    rect.moveCenter( center );

    const int flags = Qt::AlignBottom | Qt::AlignHCenter;
    painter->drawText( rect, flags, m_name );

    painter->restore();
}


void SDOValueDial::drawNeedle(QPainter* painter, const QPointF& center, double radius, double direction, QPalette::ColorGroup colorGroup) const
{
    const int fontHeight = fontMetrics().height();

    painter->save();

    const QColor color = needleColor();
    painter->setPen( color );

    QRectF rect_value( 0.0, 0.0, 2.0 * radius, radius );
    rect_value.moveLeft( center.x() - radius );
    rect_value.moveTop(center.y());

    QFont font_value(font());
    font_value.setPixelSize(fontHeight * 1.5);

    const int flags_value = Qt::AlignVCenter | Qt::AlignHCenter;
    painter->setFont(font_value);
    painter->drawText( rect_value, flags_value, QString::number(value(), 'f', m_precision) );

    painter->restore();

    QwtDial::drawNeedle(painter, center, radius, direction, colorGroup);
}
