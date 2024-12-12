#include "sdovalueindicator.h"
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


//#define SDOVALUEINDICATOR_MESSAGE_ON_WRITE_ERROR 0



SDOValueIndicator::SDOValueIndicator(CoValuesHolder* newValsHolder, QWidget* parent)
    :QWidget(parent)
{
    m_valsHolder = newValsHolder;
    m_sdoValue = nullptr;
    m_sdoValueType = COValue::Type();

    createImages();

    m_text = tr("Indicator test");
    m_borderWidth = 11; //5

    m_indicatorActive = false;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QPalette pal = palette();
    pal.setBrush(QPalette::Midlight, QColor::fromRgb(0xc7c5c1)); //
    pal.setBrush(QPalette::Light, QColor::fromRgb(0xbcfa24)); // 0xfb9972 0xfff868
    pal.setBrush(QPalette::Window, QColor::fromRgb(0xe7e7e7)); //0xaaa193 0x8c857b 0x836931
    pal.setBrush(QPalette::Shadow, Qt::black);
    setPalette(pal);

    QFont fnt = font();
    int fs = fnt.pixelSize();
    if(fs != -1){
        fnt.setPixelSize(fs * 2);
    }else{
        fs = fnt.pointSize();
        fnt.setPointSize(fs * 2);
    }
    fnt.setBold(true);
    fnt.setCapitalization(QFont::AllUppercase);
    setFont(fnt);
}

SDOValueIndicator::~SDOValueIndicator()
{
    resetSDOValue();

    deleteImages();
}

CoValuesHolder* SDOValueIndicator::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValueIndicator::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

QString SDOValueIndicator::text() const
{
    return m_text;
}

void SDOValueIndicator::setText(const QString& newText)
{
    m_text = newText;
}

QColor SDOValueIndicator::backColor() const
{
    const QPalette& pal = palette();
    return pal.window().color();
}

void SDOValueIndicator::setBackColor(const QColor& newBackColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, newBackColor);
    setPalette(pal);
}

QColor SDOValueIndicator::shadowColor() const
{
    const QPalette& pal = palette();
    return pal.shadow().color();
}

void SDOValueIndicator::setShadowColor(const QColor& newShadowColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Shadow, newShadowColor);
    setPalette(pal);
}

QColor SDOValueIndicator::indicatorColor() const
{
    const QPalette& pal = palette();
    return pal.light().color();
}

void SDOValueIndicator::setIndicatorColor(const QColor& newIndicatorColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Light, newIndicatorColor);
    setPalette(pal);
}

QColor SDOValueIndicator::glareColor() const
{
    const QPalette& pal = palette();
    return pal.midlight().color();
}

void SDOValueIndicator::setGlareColor(const QColor& newGlareColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Midlight, newGlareColor);
    setPalette(pal);
}

QColor SDOValueIndicator::textColor() const
{
    const QPalette& pal = palette();
    return pal.text().color();
}

void SDOValueIndicator::setTextColor(const QColor& newTextColor)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Text, newTextColor);
    setPalette(pal);
}

int SDOValueIndicator::borderWidth() const
{
    return m_borderWidth;
}

void SDOValueIndicator::setBorderWidth(int newBorderWidth)
{
    m_borderWidth = newBorderWidth;
}

int SDOValueIndicator::fontPointSize() const
{
    return font().pointSize();
}

void SDOValueIndicator::setFontPointSize(int newSize)
{
    QFont fnt = font();

    fnt.setPointSize(newSize);

    setFont(fnt);
}

bool SDOValueIndicator::fontCapitalization() const
{
    return font().capitalization();
}

void SDOValueIndicator::setFontCapitalization(bool newCap)
{
    QFont fnt = font();

    fnt.setCapitalization(newCap ? (QFont::AllUppercase) : (QFont::MixedCase));

    setFont(fnt);
}

bool SDOValueIndicator::fontBold() const
{
    return font().bold();
}

void SDOValueIndicator::setFontBold(bool newBold)
{
    QFont fnt = font();

    fnt.setBold(newBold);

    setFont(fnt);
}

SDOValueIndicator::CompareType SDOValueIndicator::indicatorCompare() const
{
    return m_indicatorCompare;
}

void SDOValueIndicator::setIndicatorCompare(CompareType newIndicatorCompare)
{
    m_indicatorCompare = newIndicatorCompare;
}

uint32_t SDOValueIndicator::indicatorValue() const
{
    return m_indicatorValue;
}

void SDOValueIndicator::setIndicatorValue(uint32_t newIndicatorValue)
{
    m_indicatorValue = newIndicatorValue;
}

bool SDOValueIndicator::setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType)
{
    if(m_valsHolder == nullptr) return false;
    if(m_sdoValue) resetSDOValue();

    size_t typeSize = COValue::typeSize(newType);
    if(typeSize == 0) return false;

    m_sdoValueType = newType;

    m_sdoValue = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, typeSize);
    if(m_sdoValue == nullptr){
        resetSDOValue();
        return false;
    }

    connect(m_sdoValue, &SDOValue::readed, this, &SDOValueIndicator::sdovalueReaded);

    return true;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueIndicator::getSDOValue()
{
    return m_sdoValue;
}

