#ifndef TRIANGLEMARKERSYMBOL_H
#define TRIANGLEMARKERSYMBOL_H

#include <QwtSymbol>


class TriangleMarkerSymbol : public QwtSymbol
{
public:

    enum Style {
        TriangleMarker = QwtSymbol::UserStyle
    };

    enum Direction {
        Right = 0,
        Down = 1,
        Left = 2,
        Up = 3
    };

    static constexpr qreal DEFAULT_BASE = 10.0;
    static constexpr qreal DEFAULT_HEIGHT = 5.0;

    TriangleMarkerSymbol();
    ~TriangleMarkerSymbol();

    Direction dir() const;
    void setDir(Direction newDir);

    // Основание треугольника: size().width().
    // Высота треугольника: size().height().
    // Смещение треугольника: pinPoint() (включается через setPinPointEnabled(bool on)).

    // Direction::Right
    //
    // (-h, -b/2)
    // |>
    // |  |> (0, 0)
    // |>
    // (-h, +b/2)
    static void drawTriangleMarker(QPainter* painter, const QPointF& point, qreal triangleBase, qreal triangleHeight, Direction dir);

    // QwtSymbol interface
    virtual void setColor(const QColor&col) override;
    virtual QRect boundingRect() const override;
protected:
    virtual void renderSymbols(QPainter* painter, const QPointF* points, int numPoints) const override;

    void renderSymbol(QPainter* painter, const QPointF& point, qreal triangleBase, qreal triangleHeight) const;

private:
    Direction m_dir;
};

#endif // TRIANGLEMARKERSYMBOL_H
