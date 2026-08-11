#ifndef OSCOPECHPROP_H
#define OSCOPECHPROP_H

#include <QString>
#include <QColor>
#include "cotypes.h"


struct OScopeChProp {
    bool enabled;
    bool visible;
    QString name;
    CO::Index index;
    CO::SubIndex subIndex;
    qreal baseValue;
    QColor penColor;
    Qt::PenStyle penStyle;
    qreal penWidth;
    QColor brushColor;
    Qt::BrushStyle brushStyle;
};



#endif // OSCOPECHPROP_H
