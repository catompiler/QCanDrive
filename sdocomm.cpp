#include "sdocomm.h"
#include "sdocomm_data.h"
#include <QDebug>


SDOComm::SDOComm(QObject *parent)
    : QObject{parent}
{
    m_d = new SDOComm_data();
    m_d->m_type = NONE;
    m_d->m_index = 0;
    m_d->m_subIndex = 0;
    m_d->m_data = nullptr;
    m_d->m_dataSize = 0;
    m_d->m_timeout = 0;
    m_d->m_state = IDLE;
    m_d->m_error = ERROR_NONE;
    m_d->m_cancel = false;
    m_d->m_transferSize = 0;
    m_d->m_dataTransfered = 0;
    m_d->m_dataBuffered = 0;
}

SDOComm::~SDOComm()
{
    if(m_d) delete m_d;
}

SDOComm::Type SDOComm::type() const
{
    return m_d->m_type;
}

void SDOComm::setType(Type newType)
{
    m_d->m_type = newType;
}

CO::NodeId SDOComm::nodeId() const
{
    return m_d->m_nodeId;
}

void SDOComm::setNodeId(CO::NodeId newNodeId)
{
    m_d->m_nodeId = newNodeId;
}

CO::Index SDOComm::index() const
{
    return m_d->m_index;
}

void SDOComm::setIndex(CO::Index newIndex)
{
    m_d->m_index = newIndex;
}

CO::SubIndex SDOComm::subIndex() const
{
    return m_d->m_subIndex;
}

void SDOComm::setSubIndex(CO::SubIndex newSubIndex)
{
    m_d->m_subIndex = newSubIndex;
}

void* SDOComm::data()
{
    return m_d->m_data;
}

const void* SDOComm::data() const
{
    return m_d->m_data;
}

void SDOComm::setData(void* newData)
{
    m_d->m_data = newData;
}

size_t SDOComm::dataSize() const
{
    return m_d->m_dataSize;
}

void SDOComm::setState(State newState)
{
    m_d->m_state = newState;
}

int SDOComm::timeout() const
{
    return m_d->m_timeout;
}

void SDOComm::setTimeout(int newTimeout)
{
    m_d->m_timeout = newTimeout;
}

void SDOComm::setDataSize(size_t newSize)
{
    m_d->m_dataSize = newSize;
}

SDOComm::State SDOComm::state() const
{
    return m_d->m_state;
}

SDOComm::Error SDOComm::error() const
{
    return m_d->m_error;
}

void SDOComm::setError(Error newError)
{
    m_d->m_error = newError;
}

bool SDOComm::cancelled() const
{
    return m_d->m_cancel;
}

void SDOComm::setCancel(bool newCancel)
{
    m_d->m_cancel = newCancel;
}

size_t SDOComm::transferSize() const
{
    return m_d->m_transferSize;
}

void SDOComm::setTransferSize(size_t newTransferSize)
{
    m_d->m_transferSize = newTransferSize;
}

void SDOComm::cancel()
{
    setCancel(true);
}

bool SDOComm::running() const
{
    return m_d->m_state != SDOComm::IDLE &&
           m_d->m_state != SDOComm::DONE;
}

void SDOComm::finish()
{
    m_d->m_state = DONE;
    emit finished();
}

void SDOComm::finish(Error err)
{
    m_d->m_error = err;
    finish();
}

size_t SDOComm::transferedDataSize() const
{
    return m_d->m_dataTransfered;
}

void SDOComm::resetTransferedSize()
{
    m_d->m_dataTransfered = 0;
}

void SDOComm::setDataTransfered(size_t size)
{
    m_d->m_dataTransfered = size;
}

void SDOComm::dataTransfered(size_t size)
{
    m_d->m_dataTransfered += size;
}

bool SDOComm::dataTransferDone() const
{
    return m_d->m_dataTransfered >= m_d->m_transferSize;
}

size_t SDOComm::dataSizeToTransfer() const
{
    if(m_d->m_dataTransfered >= m_d->m_transferSize) return 0;

    return m_d->m_transferSize - m_d->m_dataTransfered;
}

void* SDOComm::dataToTransfer() const
{
    if(m_d->m_data == nullptr) return nullptr;

    uint8_t* ptr = static_cast<uint8_t*>(m_d->m_data);

    return &ptr[m_d->m_dataTransfered];
}

size_t SDOComm::bufferedDataSize() const
{
    return m_d->m_dataBuffered;
}

void SDOComm::resetBufferedSize()
{
    m_d->m_dataBuffered = 0;
}

void SDOComm::setDataBuffered(size_t size)
{
    m_d->m_dataBuffered = size;
}

void SDOComm::dataBuffered(size_t size)
{
    m_d->m_dataBuffered += size;
}

bool SDOComm::dataBufferingDone() const
{
    return m_d->m_dataBuffered >= m_d->m_transferSize;
}

size_t SDOComm::dataSizeToBuffering() const
{
    if(m_d->m_dataBuffered >= m_d->m_transferSize) return 0;

    return m_d->m_transferSize - m_d->m_dataBuffered;
}

void* SDOComm::dataToBuffering() const
{
    if(m_d->m_data == nullptr) return nullptr;

    uint8_t* ptr = static_cast<uint8_t*>(m_d->m_data);

    return &ptr[m_d->m_dataBuffered];
}
