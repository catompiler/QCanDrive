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

    createImages();

    m_borderWidth = 5;

    m_mouseFlag = false;
    m_clickFlag = false;

    setCheckable(true);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QPalette pal = palette();
    pal.setBrush(QPalette::Midlight, QColor::fromRgb(0xc7c5c1)); //
    pal.setBrush(QPalette::Light, QColor::fromRgb(0xfb9972)); //0xfff868
    pal.setBrush(QPalette::Button, QColor::fromRgb(0x8c857b)); // 0x836931
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

    //setText("TEST");
}

SDOValueButton::~SDOValueButton()
{
    if(m_sdoValue){
        resetSDOValue();
        delete m_sdoValue;
    }

    deleteImages();
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

void SDOValueButton::mousePressEvent(QMouseEvent* event)
{
    //qDebug() << "mousePressEvent";

    if (event->button() == Qt::LeftButton){
        m_clickFlag = true;
    }

    QAbstractButton::mousePressEvent(event);

    //update();
}

void SDOValueButton::mouseReleaseEvent(QMouseEvent* event)
{
    //qDebug() << "mouseReleaseEvent";

    if (event->button() == Qt::LeftButton && m_clickFlag){
        m_clickFlag = false;
    }

    QAbstractButton::mouseReleaseEvent(event);

    //update();
}

void SDOValueButton::enterEvent(QEvent* event)
{
    m_mouseFlag = true;

    QAbstractButton::enterEvent(event);
}

void SDOValueButton::leaveEvent(QEvent* event)
{
    m_mouseFlag = false;

    QAbstractButton::leaveEvent(event);
}

void SDOValueButton::resizeEvent(QResizeEvent* event)
{
    QAbstractButton::resizeEvent(event);

    deleteImages();
    createImages();
    updateImages();
}

void SDOValueButton::paintEvent(QPaintEvent* event)
{
    //QAbstractButton::paintEvent(event);

    QPainter painter(this);
    painter.setClipRegion( event->region() );

    drawButton(&painter);
}

QColor SDOValueButton::lerpColor(const QColor& ca, const QColor& cb, int t)
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

void SDOValueButton::createImages()
{
    int w = width();
    int h = height();

    auto pfmt = QImage::Format_ARGB32_Premultiplied;

    m_imgNormal = new QImage(w, h, pfmt);
    m_imgChecked = new QImage(w, h, pfmt);
    m_imgNormalClicked = new QImage(w, h, pfmt);
    m_imgCheckedClicked = new QImage(w, h, pfmt);
    m_imgMouse = new QImage(w, h, pfmt);
    m_imgFocus = new QImage(w, h, pfmt);

    m_imgNormal->fill(0);
    m_imgChecked->fill(0);
    m_imgNormalClicked->fill(0);
    m_imgCheckedClicked->fill(0);
    m_imgMouse->fill(0);
    m_imgFocus->fill(0);
}

void SDOValueButton::deleteImages()
{
    delete m_imgNormal;
    delete m_imgChecked;
    delete m_imgNormalClicked;
    delete m_imgCheckedClicked;
    delete m_imgMouse;
    delete m_imgFocus;
}

void SDOValueButton::updateImages()
{
    QPainter p_normal(m_imgNormal);
    drawNormal(&p_normal);
    drawBorder(&p_normal);

    QPainter p_checked(m_imgChecked);
    drawChecked(&p_checked);
    drawBorder(&p_checked);

    QPainter p_click(m_imgNormalClicked);
    drawNormalClicked(&p_click);
    drawBorder(&p_click);

    QPainter p_ch_click(m_imgCheckedClicked);
    drawCheckedClicked(&p_ch_click);
    drawBorder(&p_ch_click);

    QPainter p_mouse(m_imgMouse);
    drawMouse(&p_mouse);

    QPainter p_focus(m_imgFocus);
    drawHighlight(&p_focus);
}

void SDOValueButton::drawButton(QPainter* p)
{
    //drawBack(p);

    auto cr = contentsRect();

    if(isChecked()){
        if(m_clickFlag){
            p->drawImage(cr, *m_imgCheckedClicked, cr);
        }else{
            p->drawImage(cr, *m_imgChecked, cr);
        }
    }else{
        if(m_clickFlag){
            p->drawImage(cr, *m_imgNormalClicked, cr);
        }else{
            p->drawImage(cr, *m_imgNormal, cr);
        }
    }

    //qDebug() << m_clickFlag;

    if(m_mouseFlag){
        p->drawImage(cr, *m_imgMouse, cr);
    }

    if(hasFocus()){
        p->drawImage(cr, *m_imgFocus, cr);
    }

    drawText(p);

//    p->setPen(Qt::magenta);
//    p->setBrush(Qt::NoBrush);
//    p->drawRect(rect());

    //drawBorderGrad(p, m_borderWidth, palette().button().color(), palette().shadow().color());
    //
}

void SDOValueButton::drawText(QPainter* p)
{
    p->setPen(palette().buttonText().color());
    p->drawText(contentsRect(), Qt::AlignHCenter | Qt::AlignVCenter, text());
}

void SDOValueButton::drawChecked(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    const QColor& fc = palette().light().color();
    const QColor& bc = palette().button().color();
    //const QColor& bc = QColor(fc.red(), fc.green(), fc.blue(), 0);//palette().button().color();

    int bw = qMin(width(), height());

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, fc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw);

    QRect cr = contentsRect();

    grr.setCenter(cr.center());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}

