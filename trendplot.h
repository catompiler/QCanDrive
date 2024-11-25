#ifndef TRENDPLOT_H
#define TRENDPLOT_H

#include <QwtPlot>
#include <QVector>


class QwtPlotCurve;
class SequentialBuffer;


class TrendPlot : public QwtPlot
{
    Q_OBJECT
public:
    TrendPlot(QWidget* parent = nullptr);
    ~TrendPlot();

    int addTrend(const SequentialBuffer* newBuffer);
    void removeTrend(int n);

protected:
    QwtPlotCurve* getCurve(int n);
};

#endif // TRENDPLOT_H
