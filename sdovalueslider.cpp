#include "sdovalueslider.h"
#include "sdovalue.h"
#include <QPaintEvent>
#include <QwtAbstractScaleDraw>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>



SDOValueSlider::SDOValueSlider(CoValuesHolder* newValsHolder, QWidget* parent)
    :QwtSlider(parent)
{
    m_valsHolder = newValsHolder;
    m_rdSdoValue = nullptr;
    m_wrSdoValue = nullptr;
    m_sdoValueType = COValue::Type();
    m_name = QString("");
    m_updateMask = false;

    setWrapping(false);
    setReadOnly(false);

    setGroove(true);
    setTrough(true);

    updateContentMargins();
}

SDOValueSlider::~SDOValueSlider()
{
    if(m_rdSdoValue){
        resetSDOValue();
        delete m_rdSdoValue;
    }
    if(m_wrSdoValue){
        delete m_wrSdoValue;
    }
}

QString SDOValueSlider::name() const
{
    return m_name;
}

void SDOValueSlider::setName(const QString& newName)
{
    m_name = newName;
}

CoValuesHolder* SDOValueSlider::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValueSlider::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

QColor SDOValueSlider::troughColor() const
{
    const QPalette& pal = palette();
    return pal.mid().color();
}

void SDOValueSlider::setTroughColor(const QColor& newTroughColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Mid, newTroughColor);
    setPalette(pal);
}

QColor SDOValueSlider::grooveColor() const
{
    const QPalette& pal = palette();
    return pal.dark().color();
}

void SDOValueSlider::setGrooveColor(const QColor& newGrooveColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Dark, newGrooveColor);
    setPalette(pal);
}

QColor SDOValueSlider::handleColor() const
{
    const QPalette& pal = palette();
    return pal.button().color();
}

void SDOValueSlider::setHandleColor(const QColor& newHandleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Button, newHandleColor);
    setPalette(pal);
}

QColor SDOValueSlider::scaleColor() const
{
    const QPalette& pal = palette();
    return pal.windowText().color();
}

void SDOValueSlider::setScaleColor(const QColor& newScaleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, newScaleColor);
    setPalette(pal);
}

QColor SDOValueSlider::textColor() const
{
    const QPalette& pal = palette();
    return pal.text().color();
}

void SDOValueSlider::setTextColor(const QColor& newTextColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, newTextColor);
    setPalette(pal);
}

qreal SDOValueSlider::penWidth() const
{
    const auto scaleDraw = static_cast<const QwtAbstractScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return 0.0;

    return scaleDraw->penWidthF();
}

void SDOValueSlider::setPenWidth(qreal newPenWidth)
{
    auto scaleDraw = static_cast<QwtAbstractScaleDraw*>(abstractScaleDraw());
    if(scaleDraw == nullptr) return;

    scaleDraw->setPenWidthF(newPenWidth);

    scaleChange();
}

uint SDOValueSlider::steps() const
{
    return totalSteps();
}

void SDOValueSlider::setSteps(uint newSteps)
{
    setTotalSteps(newSteps);
}

bool SDOValueSlider::hasTrough() const
{
    return QwtSlider::hasTrough();
}

void SDOValueSlider::setHasTrough(bool newHasTrough)
{
    setTrough(newHasTrough);
}

bool SDOValueSlider::hasGroove() const
{
    return QwtSlider::hasGroove();
}

void SDOValueSlider::setHasGroove(bool newHasGroove)
{
    setGroove(newHasGroove);
}

Qt::Orientation SDOValueSlider::orientation() const
{
    return QwtSlider::orientation();
}

void SDOValueSlider::setOrientation(Qt::Orientation newOrientation)
{
    QwtSlider::setOrientation(newOrientation);
}

qreal SDOValueSlider::rangeMin() const
{
    return lowerBound();
}

void SDOValueSlider::setRangeMin(qreal newRangeMin)
{
    setLowerBound(newRangeMin);
}

qreal SDOValueSlider::rangeMax() const
{
    return upperBound();
}

