#ifndef TRENDPLOT_H
#define TRENDPLOT_H

#include <QwtPlot>
#include <QVector>
#include <QwtPlotCurve>
#include <QPen>
#include <QBrush>
#include <QRectF>


class QwtPlotCurve;
class SequentialBuffer;


class TrendPlot : public QwtPlot
{
    Q_OBJECT
public:
    TrendPlot(QWidget* parent = nullptr);
    ~TrendPlot();

    int addTrend(const SequentialBuffer* newBuffer, qreal z = -1);
    void removeTrend(int n);
    int trendsCount() const;

    QwtPlotCurve::CurveStyle style(int n) const;
    void setStyle(int n, QwtPlotCurve::CurveStyle newStyle);

    QPen pen(int n) const;
    void setPen(int n, const QPen& newPen);

    QBrush brush(int n) const;
    void setBrush(int n, const QBrush& newBrush);

    qreal baseLine(int n) const;
    void setBaseLine(int n, qreal newBaseLine);

    QRectF boundingRect(int n) const;

protected:
    QwtPlotCurve* getCurve(int n);
    const QwtPlotCurve* getCurve(int n) const;
};

#endif // TRENDPLOT_H
