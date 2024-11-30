#include "covaluesholder.h"
#include "slcanopennode.h"
#include <QTimer>



CoValuesHolder::CoValuesHolder(SLCanOpenNode* slcon, QObject *parent)
    : QObject{parent}
{
    m_slcon = slcon;
    m_updateTimer = new QTimer();
    m_updateTimer->setInterval(500);
    m_updateTimer->setSingleShot(false);
    m_updatingEnabled = false;
    connect(m_updateTimer, &QTimer::timeout, this, &CoValuesHolder::update);
}

CoValuesHolder::~CoValuesHolder()
{
    for(auto it = m_sdoValues.begin(); it != m_sdoValues.end(); ++ it){
        if(it->second == 0){
            delete it->first;
        }else{
            it->first->deleteLater();
        }
    }
    m_sdoValues.clear();
    delete m_updateTimer;
}

SLCanOpenNode* CoValuesHolder::getSLCanOpenNode()
{
    return m_slcon;
}

bool CoValuesHolder::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    if(m_updateTimer->isActive()) return false;

    m_slcon = slcon;

    return true;
}

bool CoValuesHolder::updatingEnabled() const
{
    return m_updatingEnabled;
}

bool CoValuesHolder::setUpdatingEnabled(bool newUpdatingEnabled)
{
    if(m_slcon == nullptr) return false;
    if(newUpdatingEnabled && !m_slcon->isConnected()) return false;

    m_updatingEnabled = newUpdatingEnabled;

    if(m_updatingEnabled && !m_sdoValues.isEmpty() && !m_updateTimer->isActive()){
        m_updateTimer->start();
    }

    return true;
}

int CoValuesHolder::updateInterval() const
{
    return m_updateTimer->interval();
}

void CoValuesHolder::setUpdateInterval(int newUpdateInterval)
{
    m_updateTimer->setInterval(newUpdateInterval);
}

CoValuesHolder::HoldedSDOValuePtr CoValuesHolder::addSdoValue(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex, size_t dataSize, int timeout)
{
    if(m_slcon == nullptr) return nullptr;

    FullIndex valFullIndex = makeFullIndex(valNodeId, valIndex, valSubIndex);

    auto it = m_sdoValues.find(valFullIndex);

    if(it != m_sdoValues.end()){
        it->second ++;
        return HoldedSDOValuePtr(it->first);
    }

    SDOValue* sdoval = new SDOValue(m_slcon);
    sdoval->setNodeId(valNodeId);
    sdoval->setIndex(valIndex);
    sdoval->setSubIndex(valSubIndex);
    sdoval->setDataSize(dataSize);
    sdoval->setTimeout(timeout);

    m_sdoValues.insert(valFullIndex, qMakePair(sdoval, 1));

    if(m_slcon->isConnected() && m_updatingEnabled && !m_updateTimer->isActive()) m_updateTimer->start();

    return HoldedSDOValuePtr(sdoval);
}

void CoValuesHolder::delSdoValue(HoldedSDOValuePtr delSdoVal)
{
    if(delSdoVal == nullptr) return;

    FullIndex valFullIndex = makeFullIndex(delSdoVal->nodeId(), delSdoVal->index(), delSdoVal->subIndex());

    auto it = m_sdoValues.find(valFullIndex);

    if(it == m_sdoValues.end()) return;

    SDOValue* sdoval = it->first;

    if(sdoval != delSdoVal) return;

    if(it->second) it->second --;
    if(it->second == 0){
        if(!sdoval->running()){
            m_sdoValues.erase(it);
            delete sdoval;
        }
    }

    if(m_sdoValues.isEmpty()) m_updateTimer->stop();
}

CoValuesHolder::HoldedSDOValuePtr CoValuesHolder::getSDOValue(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex) const
{
    FullIndex valFullIndex = makeFullIndex(valNodeId, valIndex, valSubIndex);

    auto it = m_sdoValues.find(valFullIndex);

    if(it == m_sdoValues.end()) return HoldedSDOValuePtr(nullptr);

    return HoldedSDOValuePtr(it->first);
}

void CoValuesHolder::update()
{
    emit updateBegin();

    for(auto it = m_sdoValues.begin(); it != m_sdoValues.end(); ){
        SDOValue* sdoval = it->first;
        uint count = it->second;

        if(sdoval->running()){
            ++ it;
            continue;
        }

        if(count == 0){
            it = m_sdoValues.erase(it);
            delete sdoval;
            continue;
        }

        if(m_updatingEnabled) sdoval->read();

        ++ it;
    }

    if(m_sdoValues.isEmpty() || !m_updatingEnabled) m_updateTimer->stop();
}

void CoValuesHolder::enableUpdating()
{
    setUpdatingEnabled(true);
}

void CoValuesHolder::disableUpdating()
{
    setUpdatingEnabled(false);
}

CoValuesHolder::FullIndex CoValuesHolder::makeFullIndex(CO::NodeId valNodeId, CO::Index valIndex, CO::SubIndex valSubIndex) const
{
    FullIndex valFullIndex = 0;

    valFullIndex |= (quint32)valNodeId << 24;
    valFullIndex |= (quint32)valIndex << 8;
    valFullIndex |= (quint32)valSubIndex;

    return valFullIndex;
}
