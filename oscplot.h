#ifndef OSCPLOT_H
#define OSCPLOT_H

#include <QwtPlot>
#include <QVector>
#include <QwtPlotCurve>
#include <QPen>
#include <QBrush>
#include <QRectF>


class QwtPlotCurve;
class QwtPlotLegendItem;
class OscPlotData;


class OscPlot : public QwtPlot
{
    Q_OBJECT
public:

    OscPlot(QWidget* parent = nullptr, const QString& newName = QString());
    ~OscPlot();

    QString name() const;
    void setName(const QString& newName);

    qreal defaultAlpha() const;
    void setDefaultAlpha(qreal newDefaultAlpha);

    QBrush background() const;
    void setBackground(const QBrush& newBrush);

    QColor textColor() const;
    void setTextColor(const QColor& newColor);

    // takes ownership of the plotData
    int addSignal(OscPlotData* pltData, const QString& newName = QString(), const QColor& newColor = QColor(), const qreal& z = -1);
    void removeSignal(int n);
    void removeSignals();
    int signalsCount() const;

    // Данные канала.
    OscPlotData* plotData(int n);
    const OscPlotData* plotData(int n) const;

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

protected:
    qreal m_defaultAlpha;
    qreal m_period;

    QwtPlotLegendItem* m_legendItem;

    int findCurve(const QwtPlotCurve* findCurv) const;
    QwtPlotCurve* getCurve(int n);
    const QwtPlotCurve* getCurve(int n) const;

    void updateLegendItem();
};

#endif // OSCPLOT_H
