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



TrendPlot::TrendPlot(QWidget* parent)
    :QwtPlot(parent)
{
    setAutoDelete(true);

    auto myCanvas = qobject_cast<QwtPlotCanvas*>(canvas());
    if(!myCanvas){
        myCanvas = new QwtPlotCanvas();
        setCanvas(myCanvas);
    }
    myCanvas->setFrameStyle(QFrame::Plain | QFrame::NoFrame);

    plotLayout()->setAlignCanvasToScales( true );
}

TrendPlot::~TrendPlot()
{

}

int TrendPlot::addTrend(const SequentialBuffer* newBuffer, qreal z)
{
    int curvesCount = trendsCount();

    QwtPlotCurve* newCurve = new QwtPlotCurve();
    if(z < 0){
        newCurve->setZ(curvesCount);
    }else{
        newCurve->setZ(z);
    }

    newCurve->setPen(QPen(Qt::NoPen));
    newCurve->setBrush(QBrush(Qt::SolidPattern));

    newCurve->setSamples(new TrendSeriesData(newBuffer));
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

QwtPlotCurve::CurveStyle TrendPlot::style(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QwtPlotCurve::NoCurve;

    return curv->style();
}

void TrendPlot::setStyle(int n, QwtPlotCurve::CurveStyle newStyle)
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

QRectF TrendPlot::boundingRect(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QRectF(0, 0, -1, -1);

    return curv->dataRect();
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