CoValuesHolder::HoldedSDOValuePtr SDOValueIndicator::getSDOValue() const
{
    return m_sdoValue;
}

COValue::Type SDOValueIndicator::SDOValueType() const
{
    return m_sdoValueType;
}

void SDOValueIndicator::resetSDOValue()
{
    if(m_sdoValue != nullptr){
        disconnect(m_sdoValue, &SDOValue::readed, this, &SDOValueIndicator::sdovalueReaded);

        if(m_valsHolder != nullptr) m_valsHolder->delSdoValue(m_sdoValue);
        m_sdoValue = nullptr;
    }
}

bool SDOValueIndicator::indicatorActive() const
{
    return m_indicatorActive;
}

void SDOValueIndicator::setIndicatorActive(bool newActive)
{
    m_indicatorActive = newActive;
}

QSize SDOValueIndicator::sizeHint() const
{
    QFontMetrics fm(font());
    QRect r = fm.tightBoundingRect(text());

    return QSize(m_borderWidth * 2 + r.width(), m_borderWidth * 2 + r.height());
}

void SDOValueIndicator::applyAppearance()
{
    updateImages();
    update();
}

void SDOValueIndicator::sdovalueReaded()
{
    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    auto value = COValue::valueFrom<uint32_t>(sdoval->data(), m_sdoValueType, 0);

    bool curActive = m_indicatorActive;
    bool newActive = false;

    switch(m_indicatorCompare){
    case NOT_EQUAL:
        newActive = (value != m_indicatorValue);
        break;
    default:
        m_indicatorCompare = EQUAL;
    [[fallthrough]];
    case EQUAL:
        newActive = (value == m_indicatorValue);
        break;
    case LESS:
        newActive = (value < m_indicatorValue);
        break;
    case LESS_EQUAL:
        newActive = (value <= m_indicatorValue);
        break;
    case GREATER:
        newActive = (value > m_indicatorValue);
        break;
    case GREATER_EQUAL:
        newActive = (value >= m_indicatorValue);
        break;
    case MASK:
        newActive = ((value & m_indicatorValue) == m_indicatorValue);
        break;
    case MASK_ZERO:
        newActive = ((value & m_indicatorValue) == 0);
        break;
    }

    m_indicatorActive = newActive;

    if(newActive != curActive){
        update();
    }
}

void SDOValueIndicator::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    deleteImages();
    createImages();
    updateImages();
}

void SDOValueIndicator::paintEvent(QPaintEvent* event)
{
    //QWidget::paintEvent(event);

    if(contentsRect().isNull()) return;
    if(event->region().isNull()) return;

    QPainter painter(this);
    painter.setClipRegion(event->region());

    drawIndicator(&painter);

    /*painter.fillRect(rect(), Qt::magenta);
    drawHighlight(&painter);*/
}

QColor SDOValueIndicator::lerpColor(const QColor& ca, const QColor& cb, int t)
{
    int ra = ca.red();
    int rb = cb.red();

    int ga = ca.green();
    int gb = cb.green();

    int ba = ca.blue();
    int bb = cb.blue();

    int aa = ca.alpha();
    int ab = cb.alpha();

    int r = ra + (((rb - ra) * t) >> 8);
    int g = ga + (((gb - ga) * t) >> 8);
    int b = ba + (((bb - ba) * t) >> 8);
    int a = aa + (((ab - aa) * t) >> 8);

    return QColor(r, g, b, a);
}

void SDOValueIndicator::createImages()
{
    auto cr = contentsRect();

    int w = qMax(cr.width(), 1);
    int h = qMax(cr.height(), 1);

    auto pfmt = QImage::Format_ARGB32_Premultiplied;

    m_imgBuffer = new QImage(w, h, pfmt);
    m_imgNormal = new QImage(w, h, pfmt);
    m_imgActive = new QImage(w, h, pfmt);
}

void SDOValueIndicator::deleteImages()
{
    delete m_imgBuffer;
    delete m_imgNormal;
    delete m_imgActive;
}

void SDOValueIndicator::updateImages()
{
    m_imgBuffer->fill(0);
    m_imgNormal->fill(0);
    m_imgActive->fill(0);

    QPainter p_normal(m_imgNormal);
    drawNormal(&p_normal);

    QPainter p_checked(m_imgActive);
    drawActive(&p_checked);
}

void SDOValueIndicator::drawIndicator(QPainter* p)
{
    QPainter pi(m_imgBuffer);

    pi.fillRect(m_imgBuffer->rect(), palette().window().color());

    if(indicatorActive()){
        pi.drawImage(0, 0, *m_imgActive);
    }else{
        pi.drawImage(0, 0, *m_imgNormal);
    }

    drawText(&pi);

    p->drawImage(contentsRect(), *m_imgBuffer);

//    p->setPen(Qt::magenta);
//    p->setBrush(Qt::NoBrush);
//    p->drawRect(rect());
}

