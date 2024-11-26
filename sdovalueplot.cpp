#include "sdovalueplot.h"



SDOValuePlot::SDOValuePlot(CoValuesHolder* valsHolder, QWidget* parent)
    :SignalPlot(parent)
{
    m_valsHolder = valsHolder;
}

SDOValuePlot::~SDOValuePlot()
{
}

CoValuesHolder* SDOValuePlot::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValuePlot::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    m_valsHolder = newValuesHolder;
}

bool SDOValuePlot::addSDOValueSource(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex)
{
    if(m_valsHolder == nullptr) return false;

    auto sdoValPtr = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, 4);

    if(sdoValPtr == nullptr) return false;

    m_sdoValues.append(sdoValPtr);

    return true;
}
