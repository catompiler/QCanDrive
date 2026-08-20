#include "oscopeplot.h"
#include "oscopehorizontal.h"
#include "oscopedata.h"
#include "oscopeplotseriesdata.h"
#include "oscopechszerosscaledraw.h"
#include "oscopetimezeroscaledraw.h"
#include "trianglemarkersymbol.h"
#include <QwtPlotCanvas>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtPlotItem>
#include <QwtPlotLayout>
#include <QwtLinearScaleEngine>
#include <QwtSeriesData>
#include <QwtScaleEngine>
#include <QwtScaleWidget>
#include <QwtScaleDiv>
#include <QwtPlotLegendItem>
#include <QFontMetrics>
#include <QwtPlotMarker>
#include <QwtSymbol>
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

OScopePlot::OScopePlot(QWidget* parent, const QString& newName)
    :QwtPlot(parent)
{
    setTitle(newName);

    m_hori = new OScopeHorizontal();

    m_oscData = nullptr;

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
    //pltLay->setCanvasMargin(0);
    //pltLay->setSpacing(0);
    pltLay->setAlignCanvasToScales(true);

    // Легенда.
    m_legendItem = new QwtPlotLegendItem();
    m_legendItem->attach(this);
    // Видимость по-умолчанию.
    m_legendItem->setVisible(false);
    // Обновить свойства.
    updateLegendItem();

    // Пример https://forum.qt.io/topic/54848/qwtplot-zooming-logarithmic-scale
    // Оси.
    // Движок масштабирования.
    QwtLinearScaleEngine* scaleEng_xb = new QwtLinearScaleEngine();
    QwtLinearScaleEngine* scaleEng_yl = new QwtLinearScaleEngine();
    scaleEng_xb->setAttribute(QwtScaleEngine::Floating);
    scaleEng_yl->setAttribute(QwtScaleEngine::Floating);
    scaleEng_xb->setMargins(0.0, 0.0);
    scaleEng_yl->setMargins(0.0, 0.0);
    setAxisScaleEngine(QwtPlot::xBottom, scaleEng_xb);
    setAxisScaleEngine(QwtPlot::yLeft, scaleEng_yl);
    // Названия осей.
    // setAxisTitle(QwtPlot::xBottom, tr("Время"));
    // setAxisTitle(QwtPlot::yLeft, tr("Значение"));
    // Масштаб и деления.
    setupAxisTicks(QwtAxis::XBottom, -HGRID, HGRID);
    setupAxisTicks(QwtAxis::YLeft, -VGRID, VGRID);

    // Debug {
    QwtLinearScaleEngine* scaleEng_xt = new QwtLinearScaleEngine();
    QwtLinearScaleEngine* scaleEng_yr = new QwtLinearScaleEngine();
    scaleEng_xt->setAttribute(QwtScaleEngine::Floating);
    scaleEng_yr->setAttribute(QwtScaleEngine::Floating);
    scaleEng_xt->setMargins(0.0, 0.0);
    scaleEng_yr->setMargins(0.0, 0.0);
    setAxisScaleEngine(QwtPlot::xTop, scaleEng_xt);
    setAxisScaleEngine(QwtPlot::yRight, scaleEng_yr);
    // Названия осей.
    // setAxisTitle(QwtPlot::xBottom, tr("Время"));
    // setAxisTitle(QwtPlot::yLeft, tr("Значение"));
    // Масштаб и деления.
    setupAxisTicks(QwtAxis::XTop, -HGRID, HGRID);
    setupAxisTicks(QwtAxis::YRight, -VGRID, VGRID);
    // } // Debug

    // Сетка.
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableY(true);
    grid->enableYMin(true);
    grid->enableX(true);
    grid->enableXMin(true);
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
    grid->attach(this);

    /*
       In situations, when there is a label at the most right position of the
       scale, additional space is needed to display the overlapping part
       of the label would be taken by reducing the width of scale and canvas.
       To avoid this "jumping canvas" effect, we add a permanent margin.
       We don't need to do the same for the left border, because there
       is enough space for the overlapping label below the left scale.
     */


    QwtScaleWidget* scaleWidget_xb = axisWidget( QwtAxis::XBottom );
    if(scaleWidget_xb){
        int acw = QFontMetrics( scaleWidget_xb->font() ).averageCharWidth();
        scaleWidget_xb->setMinBorderDist( acw * 2, acw * 2 );
    }

    QwtScaleWidget* scaleWidget_yl = axisWidget( QwtAxis::YLeft );
    if(scaleWidget_yl){
        int ach = QFontMetrics( scaleWidget_xb->font() ).height();
        scaleWidget_yl->setMinBorderDist( ach, ach );
    }

    m_chsZerosScaleDraw = new OScopeChsZerosScaleDraw();
    m_chsZerosScaleDraw->setPlot(this);
    setAxisScaleDraw(QwtAxis::YLeft, m_chsZerosScaleDraw);
    setAxisVisible(QwtAxis::YLeft, true);

    // Debug {
    setAxisScaleDraw(QwtAxis::YLeft, new OScopeTimeZeroScaleDraw(this));
    setAxisVisible(QwtAxis::YLeft, true);
    setAxisScaleDraw(QwtAxis::YRight, new OScopeTimeZeroScaleDraw(this));
    setAxisVisible(QwtAxis::YRight, true);
    setAxisScaleDraw(QwtAxis::XTop, new OScopeTimeZeroScaleDraw(this));
    setAxisVisible(QwtAxis::XTop, true);
    setAxisScaleDraw(QwtAxis::XBottom, new OScopeTimeZeroScaleDraw(this));
    setAxisVisible(QwtAxis::XBottom, true);
    // } // Debug

    m_zeroTimeSymbol = new TriangleMarkerSymbol();
    m_zeroTimeSymbol->setDir(TriangleMarkerSymbol::Down);
    m_zeroTimeSymbol->setSize(ZERO_TIME_MARK_WIDTH, ZERO_TIME_MARK_HEIGHT);
    m_zeroTimeSymbol->setPinPoint(QPointF(0.0, ZERO_TIME_MARK_HEIGHT + ZERO_TIME_MARK_MARGIN));
    m_zeroTimeSymbol->setColor(Qt::white);

    m_zeroTimeMarker = new QwtPlotMarker();
    m_zeroTimeMarker->setSymbol(m_zeroTimeSymbol);
    m_zeroTimeMarker->setLabelAlignment(Qt::AlignTop);
    m_zeroTimeMarker->setLabelOrientation(Qt::Horizontal);
    m_zeroTimeMarker->setTitle(tr("T0"));
    m_zeroTimeMarker->setLineStyle(QwtPlotMarker::NoLine); //NoLine
    m_zeroTimeMarker->setLinePen(Qt::white, 1.0, Qt::SolidLine);
    m_zeroTimeMarker->setValue(0.0, VGRID);
    m_zeroTimeMarker->attach(this);

    setAutoReplot(false);
}

