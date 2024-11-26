#include "trendplot.h"
#include <QwtPlotCanvas>
#include <QwtPlotCurve>
#include <QwtPlotItem>
#include <QwtPlotLayout>
#include <QwtSeriesData>
#include <QwtScaleEngine>
#include "sequentialbuffer.h"
#include "trendseriesdata.h"
#include <QDebug>

const Qt::GlobalColor m_colors[] = {
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

TrendPlot::TrendPlot(QWidget* parent)
    :QwtPlot(parent)
{
    m_size = 0;
    m_defaultAlpha = 1.0;

    setAutoDelete(true);

    auto myCanvas = qobject_cast<QwtPlotCanvas*>(canvas());
    if(!myCanvas){
        myCanvas = new QwtPlotCanvas();
        setCanvas(myCanvas);
    }
    myCanvas->setFrameStyle(QFrame::Plain | QFrame::NoFrame);
    plotLayout()->setAlignCanvasToScales(true);
    setAxisScale(QwtAxis::XBottom, 0.0, 1.0);
    setAxisScale(QwtAxis::YLeft, 0.0, 1.0);
    setAxisAutoScale(QwtAxis::XBottom, true);
    setAxisAutoScale(QwtAxis::YLeft, true);
}

TrendPlot::~TrendPlot()
{

}

size_t TrendPlot::bufferSize() const
{
    return m_size;
}

void TrendPlot::setBufferSize(size_t newSize)
{
    m_size = newSize;

    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto trendData = static_cast<TrendSeriesData*>(curv->data());
        trendData->setBufferSize(newSize);
    }
}

qreal TrendPlot::defaultAlpha() const
{
    return m_defaultAlpha;
}

void TrendPlot::setDefaultAlpha(qreal newDefaultAlpha)
{
    m_defaultAlpha = newDefaultAlpha;
}

QBrush TrendPlot::background() const
{
    return canvasBackground();
}

void TrendPlot::setBackground(const QBrush& newBrush)
{
    setCanvasBackground(newBrush);
}

int TrendPlot::addTrend(const QColor& newColor, const qreal& z, SequentialBuffer* newBuffer)
{
    int curvesCount = trendsCount();

    QwtPlotCurve* newCurve = new QwtPlotCurve();
    if(z < 0){
        newCurve->setZ(curvesCount);
    }else{
        newCurve->setZ(z);
    }

    SequentialBuffer* curveBuffer = newBuffer;
    if(curveBuffer == nullptr){
        curveBuffer = new SequentialBuffer();
        curveBuffer->setSize(m_size);
        curveBuffer->setSamplingPeriod(1.0);
        curveBuffer->setAddressingMode(SequentialBuffer::CIRCULAR);
    }else{
        if(m_size == 0) m_size = curveBuffer->size();
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
    curvePen.setWidthF(3.0);
    curvePen.setColor(curveColor);
    QBrush curveBrush;
    curveBrush.setStyle(Qt::SolidPattern);
    curveBrush.setColor(curveColor);

    newCurve->setPen(curvePen);
    newCurve->setBrush(curveBrush);
    newCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    newCurve->setSamples(new TrendSeriesData(curveBuffer));
    newCurve->attach(this);

    return curvesCount;
}

void TrendPlot::removeTrend(int n)
{
    if(n == -1){
        detachItems(QwtPlotItem::Rtti_PlotCurve, true);
        return;
    }

    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->detach();
}

int TrendPlot::trendsCount() const
{
    return itemList(QwtPlotItem::Rtti_PlotCurve).count();
}

QwtPlotCurve::CurveStyle TrendPlot::curveStyle(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QwtPlotCurve::NoCurve;

    return curv->style();
}

void TrendPlot::setCurveStyle(int n, QwtPlotCurve::CurveStyle newStyle)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setStyle(newStyle);
}

QPen TrendPlot::pen(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QPen();

    return curv->pen();
}

void TrendPlot::setPen(int n, const QPen& newPen)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setPen(newPen);
}

QBrush TrendPlot::brush(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QBrush();

    return curv->brush();
}

void TrendPlot::setBrush(int n, const QBrush& newBrush)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBrush(newBrush);
}

qreal TrendPlot::baseLine(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->baseline();
}

void TrendPlot::setBaseLine(int n, qreal newBaseLine)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBaseline(newBaseLine);
}

void TrendPlot::setBaseLine(qreal newBaseLine)
{
    for(int i = 0; i < trendsCount(); i ++){
        setBaseLine(i, newBaseLine);
    }
}

QRectF TrendPlot::boundingRect(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QRectF(0, 0, -1, -1);

    return curv->dataRect();
}

QRectF TrendPlot::boundingRect() const
{
    QRectF resRect;

    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto trendData = static_cast<TrendSeriesData*>(curv->data());

        QRectF rect = trendData->boundingRect();

        resRect = resRect.united(rect);
    }

    return resRect;
}

void TrendPlot::putSample(int n, const qreal& newY, const qreal& newDx)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    auto trendData = static_cast<TrendSeriesData*>(curv->data());

    trendData->putSample(newY, newDx);
}

QwtPlotCurve* TrendPlot::getCurve(int n)
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

const QwtPlotCurve* TrendPlot::getCurve(int n) const
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}
