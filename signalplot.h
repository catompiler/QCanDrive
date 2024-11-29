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

    SignalPlot(const QString& newName = QString(), QWidget* parent = nullptr);
    ~SignalPlot();

    size_t bufferSize() const;
    void setBufferSize(size_t newSize);

    qreal defaultAlpha() const;
    void setDefaultAlpha(qreal newDefaultAlpha);

    QBrush background() const;
    void setBackground(const QBrush& newBrush);

    QString name() const;
    void setName(const QString& newName);

    // takes ownership of the newBuffer
    int addSignal(const QString& newName = QString(), const QColor& newColor = QColor(), const qreal& z = -1, SequentialBuffer* newBuffer = nullptr);
    void removeSignal(int n);
    int signalsCount() const;

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

    void putSample(int n, const qreal& newY, const qreal& newDx = -1);

    static QList<Qt::GlobalColor> getDefaultColors();

    qreal period() const;
    void setPeriod(qreal newPeriod);

public slots:
    void clear();

protected:
    size_t m_size;
    qreal m_defaultAlpha;
    qreal m_period;

    QwtPlotCurve* getCurve(int n);
    const QwtPlotCurve* getCurve(int n) const;
};

#endif // SIGNALPLOT_H
