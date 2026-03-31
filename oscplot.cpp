#include "oscplot.h"
#include "oscplotdata.h"
#include <QwtPlotCanvas>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtPlotItem>
#include <QwtPlotLayout>
#include <QwtLinearScaleEngine>
#include <QwtSeriesData>
#include <QwtScaleEngine>
#include <QwtScaleWidget>
#include <QwtPlotLegendItem>
#include <QFontMetrics>
#include <QwtPlotPanner>
#include <QwtPlotZoomer>
#include <QwtPlotMagnifier>
#include <QDebug>


static const qreal defaultLineWidthF = 1.5;


static const Qt::GlobalColor m_colors[] = {
           Qt::yellow,
           Qt::green,
           Qt::red,
           Qt::blue,
           Qt::cyan,
           Qt::magenta,
           Qt::darkYellow,
           Qt::darkGreen,
           Qt::darkRed,
           Qt::darkBlue,
           Qt::darkCyan,
           Qt::darkMagenta,
           Qt::black,
           Qt::darkGray,
           Qt::gray,
           Qt::lightGray,
           Qt::white
        };

const int ColorsCount = static_cast<int>(sizeof(m_colors) / sizeof(m_colors[0]));

OscPlot::OscPlot(QWidget* parent, const QString& newName)
    :QwtPlot(parent)
{
    setTitle(newName);

    m_defaultAlpha = 0.75;
    m_legendItem = nullptr;

    // Игнорирование размеров.
    setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    // Автоматически удалять содержимое.
    setAutoDelete(true);

    // Холст.
    QwtPlotCanvas* canv = qobject_cast<QwtPlotCanvas*>(canvas());
    if(canv == nullptr){
        canv = new QwtPlotCanvas();
        setCanvas(canv);
    }
    canv->setFrameStyle(QFrame::Plain | QFrame::NoFrame);
    setCanvasBackground(QColor(Qt::darkGray).darker()); //QColor(112, 112, 112)

    // Размещение.
    QwtPlotLayout* pltLay = plotLayout();
    if(pltLay == nullptr){
        pltLay = new QwtPlotLayout();
        setPlotLayout(pltLay);
    }
    pltLay->setAlignCanvasToScales(true);

    // Пример https://forum.qt.io/topic/54848/qwtplot-zooming-logarithmic-scale
    // Оси.
    // Движок масштабирования.
    QwtLinearScaleEngine* scaleEng_x = new QwtLinearScaleEngine();
    QwtLinearScaleEngine* scaleEng_y = new QwtLinearScaleEngine();
    scaleEng_x->setAttribute(QwtScaleEngine::Floating);
    scaleEng_y->setAttribute(QwtScaleEngine::Floating);
    scaleEng_x->setMargins(0.0, 0.0);
    scaleEng_y->setMargins(0.0, 0.0);
    setAxisScaleEngine(QwtPlot::xBottom, scaleEng_x);
    setAxisScaleEngine(QwtPlot::yLeft, scaleEng_y);
    // Названия осей.
    setAxisTitle(QwtPlot::xBottom, tr("Время, с"));
    setAxisTitle(QwtPlot::yLeft, tr("Значение"));
    // Масштаб.
    setAxisScale(QwtAxis::XBottom, 0.0, 1.0);
    setAxisScale(QwtAxis::YLeft, 0.0, 1.0);
    setAxisAutoScale(QwtAxis::XBottom, true);
    setAxisAutoScale(QwtAxis::YLeft, true);
    //setAxisMaxMajor(QwtAxis::XBottom, 3);
    //setAxisMaxMajor(QwtAxis::YLeft, 3);
    //setAxisScale(QwtPlot::yLeft, 1e-6, 1e3);

    // Сетка.
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableY(true);
    grid->enableYMin(true);
    grid->enableX(true);
    grid->enableXMin(true);
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
    grid->attach(this);

    // Паномарирование.
    QwtPlotPanner* panner = new QwtPlotPanner(canvas());
    panner->setMouseButton(Qt::RightButton);

    // Зум.
    m_zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas());
    // Рамка.
    QPen rbPen;
    rbPen.setColor(Qt::white);
    rbPen.setStyle(Qt::DotLine);
    m_zoomer->setRubberBandPen(rbPen);
    // Текст трека.
    QPen trPen;
    trPen.setColor(Qt::white);
    trPen.setStyle(Qt::DotLine);
    m_zoomer->setTrackerPen(trPen);
    // LeftButton для рамки.
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::NoModifier);
    // MiddleButton для сброса.
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::MiddleButton, Qt::NoModifier);
    // Назад - BackButton.
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::BackButton, Qt::NoModifier);
    // Вперёд - ForwardButton.
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect6, Qt::ForwardButton, Qt::NoModifier);

    // Увеличение.
    QwtPlotMagnifier* magn_x = new QwtPlotMagnifier(canvas());
    QwtPlotMagnifier* magn_y = new QwtPlotMagnifier(canvas());
    // Shift + MouseWheel -> Magnifier x
    magn_x->setMouseButton(Qt::NoButton, Qt::NoModifier);
    magn_x->setWheelModifiers(Qt::ShiftModifier);
    magn_x->setAxisEnabled(QwtPlot::xBottom, true);
    magn_x->setAxisEnabled(QwtPlot::yLeft, false);
    // CTRL + MouseWheel -> Magnifier y
    magn_y->setMouseButton(Qt::NoButton, Qt::NoModifier);
    magn_y->setWheelModifiers(Qt::ControlModifier);
    magn_y->setAxisEnabled(QwtPlot::xBottom,false);
    magn_y->setAxisEnabled(QwtPlot::yLeft,true);

    /*
       In situations, when there is a label at the most right position of the
       scale, additional space is needed to display the overlapping part
       of the label would be taken by reducing the width of scale and canvas.
       To avoid this "jumping canvas" effect, we add a permanent margin.
       We don't need to do the same for the left border, because there
       is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget* scaleWidget_xb = axisWidget( QwtAxis::XBottom );
    scaleWidget_xb->setMinBorderDist( 0, QFontMetrics( scaleWidget_xb->font() ).averageCharWidth() * 2 );

    QwtScaleWidget* scaleWidget_yl = axisWidget( QwtAxis::YLeft );
    scaleWidget_yl->setMinBorderDist( QFontMetrics( scaleWidget_yl->font() ).averageCharWidth() * 2, 0 );

    setAutoReplot(false);
}

OscPlot::~OscPlot()
{

}

QString OscPlot::name() const
{
    return title().text();
}

void OscPlot::setName(const QString& newName)
{
    setTitle(newName);
}

qreal OscPlot::defaultAlpha() const
{
    return m_defaultAlpha;
}

void OscPlot::setDefaultAlpha(qreal newDefaultAlpha)
{
    m_defaultAlpha = newDefaultAlpha;
}

QBrush OscPlot::background() const
{
    return canvasBackground();
}

void OscPlot::setBackground(const QBrush& newBrush)
{
    setCanvasBackground(newBrush);

    updateLegendItem();
}

QColor OscPlot::textColor() const
{
    return palette().color(QPalette::Text);
}

void OscPlot::setTextColor(const QColor& newColor)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Text, newColor);
    setPalette(pal);

    updateLegendItem();
}

int OscPlot::addSignal(OscPlotData* pltData, const QString& newName, const QColor& newColor, const qreal& z)
{
    // Если нет данных - нет и сигнала.
    if(pltData == nullptr) return -1;

    int curvesCount = signalsCount();

    QwtPlotCurve* newCurve = new QwtPlotCurve(newName);
    if(z < 0){
        newCurve->setZ(curvesCount);
    }else{
        newCurve->setZ(z);
    }

    QColor curveColor;
    if(newColor.isValid()){
        curveColor = newColor;
    }else{
        curveColor = m_colors[curvesCount % ColorsCount];
    }

    if(m_defaultAlpha > 0.0 && m_defaultAlpha <= 1.0){
        curveColor.setAlphaF(m_defaultAlpha);
    }

    QPen curvePen;
    curvePen.setStyle(Qt::SolidLine);
    curvePen.setWidthF(defaultLineWidthF);
    curvePen.setColor(curveColor);
    QBrush curveBrush;
    curveBrush.setStyle(Qt::SolidPattern);
    curveBrush.setColor(curveColor);

    newCurve->setPen(curvePen);
    newCurve->setBrush(curveBrush);
    newCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    newCurve->setSamples(pltData);
    newCurve->attach(this);

    return curvesCount;
}

void OscPlot::removeSignal(int n)
{
    if(n == -1){
        detachItems(QwtPlotItem::Rtti_PlotCurve, true);
        return;
    }

    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->detach();
}

void OscPlot::removeSignals()
{
    //while(signalsCount()) removeSignal(0);
    removeSignal(-1);
}

int OscPlot::signalsCount() const
{
    return itemList(QwtPlotItem::Rtti_PlotCurve).count();
}

OscPlotData* OscPlot::plotData(int n)
{
    QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return nullptr;

    OscPlotData* pltData = static_cast<OscPlotData*>(curv->data());

    return pltData;
}

const OscPlotData* OscPlot::plotData(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return nullptr;

    const OscPlotData* pltData = static_cast<const OscPlotData*>(curv->data());

    return pltData;
}

bool OscPlot::signalVisible(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return false;

    return curv->isVisible();
}

void OscPlot::setSignalVisible(int n, bool newVisible)
{
    QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return;

    curv->setVisible(newVisible);
}

QString OscPlot::signalName(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QString();

    return curv->title().text();
}

void OscPlot::setSignalName(int n, const QString& newName)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setTitle(newName);
}

qreal OscPlot::z(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->z();
}

void OscPlot::setZ(int n, const qreal& newZ)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setZ(newZ);
}

QwtPlotCurve::CurveStyle OscPlot::curveStyle(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QwtPlotCurve::NoCurve;

    return curv->style();
}

void OscPlot::setCurveStyle(int n, QwtPlotCurve::CurveStyle newStyle)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setStyle(newStyle);
}

QPen OscPlot::pen(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QPen();

    return curv->pen();
}

void OscPlot::setPen(int n, const QPen& newPen)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setPen(newPen);
}

QBrush OscPlot::brush(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QBrush();

    return curv->brush();
}

void OscPlot::setBrush(int n, const QBrush& newBrush)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBrush(newBrush);
}

qreal OscPlot::baseLine(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->baseline();
}

void OscPlot::setBaseLine(int n, qreal newBaseLine)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBaseline(newBaseLine);
}

void OscPlot::setBaseLine(qreal newBaseLine)
{
    for(int i = 0; i < signalsCount(); i ++){
        setBaseLine(i, newBaseLine);
    }
}

QRectF OscPlot::boundingRect(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QRectF(0, 0, -1, -1);

    return curv->dataRect();
}

QRectF OscPlot::boundingRect() const
{
    QRectF resRect;

    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto pltData = static_cast<OscPlotData*>(curv->data());

        QRectF rect = pltData->boundingRect();

        resRect = resRect.united(rect);
    }

    return resRect;
}

void OscPlot::updateZoomBaseSize()
{
    QRectF bounds = boundingRect();

    //qDebug() << bounds;

    m_zoomer->setZoomBase(bounds);
}

void OscPlot::clear()
{
    removeSignals();

    //replot();
}

bool OscPlot::legendItemEnabled() const
{
    return m_legendItem != nullptr;
}

void OscPlot::setLegendItemEnabled(bool newEnabled)
{
    if(newEnabled){
        if(m_legendItem == nullptr){
            m_legendItem = new QwtPlotLegendItem();
            m_legendItem->attach(this);
            // setup.
            updateLegendItem();
        }
    }else{
        if(m_legendItem != nullptr){
            delete m_legendItem;
            m_legendItem = nullptr;
        }
    }
}

QList<Qt::GlobalColor> OscPlot::getDefaultColors()
{
    QList<Qt::GlobalColor> colors;

    for(int i = 0; i < ColorsCount; i ++){
        colors.append(m_colors[i]);
    }

    return colors;
}

int OscPlot::findCurve(const QwtPlotCurve* findCurv) const
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    int n = 0;
    for(auto& item: items){
        const auto& curv = static_cast<const QwtPlotCurve*>(item);
        if(curv == findCurv) return n;
        n ++;
    }

    return -1;
}

QwtPlotCurve* OscPlot::getCurve(int n)
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

const QwtPlotCurve* OscPlot::getCurve(int n) const
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

void OscPlot::updateLegendItem()
{
    if(m_legendItem == nullptr) return;

    m_legendItem->setMaxColumns(1);
    m_legendItem->setAlignmentInCanvas(Qt::AlignRight | Qt::AlignVCenter);
    m_legendItem->setBackgroundMode(QwtPlotLegendItem::LegendBackground);

    m_legendItem->setBorderRadius(8);
    m_legendItem->setMargin(4);
    m_legendItem->setSpacing(2);
    m_legendItem->setItemMargin(0);

    QFont font = m_legendItem->font();
    font.setPointSize(font.pointSize());
    m_legendItem->setFont( font );

    m_legendItem->setTextPen(textColor());
    QColor bgCol = background().color();
    bgCol.setAlpha(200);
    m_legendItem->setBackgroundBrush(bgCol);
    m_legendItem->setBorderPen(bgCol);
}
