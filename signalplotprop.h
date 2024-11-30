#ifndef SIGNALPLOTPROP_H
#define SIGNALPLOTPROP_H

#include <stddef.h>
#include <QString>
#include <QBrush>


struct SignalPlotProp {
    QString name;
    size_t bufSize;
    qreal defAlpha;
    Qt::BrushStyle backStyle;
    QColor backColor;
};

#endif // SIGNALPLOTPROP_H
