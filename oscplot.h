#ifndef OSCPLOT_H
#define OSCPLOT_H

#include <QWidget>
#include "signalplot.h"

class OscPlot : public SignalPlot
{
    Q_OBJECT
public:
    OscPlot(QWidget* parent = nullptr);
};

#endif // OSCPLOT_H