OScopePlot::~OScopePlot()
{
    delete m_hori;
}

QString OScopePlot::name() const
{
    return title().text();
}

void OScopePlot::setName(const QString& newName)
{
    setTitle(newName);
}

qreal OScopePlot::defaultAlpha() const
{
    return m_defaultAlpha;
}

void OScopePlot::setDefaultAlpha(qreal newDefaultAlpha)
{
    m_defaultAlpha = newDefaultAlpha;
}

QBrush OScopePlot::background() const
{
    return canvasBackground();
}

void OScopePlot::setBackground(const QBrush& newBrush)
{
    setCanvasBackground(newBrush);

    updateLegendItem();
}

QColor OScopePlot::textColor() const
{
    return palette().color(QPalette::Text);
}

void OScopePlot::setTextColor(const QColor& newColor)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Text, newColor);
    setPalette(pal);

    updateLegendItem();
}

qreal OScopePlot::hDiv() const
{
    return m_hori->hDiv();
}

qreal OScopePlot::invHDiv() const
{
    return m_hori->invHDiv();
}

void OScopePlot::setHDiv(qreal newHDiv)
{
    m_hori->setHDiv(newHDiv);
    invalidateAllBounds();
    updateZeroTimeMark();
    updateTimeScale();
}

qreal OScopePlot::hOffset() const
{
    return m_hori->hOffset();
}

