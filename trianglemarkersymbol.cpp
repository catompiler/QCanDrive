#include "trianglemarkersymbol.h"
#include <QPoint>
#include <QPointF>
#include <QPainter>
#include <QRect>
#include <QMargins>
#include <QPen>
#include <QBrush>
#include <QDebug>



TriangleMarkerSymbol::TriangleMarkerSymbol()
    :QwtSymbol(static_cast<QwtSymbol::Style>(TriangleMarker))
{
    m_dir = static_cast<Direction>(0);

    setSize(DEFAULT_BASE, DEFAULT_HEIGHT);
    setPen(QPen(Qt::SolidLine));
    setBrush(QBrush(Qt::SolidPattern));
}

TriangleMarkerSymbol::~TriangleMarkerSymbol()
{
}

TriangleMarkerSymbol::Direction TriangleMarkerSymbol::dir() const
{
    return m_dir;
}

void TriangleMarkerSymbol::setDir(Direction newDir)
{
    m_dir = newDir;
    invalidateCache();
}

void TriangleMarkerSymbol::drawTriangleMarker(QPainter* painter, const QPointF& point, qreal triangleBase, qreal triangleHeight, Direction dir)
{
    qreal b2 = triangleBase / 2;
    qreal h = triangleHeight;

    painter->save();

    painter->translate(point);

    switch(dir){
    case TriangleMarkerSymbol::Right:{
        const QPointF triangle[] = {
            {-h, -b2},
            { 0,  0 },
            {-h, +b2}
        };
        const int triangle_points_count = sizeof(triangle) / sizeof(triangle[0]);
        painter->drawPolygon(triangle, triangle_points_count);
    }break;

    case TriangleMarkerSymbol::Down:{
        const QPointF triangle[] = {
            {-b2, -h},
            { 0,   0},
            {+b2, -h}
        };
        const int triangle_points_count = sizeof(triangle) / sizeof(triangle[0]);
        painter->drawPolygon(triangle, triangle_points_count);
    }break;

    case TriangleMarkerSymbol::Left:{
        const QPointF triangle[] = {
            {+h, -b2},
            { 0,  0 },
            {+h, +b2}
        };
        const int triangle_points_count = sizeof(triangle) / sizeof(triangle[0]);
        painter->drawPolygon(triangle, triangle_points_count);
    }break;

    case TriangleMarkerSymbol::Up:{
        const QPointF triangle[] = {
            {-b2, +h},
            { 0,   0},
            {+b2, +h}
        };
        const int triangle_points_count = sizeof(triangle) / sizeof(triangle[0]);
        painter->drawPolygon(triangle, triangle_points_count);
    }break;
    }

    painter->restore();
}

void TriangleMarkerSymbol::setColor(const QColor& col)
{
    QBrush b = brush();
    b.setColor(col);
    setBrush(b);

    QPen p = pen();
    p.setColor(col);
    setPen(p);
}

QRect TriangleMarkerSymbol::boundingRect() const
{
    QRect res_rect;

    QSize sz = size();

    int b = sz.width();
    int b2 = qRound(static_cast<qreal>(b) / 2);
    int h = sz.height();

    switch(m_dir){
    case TriangleMarkerSymbol::Right:
        res_rect.setRect(-h, -b2, h, b);
        break;
    case TriangleMarkerSymbol::Down:
        res_rect.setRect(-b2, -h, b, h);
        break;
    case TriangleMarkerSymbol::Left:
        res_rect.setRect(0, -b2, h, b);
        break;
    case TriangleMarkerSymbol::Up:
        res_rect.setRect(-b2, 0, b, h);
        break;
    }

    res_rect += QMargins(1, 1, 1, 1);

    if(isPinPointEnabled()) res_rect.moveTopLeft(res_rect.topLeft() + pinPoint().toPoint());

    return res_rect;
}

void TriangleMarkerSymbol::renderSymbols(QPainter* painter, const QPointF* points, int numPoints) const
{
    if(style() != static_cast<QwtSymbol::Style>(TriangleMarker)){
        QwtSymbol::renderSymbols(painter, points, numPoints);
        return;
    }

    QSize sz = size();
    qreal triangleBase = sz.width();
    qreal triangleHeight = sz.height();

    painter->save();

    painter->setPen(pen());
    painter->setBrush(brush());

    for(int i = 0; i < numPoints; i ++){
        renderSymbol(painter, points[i], triangleBase, triangleHeight);
    }

    painter->restore();
}

void TriangleMarkerSymbol::renderSymbol(QPainter* painter, const QPointF& point, qreal triangleBase, qreal triangleHeight) const
{
    //qDebug() << "TriangleMarkerSymbol::renderSymbol" << point << pinPoint() << size() << triangleBase << triangleHeight;

    QPointF posPoint = point;

    if(isPinPointEnabled()){
        posPoint += pinPoint();
    }

    /*painter->drawEllipse(point, 100.0, 100.0);*/

    drawTriangleMarker(painter, posPoint, triangleBase, triangleHeight, m_dir);
}
