#include "signalplot.h"
#include <QwtPlotCanvas>
#include <QwtPlotCurve>
#include <QwtPlotItem>
#include <QwtPlotLayout>
#include <QwtSeriesData>
#include <QwtScaleEngine>
#include <QwtScaleWidget>
#include <QwtPlotLegendItem>
#include <QFontMetrics>
#include "sequentialbuffer.h"
#include "signalseriesdata.h"
#include <QDebug>

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

SignalPlot::SignalPlot(const QString& newName, QWidget* parent)
    :QwtPlot(parent)
{
    setTitle(newName);

    m_size = 0;
    m_defaultAlpha = 0.75;

    m_legendItem = nullptr;

    setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    setAutoDelete(true);

    auto myCanvas = qobject_cast<QwtPlotCanvas*>(canvas());
    if(!myCanvas){
        myCanvas = new QwtPlotCanvas();
        setCanvas(myCanvas);
    }
    myCanvas->setFrameStyle(QFrame::Plain | QFrame::NoFrame);

    setCanvasBackground(Qt::darkGray); //QColor(112, 112, 112)

    plotLayout()->setAlignCanvasToScales(true);

    setAxisScale(QwtAxis::XBottom, 0.0, 1.0);
    setAxisScale(QwtAxis::YLeft, 0.0, 1.0);
    setAxisAutoScale(QwtAxis::XBottom, true);
    setAxisAutoScale(QwtAxis::YLeft, true);
    setAxisMaxMajor(QwtAxis::XBottom, 3);
    setAxisMaxMajor(QwtAxis::YLeft, 3);

    if(auto scaleEng = axisScaleEngine(QwtAxis::XBottom)){
        scaleEng->setAttribute(QwtScaleEngine::Floating);
    }

    /*
       In situations, when there is a label at the most right position of the
       scale, additional space is needed to display the overlapping part
       of the label would be taken by reducing the width of scale and canvas.
       To avoid this "jumping canvas" effect, we add a permanent margin.
       We don't need to do the same for the left border, because there
       is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget* scaleWidget = axisWidget( QwtAxis::XBottom );
    const int mbd = QFontMetrics( scaleWidget->font() ).averageCharWidth();
    scaleWidget->setMinBorderDist( 0, mbd * 2 );

    setAutoReplot(false);
}

SignalPlot::~SignalPlot()
{

}

QString SignalPlot::name() const
{
    return title().text();
}

void SignalPlot::setName(const QString& newName)
{
    setTitle(newName);
}

size_t SignalPlot::bufferSize() const
{
    return m_size;
}

void SignalPlot::setBufferSize(size_t newSize)
{
    m_size = newSize;

    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto trendData = static_cast<SignalSeriesData*>(curv->data());
        trendData->setBufferSize(newSize);
    }
}

qreal SignalPlot::defaultAlpha() const
{
    return m_defaultAlpha;
}

void SignalPlot::setDefaultAlpha(qreal newDefaultAlpha)
{
    m_defaultAlpha = newDefaultAlpha;
}

QBrush SignalPlot::background() const
{
    return canvasBackground();
}

void SignalPlot::setBackground(const QBrush& newBrush)
{
    setCanvasBackground(newBrush);

    updateLegendItem();
}

QColor SignalPlot::textColor() const
{
    return palette().color(QPalette::Text);
}

void SignalPlot::setTextColor(const QColor& newColor)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Text, newColor);
    setPalette(pal);

    updateLegendItem();
}

int SignalPlot::addSignal(const QString& newName, const QColor& newColor, const qreal& z, SequentialBuffer* newBuffer)
{
    int curvesCount = signalsCount();

    QwtPlotCurve* newCurve = new QwtPlotCurve(newName);
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

    curveBuffer->setSamplingPeriod(m_period);

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
    newCurve->setSamples(new SignalSeriesData(curveBuffer));
    newCurve->attach(this);

    return curvesCount;
}

void SignalPlot::removeSignal(int n)
{
    if(n == -1){
        detachItems(QwtPlotItem::Rtti_PlotCurve, true);
        return;
    }

    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->detach();
}

int SignalPlot::signalsCount() const
{
    return itemList(QwtPlotItem::Rtti_PlotCurve).count();
}

QString SignalPlot::signalName(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QString();

    return curv->title().text();
}

void SignalPlot::setSignalName(int n, const QString& newName)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setTitle(newName);
}

qreal SignalPlot::z(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->z();
}

void SignalPlot::setZ(int n, const qreal& newZ)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setZ(newZ);
}

QwtPlotCurve::CurveStyle SignalPlot::curveStyle(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QwtPlotCurve::NoCurve;

    return curv->style();
}

void SignalPlot::setCurveStyle(int n, QwtPlotCurve::CurveStyle newStyle)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setStyle(newStyle);
}

QPen SignalPlot::pen(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QPen();

    return curv->pen();
}

void SignalPlot::setPen(int n, const QPen& newPen)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setPen(newPen);
}

QBrush SignalPlot::brush(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QBrush();

    return curv->brush();
}

void SignalPlot::setBrush(int n, const QBrush& newBrush)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBrush(newBrush);
}

qreal SignalPlot::baseLine(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->baseline();
}

void SignalPlot::setBaseLine(int n, qreal newBaseLine)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBaseline(newBaseLine);
}

void SignalPlot::setBaseLine(qreal newBaseLine)
{
    for(int i = 0; i < signalsCount(); i ++){
        setBaseLine(i, newBaseLine);
    }
}

QRectF SignalPlot::boundingRect(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QRectF(0, 0, -1, -1);

    return curv->dataRect();
}

QRectF SignalPlot::boundingRect() const
{
    QRectF resRect;

    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto trendData = static_cast<SignalSeriesData*>(curv->data());

        QRectF rect = trendData->boundingRect();

        resRect = resRect.united(rect);
    }

    return resRect;
}

void SignalPlot::clear()
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto trendData = static_cast<SignalSeriesData*>(curv->data());

        trendData->clear();
    }

    replot();
}

qreal SignalPlot::period() const
{
    return m_period;
}

void SignalPlot::setPeriod(qreal newPeriod)
{
    m_period = newPeriod;
}

bool SignalPlot::legendItemEnabled() const
{
    return m_legendItem != nullptr;
}

void SignalPlot::setLegendItemEnabled(bool newEnabled)
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

void SignalPlot::putSample(int n, const qreal& newY, const qreal& newDx)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    auto trendData = static_cast<SignalSeriesData*>(curv->data());

    //qDebug() << newY << newDx;

    trendData->putSample(newY, newDx);
}

QList<Qt::GlobalColor> SignalPlot::getDefaultColors()
{
    QList<Qt::GlobalColor> colors;

    for(int i = 0; i < ColorsCount; i ++){
        colors.append(m_colors[i]);
    }

    return colors;
}

int SignalPlot::findCurve(const QwtPlotCurve* findCurv) const
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

QwtPlotCurve* SignalPlot::getCurve(int n)
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

const QwtPlotCurve* SignalPlot::getCurve(int n) const
{
    auto items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

void SignalPlot::updateLegendItem()
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