void OScopePlot::setHOffset(qreal newHOffset)
{
    m_hori->setHOffset(newHOffset);
    invalidateAllBounds();
    updateZeroTimeMark();
    updateTimeScale();
}

qreal OScopePlot::vDiv(int n) const
{
    const OScopePlotSeriesData* ser_data = plotData(n);
    if(!ser_data) return qreal();

    return ser_data->vDiv();
}

qreal OScopePlot::invVDiv(int n) const
{
    const OScopePlotSeriesData* ser_data = plotData(n);
    if(!ser_data) return qreal();

    return ser_data->invVDiv();
}

void OScopePlot::setVDiv(int n, qreal newVDiv)
{
    OScopePlotSeriesData* ser_data = plotData(n);
    if(!ser_data) return;

    ser_data->setVDiv(newVDiv);
    ser_data->invalidateBounds();
}

qreal OScopePlot::vOffset(int n) const
{
    const OScopePlotSeriesData* ser_data = plotData(n);
    if(!ser_data) return qreal();

    return ser_data->vOffset();
}

void OScopePlot::setVOffset(int n, qreal newVOffset)
{
    QwtPlotCurve* curv = getCurve(n);
    if(!curv) return;

    OScopePlotSeriesData* ser_data = static_cast<OScopePlotSeriesData*>(curv->data());
    if(!ser_data) return;

    curv->setBaseline(newVOffset * ser_data->invVDiv());

    updateZerosScale();

    ser_data->setVOffset(newVOffset);
    ser_data->invalidateBounds();
}

bool OScopePlot::setData(OScopeData* newOscData)
{
    m_oscData = newOscData;

    return updateData();
}

bool OScopePlot::updateData()
{
    removeSignals();

    OScopeData* osc_data = m_oscData;

    if(osc_data == nullptr) return true;

    size_t channels_count = osc_data->channelsCount();
    //size_t samples_count = newOscData->samplesCount();

    for(size_t ch_i = 0; ch_i < channels_count; ch_i ++){
        // Данные канала.
        OScopePlotSeriesData* series_data = new OScopePlotSeriesData(m_hori, osc_data, ch_i);
        // Добавить сигнал.
        int added_n = addSignal(series_data);
        // Если не удалось.
        if(added_n < 0){
            // Удалим данные.
            delete series_data;
            return false;
        }
    }

    return true;
}

int OScopePlot::addSignal(OScopePlotSeriesData* pltData, const QString& newName, const QColor& newColor, const qreal& z)
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
    curveBrush.setStyle(Qt::NoBrush);
    curveBrush.setColor(curveColor);

    newCurve->setPen(curvePen);
    newCurve->setBrush(curveBrush);
    newCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    newCurve->setSamples(pltData);
    newCurve->attach(this);

    return curvesCount;
}

void OScopePlot::removeSignal(int n)
{
    if(n == -1){
        detachItems(QwtPlotItem::Rtti_PlotCurve, true);
        return;
    }

    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->detach();
}

void OScopePlot::removeSignals()
{
    //while(signalsCount()) removeSignal(0);
    removeSignal(-1);
}

int OScopePlot::signalsCount() const
{
    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    return items.count();
}

OScopePlotSeriesData* OScopePlot::plotData(int n)
{
    QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return nullptr;

    OScopePlotSeriesData* pltData = static_cast<OScopePlotSeriesData*>(curv->data());

    return pltData;
}

