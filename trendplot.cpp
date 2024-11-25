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

int TrendPlot::addTrend(const SequentialBuffer* newBuffer)
{
    QwtPlotCurve* newCurve = new QwtPlotCurve();
    newCurve->setSamples(new TrendSeriesData(newBuffer));
    newCurve->attach(this);

    return itemList(QwtPlotItem::Rtti_PlotCurve).count() - 1;
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

QwtPlotCurve* TrendPlot::getCurve(int n)
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}