void SDOValueSlider::setRangeMax(qreal newRangeMax)
{
    setUpperBound(newRangeMax);
}

bool SDOValueSlider::setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin, qreal newMax)
{
    if(m_valsHolder == nullptr) return false;
    if(m_rdSdoValue || m_wrSdoValue) resetSDOValue();

    size_t typeSize = COValue::typeSize(newType);
    if(typeSize == 0) return false;

    m_wrSdoValue = new SDOValue(m_valsHolder->getSLCanOpenNode(), nullptr);
    m_wrSdoValue->setNodeId(newNodeId);
    m_wrSdoValue->setIndex(newIndex);
    m_wrSdoValue->setSubIndex(newSubIndex);
    m_wrSdoValue->setDataSize(typeSize);

    m_rdSdoValue = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, typeSize);
    if(m_rdSdoValue == nullptr){
        resetSDOValue();
        return false;
    }

    m_sdoValueType = newType;

    setScale(newMin, newMax);

    connect(m_rdSdoValue, &SDOValue::readed, this, &SDOValueSlider::sdovalueReaded);

    return true;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueSlider::getSDOValue()
{
    return m_rdSdoValue;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueSlider::getSDOValue() const
{
    return m_rdSdoValue;
}

COValue::Type SDOValueSlider::SDOValueType() const
{
    return m_sdoValueType;
}

void SDOValueSlider::resetSDOValue()
{
    if(m_valsHolder == nullptr) return;

    if(m_rdSdoValue != nullptr){
        disconnect(m_rdSdoValue, &SDOValue::readed, this, &SDOValueSlider::sdovalueReaded);

        m_valsHolder->delSdoValue(m_rdSdoValue);
        m_rdSdoValue = nullptr;
    }

    if(m_wrSdoValue != nullptr){
        delete m_wrSdoValue;
        m_wrSdoValue = nullptr;
    }
}

void SDOValueSlider::sdovalueReaded()
{
    if(m_updateMask) return;

    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    setValue(COValue::valueFrom<qreal>(sdoval->data(), m_sdoValueType, 0.0));
}

void SDOValueSlider::updateContentMargins()
{
    auto fm = fontMetrics();
    auto ms = contentsMargins();
    const int fontHeight = fm.height();
    const int bottom = fontHeight + fontHeight / 2;

    ms.setBottom(bottom);

    setContentsMargins(ms);
}

void SDOValueSlider::paintEvent(QPaintEvent* event)
{
    QwtSlider::paintEvent(event);

    QPainter painter(this);
    painter.setClipRegion( event->region() );
    const QColor color = palette().color( QPalette::Text );
    painter.setPen( color );

    auto sz = size();
    auto fm = fontMetrics();
    int fontHeight = fm.height();
    int bottom = fontHeight + fontHeight / 2;

    QRect rect(0, sz.height() - bottom, sz.width(), bottom);

    const int flags = Qt::AlignBottom | Qt::AlignHCenter;
    painter.drawText( rect, flags, m_name );
}

void SDOValueSlider::sliderChange()
{
    //qDebug() << "sliderChange()";

    QwtSlider::sliderChange();

    if(!m_wrSdoValue->running()){
//        qDebug() << /*"sliderChange()" <<*/ value()
//                 << COValue::valueTo(m_wrSdoValue->data(), m_sdoValueType, value())
//                 << COValue::valueFrom(m_wrSdoValue->data(), m_sdoValueType, 0.0);

        if(COValue::valueTo(m_wrSdoValue->data(), m_sdoValueType, value())){
        /*qDebug() << */m_wrSdoValue->write();
        }
    }
}


void SDOValueSlider::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "mousePressEvent()";

    QwtSlider::mousePressEvent(event);

    m_updateMask = true;
}

void SDOValueSlider::mouseReleaseEvent(QMouseEvent* event)
{
    //qDebug() << "mouseReleaseEvent()";

    QwtSlider::mouseReleaseEvent(event);

    m_updateMask = false;
}