const OScopePlotSeriesData* OScopePlot::plotData(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return nullptr;

    const OScopePlotSeriesData* pltData = static_cast<const OScopePlotSeriesData*>(curv->data());

    return pltData;
}

bool OScopePlot::signalVisible(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return false;

    return curv->isVisible();
}

void OScopePlot::setSignalVisible(int n, bool newVisible)
{
    QwtPlotCurve* curv = getCurve(n);
    if(curv == nullptr) return;

    curv->setVisible(newVisible);
}

QString OScopePlot::signalName(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QString();

    return curv->title().text();
}

void OScopePlot::setSignalName(int n, const QString& newName)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setTitle(newName);
}

qreal OScopePlot::z(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->z();
}

void OScopePlot::setZ(int n, const qreal& newZ)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setZ(newZ);
}

QwtPlotCurve::CurveStyle OScopePlot::curveStyle(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QwtPlotCurve::NoCurve;

    return curv->style();
}

void OScopePlot::setCurveStyle(int n, QwtPlotCurve::CurveStyle newStyle)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setStyle(newStyle);
}

QPen OScopePlot::pen(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QPen();

    return curv->pen();
}

void OScopePlot::setPen(int n, const QPen& newPen)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setPen(newPen);
}

QBrush OScopePlot::brush(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QBrush();

    return curv->brush();
}

void OScopePlot::setBrush(int n, const QBrush& newBrush)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBrush(newBrush);
}

qreal OScopePlot::baseLine(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return 0.0;

    return curv->baseline();
}

void OScopePlot::setBaseLine(int n, qreal newBaseLine)
{
    QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return;

    curv->setBaseline(newBaseLine);
}

void OScopePlot::setBaseLine(qreal newBaseLine)
{
    for(int i = 0; i < signalsCount(); i ++){
        setBaseLine(i, newBaseLine);
    }
}

QRectF OScopePlot::boundingRect(int n) const
{
    const QwtPlotCurve* curv = getCurve(n);

    if(curv == nullptr) return QRectF(0, 0, -1, -1);

    return curv->dataRect();
}

QRectF OScopePlot::boundingRect() const
{
    QRectF resRect;

    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(const auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);

        if(!curv->isVisible()) continue;

        auto pltData = static_cast<OScopePlotSeriesData*>(curv->data());

        QRectF rect = pltData->boundingRect();

        resRect = resRect.united(rect);
    }

    return resRect;
}

void OScopePlot::clear()
{
    removeSignals();

    //replot();
}

void OScopePlot::invalidateAllBounds()
{
    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    for(const auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        auto pltData = static_cast<OScopePlotSeriesData*>(curv->data());

        pltData->invalidateBounds();
    }
}

bool OScopePlot::legendItemEnabled() const
{
    return m_legendItem != nullptr;
}

void OScopePlot::setLegendItemEnabled(bool newEnabled)
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

QList<Qt::GlobalColor> OScopePlot::getDefaultColors()
{
    QList<Qt::GlobalColor> colors;

    for(int i = 0; i < ColorsCount; i ++){
        colors.append(m_colors[i]);
    }

    return colors;
}

int OScopePlot::findCurve(const QwtPlotCurve* findCurv) const
{
    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    int n = 0;
    for(const auto& item: items){
        const auto& curv = static_cast<const QwtPlotCurve*>(item);
        if(curv == findCurv) return n;
        n ++;
    }

    return -1;
}

