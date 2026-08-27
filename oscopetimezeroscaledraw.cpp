#include "oscopetimezeroscaledraw.h"
#include "trianglemarkersymbol.h"
#include "timeformatter.h"
#include <QPainter>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>
#include <QwtScaleMap>
#include <QwtScaleDiv>
#include <oscopeplot.h>
#include <QwtPlotCurve>
#include <QColor>
#include <math.h>
#include <QDebug>



OScopeTimeZeroScaleDraw::OScopeTimeZeroScaleDraw(OScopePlot* newPlot)
    :QwtScaleDraw()
{
    m_plot = newPlot;

    enableComponent(QwtAbstractScaleDraw::Backbone, false);
    enableComponent(QwtAbstractScaleDraw::Labels, false);
    enableComponent(QwtAbstractScaleDraw::Ticks, false);
}

OScopeTimeZeroScaleDraw::~OScopeTimeZeroScaleDraw()
{
}

OScopePlot* OScopeTimeZeroScaleDraw::plot()
{
    return m_plot;
}

const OScopePlot* OScopeTimeZeroScaleDraw::plot() const
{
    return m_plot;
}

void OScopeTimeZeroScaleDraw::setPlot(OScopePlot* newPlot)
{
    m_plot = newPlot;
}


void OScopeTimeZeroScaleDraw::draw(QPainter* painter, const QPalette& palette) const
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

    drawZeroTime(painter, palette);

    painter->restore();
}

double OScopeTimeZeroScaleDraw::extent(const QFont& font) const
{
    double scaleExtent = QwtScaleDraw::extent(font);

    return scaleExtent + extraExtent(font);
}

double OScopeTimeZeroScaleDraw::extraExtent(const QFont& font) const
{
    //qDebug() << "OScopeTimeZeroScaleDraw::extraExtent";

    if(!m_plot) return 0.0;

    int markerExtent = 0;
    int spacingExtent = 0;
    int labelExtent = 0;

    if(orientation() == Qt::Vertical){
        qreal T0 = m_plot->hOffset();

        QFontMetrics fm(font);

        markerExtent = 0;
        spacingExtent = MARKER_WIDTH;

        QString labelText = readableTimeValue(-T0);
        labelExtent = fm.horizontalAdvance(labelText);
    }
    // Qt::Horizontal
    else{
        QFontMetrics fm(font);

        markerExtent = 0;
        spacingExtent = MARKER_WIDTH;
        labelExtent = fm.height();
    }

    //qDebug() << orientation() << "spacingExtent" << spacingExtent << "labelExtent" << labelExtent;

    return markerExtent + labelExtent + spacingExtent;
}

void OScopeTimeZeroScaleDraw::drawZeroTime(QPainter* painter, const QPalette& palette) const
{
    if(!m_plot) return;

    const QwtScaleMap& map = scaleMap();

    qreal T0 = m_plot->hOffset();
    qreal T0_scale = 0.0; //T0 * m_plot->invHDiv();

    drawZeroLabel(painter, palette.color(QPalette::WindowText), palette.color(QPalette::WindowText), readableTimeValue(-T0), qRound(map.transform(T0_scale)));
}

void OScopeTimeZeroScaleDraw::drawZeroLabel(QPainter* painter, const QColor& markerCol, const QColor& labelCol, const QString& labelText, int zeroPos) const
{
    //qDebug() << "OScopeTimeZeroScaleDraw::drawZeroLabel" << zeroPos << pos();

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

    QRect labelRect = painter->fontMetrics().boundingRect(labelText);

    QPoint markerZeroPoint;
    QPoint labelZeroPoint;

    switch(alignment()){
    case QwtScaleDraw::BottomScale:
        // marker.
        markerZeroPoint.setX(zeroPoint.x() - MARKER_HEIGHT);
        markerZeroPoint.setY(zeroPoint.y() + (labelRect.height() - MARKER_WIDTH) / 2);
        // label.
        labelZeroPoint.setX(zeroPoint.x() + labelRect.width() / 2);
        labelZeroPoint.setY(zeroPoint.y());
        break;
    case QwtScaleDraw::TopScale:
        // marker.
        markerZeroPoint.setX(zeroPoint.x() - MARKER_HEIGHT);
        markerZeroPoint.setY(zeroPoint.y() - (labelRect.height() - MARKER_WIDTH) / 2);
        // label.
        labelZeroPoint.setX(zeroPoint.x() + labelRect.width() / 2);
        labelZeroPoint.setY(zeroPoint.y());
        break;
    case QwtScaleDraw::LeftScale:
        // marker.
        markerZeroPoint.setX(zeroPoint.x() - (labelRect.width() - MARKER_WIDTH) / 2);
        markerZeroPoint.setY(zeroPoint.y() - MARKER_HEIGHT);
        // label.
        labelZeroPoint.setX(zeroPoint.x());
        labelZeroPoint.setY(zeroPoint.y() + labelRect.height() / 2);
        break;
    case QwtScaleDraw::RightScale:
        // marker.
        markerZeroPoint.setX(zeroPoint.x() + (labelRect.width() - MARKER_WIDTH) / 2);
        markerZeroPoint.setY(zeroPoint.y() - MARKER_HEIGHT);
        // label.
        labelZeroPoint.setX(zeroPoint.x());
        labelZeroPoint.setY(zeroPoint.y() + labelRect.height() / 2);
        break;
    }

    //painter->drawRect(pos().x(), pos().y(), 10, 10);

    drawMarker(painter, markerCol, markerZeroPoint, alignment());
    drawLabelText(painter, labelCol, labelZeroPoint, labelText, alignment());
}

void OScopeTimeZeroScaleDraw::drawMarker(QPainter* painter, const QColor& markerCol, const QPoint& zeroPoint, QwtScaleDraw::Alignment align) const
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

void OScopeTimeZeroScaleDraw::drawLabelText(QPainter* painter, const QColor& labelCol, const QPoint& zeroPoint, const QString& labelText, QwtScaleDraw::Alignment align) const
{
    //const QwtScaleMap& map = scaleMap();
    //const QwtScaleDiv& div = scaleDiv();

    QFontMetrics fm = painter->fontMetrics();
    QRect textRect = fm.boundingRect(labelText);

    int markerSpacing = 0;//MARKER_WIDTH / 2;

    //qDebug() << textRect.left() << textRect.top() << textRect.right() << textRect.bottom() << fm.height();

    painter->save();

    painter->translate(zeroPoint);

    switch(align){
    default:
    case QwtScaleDraw::LeftScale:
        painter->translate(-(markerSpacing) - (textRect.right() + 1), -(textRect.bottom() + 1) + textRect.height() / 2);
        break;
    case QwtScaleDraw::RightScale:
        painter->translate(+(markerSpacing) - (textRect.left()),      -(textRect.bottom() + 1) + textRect.height() / 2);
        break;
    case QwtScaleDraw::BottomScale:
        painter->translate(-(textRect.right() + 1) + textRect.width() / 2, +(markerSpacing - (textRect.top()) ));
        break;
    case QwtScaleDraw::TopScale:
        painter->translate(-(textRect.right() + 1) + textRect.width() / 2, -(markerSpacing) - (textRect.bottom() + 1) );
        break;
    }

    painter->setPen(QPen(labelCol));
    //painter->drawRect(textRect);
    painter->setBrush(QBrush(labelCol));
    painter->drawText(0, 0, labelText);

    painter->restore();
}

QString OScopeTimeZeroScaleDraw::readableTimeValue(qreal value) const
{
    return TimeFormatter::formatValue(value);
}
