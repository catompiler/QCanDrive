#ifndef OSCOPEPLOT_H
#define OSCOPEPLOT_H

#include <QwtPlot>
#include <QVector>
#include <QwtPlotCurve>
#include <QPen>
#include <QBrush>
#include <QRectF>


class QwtPlotCurve;
class QwtPlotLegendItem;
class QwtPlotZoomer;
class OScopeHorizontal;
class OScopeData;
class OScopePlotSeriesData;


class OScopePlot : public QwtPlot
{
    Q_OBJECT
public:

    static constexpr size_t HGRID = 4;
    static constexpr size_t VGRID = 4;

    OScopePlot(QWidget* parent = nullptr, const QString& newName = QString());
    ~OScopePlot();

    QString name() const;
    void setName(const QString& newName);

    qreal defaultAlpha() const;
    void setDefaultAlpha(qreal newDefaultAlpha);

    QBrush background() const;
    void setBackground(const QBrush& newBrush);

    QColor textColor() const;
    void setTextColor(const QColor& newColor);

    qreal hDiv() const;
    void setHDiv(qreal newHDiv);

    qreal hOffset() const;
    void setHOffset(qreal newHOffset);

    qreal vDiv(int n) const;
    void setVDiv(int n, qreal newVDiv);

    qreal vOffset(int n) const;
    void setVOffset(int n, qreal newVOffset);

    //! Очищает график и добавляет сигналы каналов осциллографа.
    bool setData(OScopeData* newOscData);
    //! Обновляет каналы из ранее установленных данных каналов осциллографа.
    bool updateData();

    // takes ownership of the plotData
    int addSignal(OScopePlotSeriesData* pltData, const QString& newName = QString(), const QColor& newColor = QColor(), const qreal& z = -1);
    void removeSignal(int n);
    void removeSignals();
    int signalsCount() const;

    // Данные канала.
    OScopePlotSeriesData* plotData(int n);
    const OScopePlotSeriesData* plotData(int n) const;

    bool signalVisible(int n) const;
    void setSignalVisible(int n, bool newVisible);

    QString signalName(int n) const;
    void setSignalName(int n, const QString& newName);

    qreal z(int n) const;
    void setZ(int n, const qreal& newZ);

    QwtPlotCurve::CurveStyle curveStyle(int n) const;
    void setCurveStyle(int n, QwtPlotCurve::CurveStyle newStyle);

    QPen pen(int n) const;
    void setPen(int n, const QPen& newPen);

    QBrush brush(int n) const;
    void setBrush(int n, const QBrush& newBrush);

    qreal baseLine(int n) const;
    void setBaseLine(int n, qreal newBaseLine);
    void setBaseLine(qreal newBaseLine);

    QRectF boundingRect(int n) const;
    QRectF boundingRect() const;

    static QList<Qt::GlobalColor> getDefaultColors();

    bool legendItemEnabled() const;
    void setLegendItemEnabled(bool newEnabled);

public slots:
    void clear(); // Вызывает removeSignals();
    void invalidateAllBounds(); // Помечает предрассчитанные границы графиков как невалидные.

protected:
    OScopeHorizontal* m_hori;

    qreal m_defaultAlpha;
    qreal m_period;

    OScopeData* m_oscData;

    QwtPlotLegendItem* m_legendItem;

    int findCurve(const QwtPlotCurve* findCurv) const;
    QwtPlotCurve* getCurve(int n);
    const QwtPlotCurve* getCurve(int n) const;

    void updateLegendItem();
    void setupAxisTicks(QwtAxisId axis_id, int min_tick, int max_tick);
};

#endif // OSCOPEPLOT_H
