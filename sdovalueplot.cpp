#include "sdovalueplot.h"
#include "sdovalue.h"
#include <algorithm>
#include <QDebug>


#define SDOVALUEPLOT_FALLBACK_VALUE 0.0
#define SDOVALUEPLOT_TIMEOUT_VALUE it->value


SDOValuePlot::SDOValuePlot(const QString& newName, CoValuesHolder* valsHolder, QWidget* parent)
    :SignalPlot(newName, parent)
{
    m_valsHolder = nullptr;
    setValuesHolder(valsHolder);
}

SDOValuePlot::~SDOValuePlot()
{
    if(m_valsHolder){
        for(auto& valData: m_sdoValues){
            m_valsHolder->delSdoValue(valData.sdoval);
        }
        m_sdoValues.clear();
    }
}

CoValuesHolder* SDOValuePlot::valuesHolder() const
{
    return m_valsHolder;
}

void SDOValuePlot::setValuesHolder(CoValuesHolder* newValuesHolder)
{
    if(m_valsHolder){
        disconnect(m_valsHolder, &CoValuesHolder::updateBegin, this, &SDOValuePlot::sdovalsUpdating);
    }
    m_valsHolder = newValuesHolder;
    if(m_valsHolder){
        connect(newValuesHolder, &CoValuesHolder::updateBegin, this, &SDOValuePlot::sdovalsUpdating);
    }
}

bool SDOValuePlot::addSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type type,
                               const QString& newName, const QColor& newColor, const qreal& z)
{
    if(m_valsHolder == nullptr) return false;

    size_t typeSize = COValue::typeSize(type);
    if(typeSize == 0) return false;

    int signal_num = addSignal(newName, newColor, z);
    if(signal_num == -1) return false;

    auto sdoValPtr = m_valsHolder->addSdoValue(newNodeId, newIndex, newSubIndex, typeSize);

    if(sdoValPtr == nullptr){
        removeSignal(signal_num);
        return false;
    }

    m_sdoValues.append({sdoValPtr, true, type, 0.0});

    connect(sdoValPtr, &SDOValue::readed, this, &SDOValuePlot::sdovalueReaded);

    return true;
}

int SDOValuePlot::SDOValuesCount() const
{
    return m_sdoValues.size();
}

CoValuesHolder::HoldedSDOValuePtr SDOValuePlot::getSDOValue(int n) const
{
    if(n < 0 || n >= m_sdoValues.size()) return nullptr;

    return m_sdoValues[n].sdoval;
}

void SDOValuePlot::delSDOValue(int n)
{
    if(n < 0 || n >= m_sdoValues.size()) return;

    auto& item  = m_sdoValues[n];

    removeSignal(n);

    disconnect(item.sdoval, &SDOValue::readed, this, &SDOValuePlot::sdovalueReaded);
    m_valsHolder->delSdoValue(item.sdoval);

    m_sdoValues.removeAt(n);
}

void SDOValuePlot::delSDOValue(CoValuesHolder::HoldedSDOValuePtr sdoval)
{
    if(auto it = std::find_if(m_sdoValues.begin(), m_sdoValues.end(), [sdoval](const auto& val){ return val.sdoval == sdoval; }); it != m_sdoValues.end()){
        delSDOValue(std::distance(m_sdoValues.begin(), it));
    }
}

void SDOValuePlot::delAllSDOValues()
{
    while(SDOValuesCount() > 0){
        delSDOValue(0);
    }
}

COValue::Type SDOValuePlot::SDOValueType(int n) const
{
    if(n < 0 || n >= m_sdoValues.size()) return COValue::Type();

    return m_sdoValues[n].type;
}

void SDOValuePlot::sdovalueReaded()
{
    auto sdoval = qobject_cast<CoValuesHolder::HoldedSDOValuePtr>(sender());
    if(sdoval == nullptr) return;

    int n = 0;
    auto it = m_sdoValues.begin();
    for(; it != m_sdoValues.end(); ++ it, n ++){
        if(it->sdoval == sdoval) break;
    }

    if(it != m_sdoValues.end()){
        it->value = COValue::valueFrom<qreal>(it->sdoval->data(), it->type, SDOVALUEPLOT_FALLBACK_VALUE);
        it->readed = true;
    }
}

void SDOValuePlot::sdovalsUpdating()
{
    //qDebug() << "sdovalsUpdating";

    qreal dt = 0.0;
    if(m_elapsedTimer.isValid()) dt = static_cast<qreal>(m_elapsedTimer.elapsed()) / 1000;
    m_elapsedTimer.start();

    int n = 0;
    for(auto it = m_sdoValues.begin(); it != m_sdoValues.end(); ++ it, n ++){
        putSample(n, it->readed ? it->value : SDOVALUEPLOT_TIMEOUT_VALUE, dt);
        it->readed = false;
    }

    replot();
}

