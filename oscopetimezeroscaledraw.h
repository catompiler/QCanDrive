#ifndef OSCOPETIMEZEROSCALEDRAW_H
#define OSCOPETIMEZEROSCALEDRAW_H

#include <QwtScaleDraw>
#include <QString>
#include <QMap>
#include <QPair>


class OScopePlot;
class QColor;


class OScopeTimeZeroScaleDraw : public QwtScaleDraw
{
public:

    static constexpr int MARKER_WIDTH = 5;
    static constexpr int MARKER_HALF_HEIGHT = 5;
    static constexpr int MARKER_HEIGHT = MARKER_HALF_HEIGHT * 2;
    static constexpr int LIMIT_MARGIN = MARKER_HALF_HEIGHT;

    OScopeTimeZeroScaleDraw(OScopePlot* newPlot = nullptr);
    ~OScopeTimeZeroScaleDraw();

    /*
     * График, для кривых которого рисовать метки.
     */
    OScopePlot* plot();
    const OScopePlot* plot() const;
    void setPlot(OScopePlot* newPlot);

    // QwtAbstractScaleDraw interface
    virtual void draw(QPainter* painter, const QPalette& palette) const override;
    virtual double extent(const QFont& font) const override;

protected:
    double extraExtent(const QFont& font) const;
    void drawZeroTime(QPainter* painter, const QPalette& palette) const;
    void drawZeroLabel(QPainter* painter, const QColor& markerCol, const QColor& labelCol, const QString& labelText, int zeroPos) const;
    void drawMarker(QPainter* painter, const QColor& markerCol, const QPoint& zeroPoint, Alignment align) const;
    void drawLabelText(QPainter* painter, const QColor& labelCol, const QPoint& zeroPoint, const QString& labelText, Alignment align) const;

    QString readableTimeValue(qreal value) const;

    OScopePlot* m_plot;

    static const QMap<int, QString> m_unitMap; //!< Приставки к елинице измерения.
};

#endif // OSCOPETIMEZEROSCALEDRAW_H
