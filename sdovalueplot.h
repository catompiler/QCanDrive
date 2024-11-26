#ifndef SDOVALUEPLOT_H
#define SDOVALUEPLOT_H

#include "signalplot.h"
#include "cotypes.h"
#include "sdovalue.h"
#include "covaluesholder.h"
#include <QList>



class SDOValuePlot : public SignalPlot
{
    Q_OBJECT
public:
    SDOValuePlot(CoValuesHolder* valsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValuePlot();

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    bool addSDOValueSource(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex);

protected:
    CoValuesHolder* m_valsHolder;

    QList<CoValuesHolder::HoldedSDOValuePtr> m_sdoValues;
};

#endif // SDOVALUEPLOT_H