void SDOValueButton::drawBorder(QPainter* p)
{
    const int border_k = 4;
    const int bw_lrg = m_borderWidth;
    const int bw_sml = m_borderWidth / border_k;
    const QColor& glare_col = palette().midlight().color();
    const QColor& button_col = palette().button().color();
    const QColor& border_col = palette().shadow().color();
    QColor glare_col_transp = lerpColor(border_col, glare_col, 96);
    QColor border_small_col = lerpColor(border_col, button_col, 128);
    QColor glare_small_col_transp = lerpColor(border_small_col, glare_col, 96);

    QRect r = contentsRect();
    QPoint grad_center;

    if(r.width() == 0){
        grad_center = r.bottomLeft();
    }else if(r.height() == 0){
        grad_center = r.topRight();
    }else{
        qreal a = r.width();
        qreal b = r.height();
        qreal k = a / b;
        qreal hx = a / (1 + k * k);
        qreal hy = k * hx;
        const qreal scale = 1.4142;
        QPoint v = QPoint(hx * scale, hy * scale);
        grad_center = (r.topLeft() + v);
    }
    //qDebug() << grad_center << r.width() << r.height();

    p->setBrush(Qt::NoBrush);

    QLinearGradient grl;
    grl.setStart(r.topLeft());
    grl.setFinalStop(grad_center); //r.bottomRight()
    grl.setColorAt(0, glare_col_transp);
    grl.setColorAt(1.0, border_col);

    QPen pen;
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setBrush(QBrush(grl));
    p->setPen(pen);

    drawTickRect(p, r, bw_lrg);

    grl.setStart(r.topLeft() + QPoint(bw_lrg + bw_sml, bw_lrg + bw_sml));
    grl.setColorAt(0, glare_small_col_transp);
    grl.setColorAt(1.0, border_small_col);
    pen.setBrush(QBrush(grl));
    p->setPen(pen);

    r.moveLeft(r.left() + bw_lrg + bw_sml);
    r.moveTop(r.top() + bw_lrg + bw_sml);
    r.setWidth(r.width() - 2 * (bw_lrg + bw_sml));
    r.setHeight(r.height() - 2 * (bw_lrg + bw_sml));

    drawTickRect(p, r, bw_sml);

    //p->setBrush(Qt::magenta);
    //p->drawEllipse(grad_center, 5, 5);
    //p->drawLine(contentsRect().bottomLeft(), contentsRect().topRight());
}

void SDOValueButton::drawTickRect(QPainter* p, const QRect& r, int w)
{
    QPen pen(p->pen());
    pen.setWidth(w);
    p->setPen(pen);

    int hw = w / 2;
    int left = r.left() + hw;
    int width = r.width() - (w);
    int top = r.top() + hw;
    int height = r.height() - (w);

    p->drawRect(left, top, width, height);
}

void SDOValueButton::drawNormal(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    auto cr = contentsRect();

    const QColor& fc = palette().midlight().color();
    const QColor& bc = palette().button().color();

    int bw = qMin(cr.width(), cr.height());

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, fc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw);

    grr.setCenter(cr.center());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}

void SDOValueButton::drawBorderGrad(QPainter* p, int borderWidth, const QColor& btnCol, const QColor& brdrCol)
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

void SDOValueButton::drawHighlight(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    QColor btn_col = palette().button().color();
    QColor hl_col = palette().highlight().color();
    btn_col.setAlpha(0);
    hl_col.setAlpha(96);
    drawBorderGrad(p, m_borderWidth / 2, btn_col, hl_col);
}

void SDOValueButton::drawMouse(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    auto cr = contentsRect();

    QColor front_col = palette().highlight().color();
    QColor back_col = palette().button().color();
    front_col.setAlpha(96);
    back_col.setAlpha(0);

    int bw = qMin(cr.width(), cr.height());

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, front_col);
    grr.setColorAt(1.0, back_col);
    grr.setRadius(bw);

    grr.setCenter(cr.left() + cr.width() / 2, cr.bottom());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}

void SDOValueButton::drawNormalClicked(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    auto cr = contentsRect();

    const QColor& bc = palette().midlight().color();
    const QColor& fc = palette().button().color();

    int bw = qMin(cr.width(), cr.height());

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, fc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw);

    grr.setCenter(cr.center());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}

void SDOValueButton::drawCheckedClicked(QPainter* p)
{
    //p->fillRect(rect(), QColor(0, 0, 0, 0));

    const QColor& bc = palette().light().color();
    const QColor& fc = palette().button().color();
    //const QColor& bc = QColor(fc.red(), fc.green(), fc.blue(), 0);//palette().button().color();

    int bw = qMin(width(), height());

    QRadialGradient grr;
    grr.setSpread(QRadialGradient::PadSpread);
    grr.setColorAt(0, fc);
    grr.setColorAt(1.0, bc);
    grr.setRadius(bw);

    QRect cr = contentsRect();

    grr.setCenter(cr.center());
    grr.setFocalPoint(grr.center());
    p->fillRect(cr, grr);
}
