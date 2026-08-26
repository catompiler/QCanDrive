#include "oscopechszerosscaledraw.h"
#include "trianglemarkersymbol.h"
#include <QPainter>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>
#include <QwtScaleMap>
#include <QwtScaleDiv>
#include <QwtPlot>
#include <QwtPlotCurve>
#include <QColor>
#include <QDebug>


OScopeChsZerosScaleDraw::OScopeChsZerosScaleDraw(QwtPlot* newPlot)
    :QwtScaleDraw()
{
    m_plot = newPlot;

    enableComponent(QwtAbstractScaleDraw::Backbone, false);
    enableComponent(QwtAbstractScaleDraw::Labels, false);
    enableComponent(QwtAbstractScaleDraw::Ticks, false);
}

OScopeChsZerosScaleDraw::~OScopeChsZerosScaleDraw()
{
}

QwtPlot* OScopeChsZerosScaleDraw::plot()
{
    return m_plot;
}

const QwtPlot* OScopeChsZerosScaleDraw::plot() const
{
    return m_plot;
}

void OScopeChsZerosScaleDraw::setPlot(QwtPlot* newPlot)
{
    m_plot = newPlot;
}


void OScopeChsZerosScaleDraw::draw(QPainter* painter, const QPalette& palette) const
{
    painter->save();

    double offset = extraExtent(painter->font());

    //qDebug() << alignment() << offset;

    painter->save();
    switch(alignment()){
    default:
        break;
    case QwtScaleDraw::BottomScale:
        painter->translate(0, offset);
        break;
    case QwtScaleDraw::TopScale:
        painter->translate(0, -offset);
        break;
    case QwtScaleDraw::LeftScale:
        painter->translate(-offset, 0);
        break;
    case QwtScaleDraw::RightScale:
        painter->translate(offset, 0);
        break;
    }
    QwtScaleDraw::draw(painter, palette);
    painter->restore();

    drawZeros(painter, palette);

    painter->restore();
}

double OScopeChsZerosScaleDraw::extent(const QFont& font) const
{
    double scaleExtent = QwtScaleDraw::extent(font);

    return scaleExtent + extraExtent(font);
}

double OScopeChsZerosScaleDraw::extraExtent(const QFont& font) const
{
    //qDebug() << "OScopeChsZerosScaleDraw::extraExtent";

    if(!m_plot) return 0.0;

    int markerExtent = 0;
    int spacingExtent = 0;
    int labelExtent = 0;

    if(orientation() == Qt::Vertical){
        int curvesCount = m_plot->itemList(QwtPlotItem::Rtti_PlotCurve).size();
        if(curvesCount == 0) return 0.0;

        QFontMetrics fm(font);

        markerExtent = MARKER_WIDTH;
        spacingExtent = MARKER_WIDTH / 2;

        int charExtent = fm.averageCharWidth();
        labelExtent = 0;

        for(int i = 1; (i <= curvesCount) && (i > 0); i *= 10){
            labelExtent += charExtent;
        }
    }
    // Qt::Horizontal
    else{
        QFontMetrics fm(font);

        markerExtent = MARKER_WIDTH;
        spacingExtent = MARKER_WIDTH / 2;
        labelExtent = fm.height();
    }

    //qDebug() << orientation() << "spacingExtent" << spacingExtent << "labelExtent" << labelExtent;

    return markerExtent + spacingExtent + labelExtent + spacingExtent;
}

void OScopeChsZerosScaleDraw::drawZeros(QPainter* painter, const QPalette& palette) const
{
    if(!m_plot) return;

    const QwtScaleMap& map = scaleMap();
    const QwtScaleDiv& div = scaleDiv();

    int lowerBound = map.transform(div.lowerBound());
    int upperBound = map.transform(div.upperBound());

    if(lowerBound > upperBound){
        qSwap(lowerBound, upperBound);
    }

    const auto& items = m_plot->itemList(QwtPlotItem::Rtti_PlotCurve);

    int n = 1;
    for(const auto& item: items){
        auto curv = static_cast<QwtPlotCurve*>(item);
        if(!curv->isVisible()) continue;

        drawZero(painter, curv->pen().color(), palette.color(QPalette::WindowText), QString::number(n), qRound(map.transform(curv->baseline())), lowerBound, upperBound);

        n ++;
    }
}

