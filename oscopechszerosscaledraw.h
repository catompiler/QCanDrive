#ifndef OSCOPECHSZEROSSCALEDRAW_H
#define OSCOPECHSZEROSSCALEDRAW_H

#include <QwtScaleDraw>


class QwtPlot;
class QColor;


class OScopeChsZerosScaleDraw : public QwtScaleDraw
{
public:

    static constexpr int MARKER_WIDTH = 5;
    static constexpr int MARKER_HALF_HEIGHT = 5;
    static constexpr int LIMIT_MARGIN = MARKER_HALF_HEIGHT;

    OScopeChsZerosScaleDraw(QwtPlot* newPlot = nullptr);
    ~OScopeChsZerosScaleDraw();

    /*
     * График, для кривых которого рисовать метки.
     */
    QwtPlot* plot();
    const QwtPlot* plot() const;
    void setPlot(QwtPlot* newPlot);

    // QwtAbstractScaleDraw interface
    virtual void draw(QPainter* painter, const QPalette& palette) const override;
    virtual double extent(const QFont& font) const override;

protected:
    double extraExtent(const QFont& font) const;
    void drawZeros(QPainter* painter, const QPalette& palette) const;
    void drawZero(QPainter* painter, const QColor& markerCol, const QColor& labelCol, const QString& labelText, int zeroPos, int lowerBound, int upperBound) const;
    void drawMarker(QPainter* painter, const QColor& markerCol, const QPoint& zeroPoint, Alignment align) const;
    void drawLabelText(QPainter* painter, const QColor& labelCol, const QPoint& zeroPoint, const QString& labelText, Alignment align) const;

    QwtPlot* m_plot;
};

#endif // OSCOPECHSZEROSSCALEDRAW_H
