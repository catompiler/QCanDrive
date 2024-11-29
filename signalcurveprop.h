#ifndef SIGNALCURVEPROP_H
#define SIGNALCURVEPROP_H

#include <QString>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"


struct SignalCurveProp
{
    QString name;
    CO::NodeId nodeId;
    CO::Index index;
    CO::SubIndex subIndex;
    COValue::Type type;
    QColor penColor;
    Qt::PenStyle penStyle;
    QColor brushColor;
    Qt::BrushStyle brushStyle;
};

#endif // SIGNALCURVEPROP_H