QwtPlotCurve* OScopePlot::getCurve(int n)
{
    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

const QwtPlotCurve* OScopePlot::getCurve(int n) const
{
    const auto& items = itemList(QwtPlotItem::Rtti_PlotCurve);

    if(n >= items.count()) return nullptr;

    return static_cast<QwtPlotCurve*>(items.at(n));
}

void OScopePlot::updateLegendItem()
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

void OScopePlot::setupAxisTicks(QwtAxisId axis_id, int min_tick, int max_tick)
{
    constexpr int step = 1;

    QwtScaleDiv scale_div(min_tick, max_tick);

    QList<double> minor_ticks, major_ticks {static_cast<double>(min_tick), 0.0, static_cast<double>(max_tick)};

    for(int i = min_tick + step; i < max_tick; i += step){
        if(i == 0) continue;
        minor_ticks.append(static_cast<double>(i));
    }

    scale_div.setTicks(QwtScaleDiv::MinorTick, minor_ticks);
    scale_div.setTicks(QwtScaleDiv::MajorTick, major_ticks);

    setAxisScaleDiv(axis_id, scale_div);

    QwtScaleDraw* scale_draw = axisScaleDraw(axis_id);
    if(scale_draw){
        scale_draw->enableComponent(QwtScaleDraw::Labels, false);
        scale_draw->enableComponent(QwtScaleDraw::Ticks, false);
        scale_draw->enableComponent(QwtScaleDraw::Backbone, false);
    }
}

void OScopePlot::updateZerosScale()
{
    // Debug {
    axisWidget(QwtAxis::XTop)->update();
    axisWidget(QwtAxis::XBottom)->update();
    axisWidget(QwtAxis::YLeft)->update();
    axisWidget(QwtAxis::YRight)->update();
    // } // Debug

    QwtScaleWidget* zeroScaleWgt = axisWidget(QwtAxis::YLeft);
    if(zeroScaleWgt) zeroScaleWgt->update();
}

void OScopePlot::updateZeroTimeMark()
{
    int canvas_width;

    if(QwtPlotLayout* pltLay = plotLayout(); pltLay != nullptr){
        QRectF canvas_rect = pltLay->canvasRect();
        canvas_width = qRound(canvas_rect.width());
    }else{
        canvas_width = qRound(
            transform(QwtAxis::XTop, HGRID) - transform(QwtAxis::XTop, -HGRID)
            );
    }

    qreal grid_offset = hOffset() * invHDiv();
    int canvas_offset = qRound(transform(QwtAxis::XTop, grid_offset));

    if(canvas_offset <= (ZERO_TIME_MARK_WIDTH/2)){
        grid_offset = -HGRID;
        m_zeroTimeSymbol->setPinPoint(QPointF(ZERO_TIME_MARK_MARGIN, (ZERO_TIME_MARK_WIDTH/2) + ZERO_TIME_MARK_MARGIN));
        m_zeroTimeSymbol->setDir(TriangleMarkerSymbol::Left);
    }
    else if(canvas_offset >= (canvas_width - ZERO_TIME_MARK_WIDTH/2)){
        grid_offset = HGRID;
        m_zeroTimeSymbol->setPinPoint(QPointF(-ZERO_TIME_MARK_MARGIN, (ZERO_TIME_MARK_WIDTH/2) + ZERO_TIME_MARK_MARGIN));
        m_zeroTimeSymbol->setDir(TriangleMarkerSymbol::Right);
    }
    else{
        m_zeroTimeSymbol->setPinPoint(QPointF(0.0, ZERO_TIME_MARK_HEIGHT + ZERO_TIME_MARK_MARGIN));
        m_zeroTimeSymbol->setDir(TriangleMarkerSymbol::Down);
    }

    m_zeroTimeMarker->setXValue(grid_offset);
}

void OScopePlot::updateTimeScale()
{
    // Debug {
    axisWidget(QwtAxis::XTop)->update();
    axisWidget(QwtAxis::XBottom)->update();
    axisWidget(QwtAxis::YLeft)->update();
    axisWidget(QwtAxis::YRight)->update();
    // } // Debug

    QwtScaleWidget* timeScaleWgt = axisWidget(QwtAxis::XTop);
    if(!timeScaleWgt) timeScaleWgt->update();
}
