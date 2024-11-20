#include "sdovalue.h"
#include "slcanopennode.h"
#include <algorithm>
#include <string.h>


SDOValue::SDOValue(QObject *parent)
    : QObject{parent}
{
    m_sdoc = new SDOCommunication();
    connect(m_sdoc, &SDOCommunication::finished, this, &SDOValue::sdocommFinished);

    m_slcon = nullptr;
}

SDOValue::SDOValue(SLCanOpenNode* slcon, QObject* parent)
    : QObject{parent}
{
    m_sdoc = new SDOCommunication();
    connect(m_sdoc, &SDOCommunication::finished, this, &SDOValue::sdocommFinished);

    m_slcon = slcon;
}

SDOValue::~SDOValue()
{
    if(!m_sdoc->running() || m_slcon == nullptr || m_slcon->cancel(m_sdoc)){
        if(m_sdoc->data()) delete[] static_cast<uint8_t*>(m_sdoc->data());
        delete m_sdoc;
    }else{
        disconnect(m_sdoc, &SDOCommunication::finished, this, &SDOValue::sdocommFinished);

        m_sdoc->cancel();
        connect(m_sdoc, &SDOCommunication::finished, m_sdoc, &SDOCommunication::deleteLater);

        void* ptr_value = m_sdoc->data();
        connect(m_sdoc, &SDOCommunication::destroyed, [ptr_value](){ delete[] static_cast<uint8_t*>(ptr_value); });
    }
}

SLCanOpenNode* SDOValue::getSLCanOpenNode()
{
    return m_slcon;
}

bool SDOValue::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    if(m_sdoc->running()){
        return false;
    }
    m_slcon = slcon;
    return true;
}

size_t SDOValue::dataSize() const
{
    return m_sdoc->size();
}

bool SDOValue::setDataSize(size_t newDataSize)
{
    if(m_sdoc->running()){
        return false;
    }

    if(m_sdoc->size() == newDataSize) return true;

    uint8_t* oldData = static_cast<uint8_t*>(m_sdoc->data());
    uint8_t* newData = new uint8_t[newDataSize];

    if(oldData != nullptr){
        size_t minSize = std::min(m_sdoc->size(), newDataSize);
        memcpy(newData, oldData, minSize);
        delete[] oldData;
    }

    m_sdoc->setData(newData);
    m_sdoc->setSize(newDataSize);

    return true;
}

SDOCommunication::Index SDOValue::index() const
{
    return m_sdoc->index();
}

bool SDOValue::setIndex(SDOCommunication::Index newIndex)
{
    if(running()) return false;

    m_sdoc->setIndex(newIndex);

    return true;
}

SDOCommunication::SubIndex SDOValue::subIndex() const
{
    return m_sdoc->subIndex();
}

bool SDOValue::setSubIndex(SDOCommunication::SubIndex newSubIndex)
{
    if(running()) return false;

    m_sdoc->setSubIndex(newSubIndex);

    return true;
}

SDOCommunication::NodeId SDOValue::nodeId() const
{
    return m_sdoc->nodeId();
}

bool SDOValue::setNodeId(SDOCommunication::NodeId newNodeId)
{
    if(running()) return false;

    m_sdoc->setNodeId(newNodeId);

    return true;
}

int SDOValue::timeout() const
{
    return m_sdoc->timeout();
}

bool SDOValue::setTimeout(int newTimeout)
{
    if(running()) return false;

    m_sdoc->setTimeout(newTimeout);

    return true;
}

SDOCommunication::Error SDOValue::error() const
{
    return m_sdoc->error();
}

bool SDOValue::running() const
{
    return m_sdoc->running();
}


bool SDOValue::read()
{
    if(m_slcon == nullptr) return false;
    if(m_sdoc->data() == nullptr || m_sdoc->size() == 0) return false;

    if(!m_slcon->read(m_sdoc)) return false;

    return true;
}

bool SDOValue::write()
{
    if(m_slcon == nullptr) return false;
    if(m_sdoc->data() == nullptr || m_sdoc->size() == 0) return false;

    if(!m_slcon->write(m_sdoc)) return false;

    return true;
}

void SDOValue::sdocommFinished()
{
    if(m_sdoc->error() != SDOCommunication::ERROR_NONE){
        if(m_sdoc->error() == SDOCommunication::ERROR_CANCEL && m_sdoc->cancelled()){
            emit canceled();
        }else{
            emit errorOccured();
        }
    }else{
        if(m_sdoc->type() == SDOCommunication::UPLOAD){
            emit readed();
        }else if(m_sdoc->type() == SDOCommunication::DOWNLOAD){
            emit written();
        }
    }
    emit finished();
}