void SDOValueIndicator::drawText(QPainter* p)
{
    QColor fc = palette().text().color();
    fc.setAlpha(192);

    const int tflags = Qt::TextWordWrap | //Qt::TextWrapAnywhere |
                 Qt::AlignHCenter | Qt::AlignVCenter;

    QRect r = p->viewport();
    r.moveTo(m_borderWidth, m_borderWidth);
    r.setWidth(r.width() - 2 * m_borderWidth);
    r.setHeight(r.height() - 2 * m_borderWidth);

    p->setFont(font());
    p->setPen(fc);
    p->drawText(r, tflags, text());
}

void SDOValueIndicator::drawNormal(QPainter* p)
{
    QColor fc = palette().midlight().color();
    QColor sc = palette().shadow().color();

    drawGrad(p, m_borderWidth, fc, sc, 0.4);
}

void SDOValueIndicator::drawActive(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    QRect cr = p->viewport();

    const QColor& lc = palette().light().color();
    QColor fc = palette().midlight().color();
    QColor bc = Qt::transparent; //palette().indicator().color();
    QColor sc = palette().shadow().color();

    drawGrad(p, m_borderWidth, fc, sc, 0.4);

    //int bw = qMax(width(), height());
    int bw = (width() + height()) / 2;

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, lc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw);

    grr.setCenter(cr.center());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}

void SDOValueIndicator::drawGrad(QPainter* p, int borderWidth, const QColor& glareCol, const QColor& shadowCol, qreal glarePos)
{
    auto cr = p->viewport();

    int left = cr.left();
    int right = cr.right();
    int top = cr.top();
    int bottom = cr.bottom();
    int w = cr.width();
    int h = cr.height();

    const int bw = borderWidth;
    const int bws = bw * glarePos;
    //const int bwl = bw - bws;

    QColor bc = Qt::transparent;
    QColor gc = glareCol; //gc.setAlpha(128);
    QColor sc = shadowCol; //sc.setAlpha(128);


    // border rectangles.
    QLinearGradient grl;

    gc.setAlphaF(0.7);
    sc.setAlphaF(0.5);
    grl.setColorAt(0, sc);
    grl.setColorAt(1.0, bc);

    // left. V
    grl.setStart(left + 1, top);
    grl.setFinalStop(left + bw, top);
    //p->fillRect(left, top + bw, bw, h - 2 * bw, grl);
    p->fillRect(left, top, bw, h, grl);

    // up. V
    grl.setStart(right, top + 1);
    grl.setFinalStop(right, top + bw);
    //p->fillRect(left + bw, top, w - 2 * bw, bw, grl);
    p->fillRect(left, top, w, bw, grl);

    grl.setColorAt(0, gc);
    grl.setColorAt(1.0, bc);

    // down. V
    grl.setStart(left, bottom);
    grl.setFinalStop(left, bottom - bw + 1);
    //p->fillRect(left + bw, bottom - bw + 1, w - 2 * bw, bw, grl);
    p->fillRect(left, bottom - bw + 1, w, bw, grl);

    // right. V
    grl.setStart(right, top);
    grl.setFinalStop(right - bw + 1, top);
    //p->fillRect(right - bw + 1, top + bw, bw, h - 2 * bw, grl);
    p->fillRect(right - bw + 1, top, bw, h, grl);

    gc.setAlphaF(0.4);
    sc.setAlphaF(0.4);
    grl.setColorAt(0, gc);
    grl.setColorAt(1.0, bc);

    // left. V
    grl.setStart(left + 1, top);
    grl.setFinalStop(left + bws, top);
    //p->fillRect(left, top + bws, bws, h - 2 * bws, grl);
    p->fillRect(left, top, bws, h, grl);

    // up. V
    grl.setStart(right, top + 1);
    grl.setFinalStop(right, top + bws);
    //p->fillRect(left + bws, top, w - 2 * bws, bws, grl);
    p->fillRect(left, top, w, bws, grl);

    grl.setColorAt(0, sc);
    grl.setColorAt(1.0, bc);

    // down. V
    grl.setStart(left, bottom);
    grl.setFinalStop(left, bottom - bws + 1);
    //p->fillRect(left + bws, bottom - bws + 1, w - 2 * bws, bws, grl);
    p->fillRect(left, bottom - bws + 1, w, bws, grl);

    // right. V
    grl.setStart(right, top);
    grl.setFinalStop(right - bws + 1, top);
    //p->fillRect(right - bws + 1, top + bws, bws, h - 2 * bws, grl);
    p->fillRect(right - bws + 1, top, bws, h, grl);

    gc.setAlphaF(0.2);
    sc.setAlphaF(0.2);
    grl.setColorAt(0, sc);
    grl.setColorAt(1.0, gc);

    grl.setStart(left, top);
    grl.setFinalStop(left, top + h);
    p->fillRect(left, top, w, h, grl);
}


