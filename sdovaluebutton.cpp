#include "sdovaluebutton.h"
#include "sdovalue.h"
#include <QPaintEvent>
#include <QwtAbstractScaleDraw>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QGradient>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QMessageBox>
#include <QDebug>


//#define SDOVALUEBUTTON_MESSAGE_ON_WRITE_ERROR 0



SDOValueButton::SDOValueButton(CoValuesHolder* newValsHolder, QWidget* parent)
    :QAbstractButton(parent)
{
    m_valsHolder = newValsHolder;
    m_sdoValue = nullptr;
    m_sdoValueType = COValue::Type();

    m_borderWidth = 16;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    //QPalette pal = palette();
    //pal.setBrush(QPalette::Shadow, Qt::black);
    //setPalette(pal);
}

SDOValueButton::~SDOValueButton()
{
    if(m_sdoValue){
        resetSDOValue();
        delete m_sdoValue;
    }
}

CoValuesHolder* SDOValueButton::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValueButton::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

QColor SDOValueButton::buttonBackColor() const
{
    const QPalette& pal = palette();
    return pal.base().color();
}

void SDOValueButton::setButtonBackColor(const QColor& newButtonBackColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, newButtonBackColor);
    setPalette(pal);
}

QColor SDOValueButton::buttonColor() const
{
    const QPalette& pal = palette();
    return pal.buttonText().color();
}

void SDOValueButton::setButtonColor(const QColor& newButtonColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::ButtonText, newButtonColor);
    setPalette(pal);
}

QColor SDOValueButton::buttonAlarmColor() const
{
    const QPalette& pal = palette();
    return pal.highlight().color();
}

void SDOValueButton::setButtonAlarmColor(const QColor& newButtonAlarmColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Highlight, newButtonAlarmColor);
    setPalette(pal);
}

QColor SDOValueButton::scaleColor() const
{
    const QPalette& pal = palette();
    return pal.windowText().color();
}

void SDOValueButton::setScaleColor(const QColor& newScaleColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, newScaleColor);
    setPalette(pal);
}

QColor SDOValueButton::textColor() const
{
    const QPalette& pal = palette();
    return pal.text().color();
}

void SDOValueButton::setTextColor(const QColor& newTextColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, newTextColor);
    setPalette(pal);
}

bool SDOValueButton::setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType)
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

    connect(m_sdoValue, &SDOValue::readed, this, &SDOValueButton::sdovalueReaded);

    return true;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueButton::getSDOValue()
{
    return m_sdoValue;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueButton::getSDOValue() const
{
    return m_sdoValue;
}

COValue::Type SDOValueButton::SDOValueType() const
{
    return m_sdoValueType;
}

void SDOValueButton::resetSDOValue()
{
    if(m_valsHolder == nullptr) return;

    if(m_sdoValue != nullptr){
        disconnect(m_sdoValue, &SDOValue::readed, this, &SDOValueButton::sdovalueReaded);

        m_valsHolder->delSdoValue(m_sdoValue);
        m_sdoValue = nullptr;
    }
}

void SDOValueButton::sdovalueReaded()
{
    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    //setValue(COValue::valueFrom<qreal>(sdoval->data(), m_sdoValueType, 0.0));
}

void SDOValueButton::paintEvent(QPaintEvent* event)
{
    //QAbstractButton::paintEvent(event);

    QPainter painter(this);
    painter.setClipRegion( event->region() );

    drawButton(&painter);
}

void SDOValueButton::drawButton(QPainter* p)
{
    drawButtonIdle(p);
}

void SDOValueButton::drawButtonIdle(QPainter* p)
{
    p->fillRect(rect(), Qt::blue);
    drawBorder(p, m_borderWidth, palette().button().color(), palette().shadow().color());
    drawCenter(p);
    drawHighlight(p);
    p->setPen(palette().buttonText().color());
    p->drawText(contentsRect(), Qt::AlignHCenter | Qt::AlignVCenter, text());
}

void SDOValueButton::drawBorder(QPainter* p, int borderWidth, const QColor& btnCol, const QColor& brdrCol)
{
    auto cr = contentsRect();

    int left = cr.left();
    int right = cr.right();
    int top = cr.top();
    int bottom = cr.bottom();
    int w = cr.width();
    int h = cr.height();

    const int& bw = borderWidth;

    const QColor& fc = btnCol;
    const QColor& bc = brdrCol;

    // border rectangles.
    QLinearGradient grl;
    grl.setColorAt(0, bc);
    grl.setColorAt(1.0, fc);

    // left. V
    grl.setStart(left + 1, top);
    grl.setFinalStop(left + bw, top);
    p->fillRect(left, top + bw, bw, h - 2 * bw, grl);

    // down. V
    grl.setStart(left, bottom);
    grl.setFinalStop(left, bottom - bw + 1);
    p->fillRect(left + bw, bottom - bw + 1, w - 2 * bw, bw, grl);

    // right. V
    grl.setStart(right, top);
    grl.setFinalStop(right - bw + 1, top);
    p->fillRect(right - bw + 1, top + bw, bw, h - 2 * bw, grl);

    // up. V
    grl.setStart(right, top + 1);
    grl.setFinalStop(right, top + bw);
    p->fillRect(left + bw, top, w - 2 * bw, bw, grl);

    // small quads
    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, fc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw - 1);

    // top left. V
    grr.setCenter(left + bw, top + bw);
    grr.setFocalPoint(grr.center());
    p->fillRect(left, top, bw, bw, grr);

    // bottom left. V
    grr.setCenter(left + bw, bottom - bw + 1);
    grr.setFocalPoint(grr.center());
    p->fillRect(left, bottom - bw + 1, bw, bw, grr);

    // top right. V
    grr.setCenter(right - bw + 1, top + bw);
    grr.setFocalPoint(grr.center());
    p->fillRect(right - bw + 1, top, bw, bw, grr);

    // bottom right. V
    grr.setCenter(right - bw + 1, bottom - bw + 1);
    grr.setFocalPoint(grr.center());
    p->fillRect(right - bw + 1, bottom - bw + 1, bw, bw, grr);


}

void SDOValueButton::drawCenter(QPainter* p)
{
    auto cr = contentsRect();

    int left = cr.left();
    int right = cr.right();
    int top = cr.top();
    int bot = cr.bottom();
    int w = cr.width();
    int h = cr.height();

    //const QColor& bc = palette().dark().color();
    //const QColor& fc = palette().button().color();

    p->setPen(Qt::NoPen);
    p->fillRect(left + m_borderWidth, top + m_borderWidth, w - 2 * m_borderWidth, h - 2 * m_borderWidth, palette().button());
}

void SDOValueButton::drawHighlight(QPainter* p)
{
    QColor btn_col = palette().highlight().color();//palette().button().color();
    QColor hl_col = palette().highlight().color();
    btn_col.setAlpha(0);
    hl_col.setAlpha(128);
    drawBorder(p, m_borderWidth / 2, btn_col, hl_col);
}

