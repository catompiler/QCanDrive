#ifndef SIGNALPLOT_H
#define SIGNALPLOT_H

#include <QwtPlot>
#include <QVector>
#include <QwtPlotCurve>
#include <QPen>
#include <QBrush>
#include <QRectF>


class QwtPlotCurve;
class SequentialBuffer;


class SignalPlot : public QwtPlot
{
    Q_OBJECT
public:

    SignalPlot(QWidget* parent = nullptr);
    ~SignalPlot();

    size_t bufferSize() const;
    void setBufferSize(size_t newSize);

    qreal defaultAlpha() const;
    void setDefaultAlpha(qreal newDefaultAlpha);

    QBrush background() const;
    void setBackground(const QBrush& newBrush);

    // takes ownership of the newBuffer
    int addSignal(const QColor& newColor = QColor(), const qreal& z = -1, SequentialBuffer* newBuffer = nullptr);
    void removeSignal(int n);
    int signalsCount() const;

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

    void putSample(int n, const qreal& newY, const qreal& newDx = -1);

protected:
    size_t m_size;
    qreal m_defaultAlpha;

    QwtPlotCurve* getCurve(int n);
    const QwtPlotCurve* getCurve(int n) const;
};

#endif // SIGNALPLOT_H
