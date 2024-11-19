#include "sdocommunication.h"
#include "sdocommunication_data.h"


SDOCommunication::SDOCommunication(QObject *parent)
    : QObject{parent}
{
    m_d = new SDOCommunication_data();
    m_d->m_type = NONE;
    m_d->m_index = 0;
    m_d->m_subIndex = 0;
    m_d->m_data = nullptr;
    m_d->m_size = 0;
    m_d->m_timeout = 0;
    m_d->m_state = IDLE;
    m_d->m_error = ERROR_NONE;
    m_d->m_cancel = false;
    m_d->m_dataTransfered = 0;
}

/*SDOCommunication::SDOCommunication(const SDOCommunication& sdo_comm)
    :QObject(sdo_comm)
{
    m_d = new SDOCommunication_data();
    m_d->m_type = sdo_comm.m_d->m_type;
    m_d->m_index = sdo_comm.m_d->m_index;
    m_d->m_subIndex = sdo_comm.m_d->m_subIndex;
    m_d->m_data = sdo_comm.m_d->m_data;
    m_d->m_size = sdo_comm.m_d->m_size;
    m_d->m_timeout = sdo_comm.m_d->m_timeout;
    m_d->m_state = sdo_comm.m_d->m_state;
    m_d->m_error = sdo_comm.m_d->m_error;
    m_d->m_cancel = sdo_comm.m_d->m_cancel;
    m_d->m_dataTransfered = sdo_comm.m_d->m_dataTransfered;
}

SDOCommunication::SDOCommunication(SDOCommunication&& sdo_comm)
    :QObject(sdo_comm)
{
    SDOCommunication_data* d = sdo_comm.m_d;
    sdo_comm.m_d = nullptr;
    m_d = d;
}*/

SDOCommunication::~SDOCommunication()
{
    if(m_d) delete m_d;
}

SDOCommunication::Type SDOCommunication::type() const
{
    return m_d->m_type;
}

void SDOCommunication::setType(Type newType)
{
    m_d->m_type = newType;
}

SDOCommunication::NodeId SDOCommunication::nodeId() const
{
    return m_d->m_nodeId;
}

void SDOCommunication::setNodeId(NodeId newNodeId)
{
    m_d->m_nodeId = newNodeId;
}

SDOCommunication::Index SDOCommunication::index() const
{
    return m_d->m_index;
}

void SDOCommunication::setIndex(Index newIndex)
{
    m_d->m_index = newIndex;
}

SDOCommunication::SubIndex SDOCommunication::subIndex() const
{
    return m_d->m_subIndex;
}

void SDOCommunication::setSubIndex(SubIndex newSubIndex)
{
    m_d->m_subIndex = newSubIndex;
}

void* SDOCommunication::data() const
{
    return m_d->m_data;
}

void SDOCommunication::setData(void* newData)
{
    m_d->m_data = newData;
}

size_t SDOCommunication::size() const
{
    return m_d->m_size;
}

void SDOCommunication::setState(State newState)
{
    m_d->m_state = newState;
}

int SDOCommunication::timeout() const
{
    return m_d->m_timeout;
}

void SDOCommunication::setTimeout(int newTimeout)
{
    m_d->m_timeout = newTimeout;
}

void SDOCommunication::setSize(size_t newSize)
{
    m_d->m_size = newSize;
}

SDOCommunication::State SDOCommunication::state() const
{
    return m_d->m_state;
}

SDOCommunication::Error SDOCommunication::error() const
{
    return m_d->m_error;
}

void SDOCommunication::setError(Error newError)
{
    m_d->m_error = newError;
}

bool SDOCommunication::cancel() const
{
    return m_d->m_cancel;
}

void SDOCommunication::setCancel(bool newCancel)
{
    m_d->m_cancel = newCancel;
}

void SDOCommunication::finish()
{
    m_d->m_state = DONE;
    emit finished();
}

void SDOCommunication::finish(Error err)
{
    m_d->m_error = err;
    finish();
}

void SDOCommunication::resetDataTransfered()
{
    m_d->m_dataTransfered = 0;
}

void SDOCommunication::dataTransfered(size_t size)
{
    m_d->m_dataTransfered += size;
}

bool SDOCommunication::dataTransferDone() const
{
    return m_d->m_dataTransfered >= m_d->m_size;
}

size_t SDOCommunication::dataSizeToTransfer() const
{
    if(m_d->m_dataTransfered >= m_d->m_size) return 0;
    return m_d->m_size - m_d->m_dataTransfered;
}

void* SDOCommunication::dataToTransfer() const
{
    uint8_t* ptr = static_cast<uint8_t*>(m_d->m_data);

    return &ptr[m_d->m_dataTransfered];
}