void OScopeChsZerosScaleDraw::drawZero(QPainter* painter, const QColor& markerCol, const QColor& labelCol, const QString& labelText, int zeroPos, int lowerBound, int upperBound) const
{
    //qDebug() << "OScopeChsZerosScaleDraw::drawZero" << lowerBound << zeroPos << upperBound << pos();

    //const QwtScaleMap& map = scaleMap();
    //const QwtScaleDiv& div = scaleDiv();

    bool upperLimit = zeroPos >= (upperBound - LIMIT_MARGIN);
    bool lowerLimit = zeroPos <= (lowerBound + LIMIT_MARGIN);

    // clamp.
    zeroPos = upperLimit ? (upperBound) : ( lowerLimit ? (lowerBound) : (zeroPos) );

    QPoint zeroPoint;

    switch(alignment()){
    case QwtScaleDraw::BottomScale:
        zeroPoint.setX(zeroPos);
        zeroPoint.setY(pos().y());
        break;
    case QwtScaleDraw::TopScale:
        zeroPoint.setX(zeroPos);
        zeroPoint.setY(pos().y());
        break;
    case QwtScaleDraw::LeftScale:
        zeroPoint.setX(pos().x());
        zeroPoint.setY(zeroPos);
        break;
    case QwtScaleDraw::RightScale:
        zeroPoint.setX(pos().x());
        zeroPoint.setY(zeroPos);
        break;
    }

    //painter->drawRect(pos().x(), pos().y(), 10, 10);

    if(upperLimit){
        QwtScaleDraw::Alignment lim_align;

        switch(alignment()){
        case QwtScaleDraw::BottomScale:
            lim_align = QwtScaleDraw::LeftScale;
            zeroPoint.setX(zeroPoint.x());
            zeroPoint.setY(zeroPoint.y() + MARKER_HALF_HEIGHT);
            break;
        case QwtScaleDraw::TopScale:
            lim_align = QwtScaleDraw::LeftScale;
            zeroPoint.setX(zeroPoint.x());
            zeroPoint.setY(zeroPoint.y() - MARKER_HALF_HEIGHT);
            break;
        case QwtScaleDraw::LeftScale:
            lim_align = QwtScaleDraw::TopScale;
            zeroPoint.setX(zeroPoint.x() - MARKER_HALF_HEIGHT);
            zeroPoint.setY(zeroPoint.y());
            break;
        case QwtScaleDraw::RightScale:
            lim_align = QwtScaleDraw::TopScale;
            zeroPoint.setX(zeroPoint.x() + MARKER_HALF_HEIGHT);
            zeroPoint.setY(zeroPoint.y());
            break;
        }

        drawMarker(painter, markerCol, zeroPoint, lim_align);
        drawLabelText(painter, labelCol, zeroPoint, labelText, lim_align);
    }else if(lowerLimit){
        QwtScaleDraw::Alignment lim_align;

        switch(alignment()){
        case QwtScaleDraw::BottomScale:
            lim_align = QwtScaleDraw::RightScale;
            zeroPoint.setX(zeroPoint.x());
            zeroPoint.setY(zeroPoint.y() + MARKER_HALF_HEIGHT);
            break;
        case QwtScaleDraw::TopScale:
            lim_align = QwtScaleDraw::RightScale;
            zeroPoint.setX(zeroPoint.x());
            zeroPoint.setY(zeroPoint.y() - MARKER_HALF_HEIGHT);
            break;
        case QwtScaleDraw::LeftScale:
            lim_align = QwtScaleDraw::BottomScale;
            zeroPoint.setX(zeroPoint.x() - MARKER_HALF_HEIGHT);
            zeroPoint.setY(zeroPoint.y());
            break;
        case QwtScaleDraw::RightScale:
            lim_align = QwtScaleDraw::BottomScale;
            zeroPoint.setX(zeroPoint.x() + MARKER_HALF_HEIGHT);
            zeroPoint.setY(zeroPoint.y());
            break;
        }

        drawMarker(painter, markerCol, zeroPoint, lim_align);
        drawLabelText(painter, labelCol, zeroPoint, labelText, lim_align);
    }else{ // noLimit
        drawMarker(painter, markerCol, zeroPoint, alignment());
        drawLabelText(painter, labelCol, zeroPoint, labelText, alignment());
    }
}

void OScopeChsZerosScaleDraw::drawMarker(QPainter* painter, const QColor& markerCol, const QPoint& zeroPoint, QwtScaleDraw::Alignment align) const
{
    TriangleMarkerSymbol::Direction dir;

    switch(align){
    default:
    case QwtScaleDraw::LeftScale:
        dir = TriangleMarkerSymbol::Right;
        break;
    case QwtScaleDraw::BottomScale:
        dir = TriangleMarkerSymbol::Up;
        break;
    case QwtScaleDraw::TopScale:
        dir = TriangleMarkerSymbol::Down;
        break;
    case QwtScaleDraw::RightScale:
        dir = TriangleMarkerSymbol::Left;
        break;
    }

    painter->setBrush(QBrush(markerCol));
    painter->setPen(QPen(markerCol));

    TriangleMarkerSymbol::drawTriangleMarker(painter, zeroPoint, MARKER_HALF_HEIGHT * 2, MARKER_WIDTH, dir);
}

void OScopeChsZerosScaleDraw::drawLabelText(QPainter* painter, const QColor& labelCol, const QPoint& zeroPoint, const QString& labelText, QwtScaleDraw::Alignment align) const
{
    //const QwtScaleMap& map = scaleMap();
    //const QwtScaleDiv& div = scaleDiv();

    QFontMetrics fm = painter->fontMetrics();
    QRect textRect = fm.boundingRect(labelText);

    int markerSpacing = MARKER_WIDTH / 2;

    //qDebug() << textRect.left() << textRect.top() << textRect.right() << textRect.bottom() << fm.height();

    painter->save();

    painter->translate(zeroPoint);

    switch(align){
    default:
    case QwtScaleDraw::LeftScale:
        painter->translate(-(MARKER_WIDTH + markerSpacing) - (textRect.right() + 1), -(textRect.bottom() + 1) + textRect.height() / 2);
        break;
    case QwtScaleDraw::RightScale:
        painter->translate(+(MARKER_WIDTH + markerSpacing) - (textRect.left()),      -(textRect.bottom() + 1) + textRect.height() / 2);
        break;
    case QwtScaleDraw::BottomScale:
        painter->translate(-(textRect.right() + 1) + textRect.width() / 2, +(MARKER_WIDTH + markerSpacing - (textRect.top()) ));
        break;
    case QwtScaleDraw::TopScale:
        painter->translate(-(textRect.right() + 1) + textRect.width() / 2, -(MARKER_WIDTH + markerSpacing) - (textRect.bottom() + 1) );
        break;
    }

    painter->setPen(QPen(labelCol));
    //painter->drawRect(textRect);
    painter->setBrush(QBrush(labelCol));
    painter->drawText(0, 0, labelText);

    painter->restore();
}
