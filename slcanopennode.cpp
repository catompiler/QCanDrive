#include "slcanopennode.h"
#include "coobjectdict.h"
#include "slcan_port_qt.h"
#include <QTimer>
#include <QDebug>


#define SDO_COMM_READ_ERROR_ON_SIZE_MISMATCH 0



SLCanOpenNode::SLCanOpenNode(QObject *parent)
    : QObject{parent}
{
    slcan_init(&m_sc);
    slcan_master_init(&m_scm, &m_sc);
    m_co = nullptr;

    m_firstHBTime = 100;
    m_SDOserverTimeout = 500;
    m_SDOclientTimeout = 500;
    m_SDOclientBlockTransfer = true;
    m_nodeId = 127;

    m_cobidClientToServer = 0x600;
    m_cobidServerToClient = 0x580;
    m_heartbeatTime = 0;
    m_defaultTimeout = 1000;

    m_coProcessTimer = new QTimer(this);
    m_coProcessTimer->setSingleShot(false);
    m_coProcessTimer->setInterval(0);
    connect(m_coProcessTimer, &QTimer::timeout, this, &SLCanOpenNode::pollSlcanProcessCO);

    createOd();
}

SLCanOpenNode::~SLCanOpenNode()
{
    slcan_master_deinit(&m_scm);
    slcan_deinit(&m_sc);

    delete m_coProcessTimer;
}

bool SLCanOpenNode::openPort(const QString& name, QSerialPort::BaudRate baud, QSerialPort::Parity parity, QSerialPort::StopBits stopBits)
{
    if(name.isEmpty()) return false;
    if(baud <= 0) return false;

    slcan_port_conf_t port_conf;
    port_conf.baud = SLCAN_PORT_BAUD_115200;
    port_conf.parity = SLCAN_PORT_PARITY_NONE;
    port_conf.stop_bits = SLCAN_PORT_STOP_BITS_1;

    switch (baud) {
    case QSerialPort::Baud2400:
        port_conf.baud = SLCAN_PORT_BAUD_2400;
        break;
    case QSerialPort::Baud9600:
        port_conf.baud = SLCAN_PORT_BAUD_9600;
        break;
    case QSerialPort::Baud19200:
        port_conf.baud = SLCAN_PORT_BAUD_19200;
        break;
    case QSerialPort::Baud38400:
        port_conf.baud = SLCAN_PORT_BAUD_38400;
        break;
    case QSerialPort::Baud57600:
        port_conf.baud = SLCAN_PORT_BAUD_57600;
        break;
    case QSerialPort::Baud115200:
        port_conf.baud = SLCAN_PORT_BAUD_115200;
        break;
    default:
        return false;
    }

    switch(parity){
    case QSerialPort::NoParity:
        port_conf.parity = SLCAN_PORT_PARITY_NONE;
        break;
    case QSerialPort::EvenParity:
        port_conf.parity = SLCAN_PORT_PARITY_EVEN;
        break;
    case QSerialPort::OddParity:
        port_conf.parity = SLCAN_PORT_PARITY_ODD;
        break;
    default:
        return false;
    }

    switch (stopBits) {
    case QSerialPort::OneStop:
        port_conf.stop_bits = SLCAN_PORT_STOP_BITS_1;
        break;
    case QSerialPort::TwoStop:
        port_conf.stop_bits = SLCAN_PORT_STOP_BITS_2;
        break;
    default:
        return false;
    }

    slcan_err_t err = E_SLCAN_NO_ERROR;

    err = slcan_open(&m_sc, name.toUtf8());
    if(err != E_SLCAN_NO_ERROR) return false;

    err = slcan_configure(&m_sc, &port_conf);
    if(err != E_SLCAN_NO_ERROR){
        slcan_close(&m_sc);
        return false;
    }

    QSerialPort* port = slcan_serial_getQSerialPort(slcan_serial_port(&m_sc));
    if(port == nullptr){
        closePort();

        return false;
    }

    connect(port, &QSerialPort::readyRead, this, &SLCanOpenNode::slcanSerialReadyRead);
    connect(port, &QSerialPort::bytesWritten, this, &SLCanOpenNode::slcanSerialBytesWritten);

    return true;
}

void SLCanOpenNode::closePort()
{

    slcan_master_reset(&m_scm);
    slcan_close(&m_sc);
    slcan_reset(&m_sc);
}

bool SLCanOpenNode::createCO()
{
    if(!slcan_opened(&m_sc)) return false;

    QSerialPort* port = slcan_serial_getQSerialPort(slcan_serial_port(&m_sc));
    if(port != nullptr){
        port->blockSignals(true);
    }

    if(m_co != nullptr) return false;

    m_co = CO_new(m_od.config(), nullptr);
    if(m_co == nullptr) return false;

    CO_ReturnError_t co_err = CO_ERROR_NO;

    uint16_t bitRate = 125;

    co_err = CO_CANinit(m_co, &m_scm, bitRate);
    if(co_err != CO_ERROR_NO){
        CO_delete(m_co);
        m_co = nullptr;
        return false;
    }

    uint32_t errInfo;

    co_err = CO_CANopenInit(m_co, nullptr, nullptr, m_od.od(),
                   nullptr, CO_CONFIG_NMT, m_firstHBTime,
                   m_SDOserverTimeout, m_SDOclientTimeout,
                   m_SDOclientBlockTransfer, m_nodeId, &errInfo);

    if(co_err != CO_ERROR_NO){
        qDebug() << "CO_CANopenInit fail:" << co_err << errInfo;
        destroyCO();
        return false;
    }

#if (CO_CONFIG_PDO) != 0
    co_err = CO_CANopenInitPDO(m_co, nullptr, m_od.od(), m_nodeId, &errInfo);

    if(co_err != CO_ERROR_NO){
        qDebug() << "CO_CANopenInitPDO fail:" << errInfo;
        destroyCO();
        return false;
    }
#endif

    m_coProcessTp = meas_clock::now();

    CO_CANsetNormalMode(m_co->CANmodule);

    if(port != nullptr){
        port->blockSignals(false);
    }

    m_coProcessTimer->start();

    emit connected();

    return true;
}

void SLCanOpenNode::destroyCO()
{
    m_coProcessTimer->stop();

    if(m_co == nullptr) return;

    if(m_co->CANmodule != nullptr && m_co->CANmodule->CANptr != nullptr){
        CO_CANsetConfigurationMode(m_co->CANmodule->CANptr);
    }
    CO_delete(m_co);
    m_co = nullptr;

    cancelAllSDOComms();

    emit disconnected();
}

bool SLCanOpenNode::isConnected() const
{
    if(m_co == nullptr) return false;

    return true;
}

quint16 SLCanOpenNode::firstHBTime() const
{
    return m_firstHBTime;
}

void SLCanOpenNode::setFirstHBTime(quint16 newFirstHBTime)
{
    m_firstHBTime = newFirstHBTime;
}

quint16 SLCanOpenNode::SDOserverTimeout() const
{
    return m_SDOserverTimeout;
}

void SLCanOpenNode::setSDOserverTimeout(quint16 newSDOserverTimeout)
{
    m_SDOserverTimeout = newSDOserverTimeout;
}

quint16 SLCanOpenNode::SDOclientTimeout() const
{
    return m_SDOclientTimeout;
}

void SLCanOpenNode::setSDOclientTimeout(quint16 newSDOclientTimeout)
{
    m_SDOclientTimeout = newSDOclientTimeout;
}

bool SLCanOpenNode::SDOclientBlockTransfer() const
{
    return m_SDOclientBlockTransfer;
}

void SLCanOpenNode::setSDOclientBlockTransfer(bool newSDOclientBlockTransfer)
{
    m_SDOclientBlockTransfer = newSDOclientBlockTransfer;
}

quint8 SLCanOpenNode::nodeId() const
{
    return m_nodeId;
}

void SLCanOpenNode::setNodeId(NodeId newNodeId)
{
    m_nodeId = newNodeId;
}

uint32_t SLCanOpenNode::cobidServerToClient() const
{
    return m_cobidServerToClient;
}

void SLCanOpenNode::setCobidServerToClient(uint32_t newCobidServerToClient)
{
    m_cobidServerToClient = newCobidServerToClient;
}

uint32_t SLCanOpenNode::cobidClientToServer() const
{
    return m_cobidClientToServer;
}

void SLCanOpenNode::setCobidClientToServer(uint32_t newCobidClientToServer)
{
    m_cobidClientToServer = newCobidClientToServer;
}

uint16_t SLCanOpenNode::heartbeatTime() const
{
    return m_heartbeatTime;
}

void SLCanOpenNode::setHeartbeatTime(uint16_t newHeartbeatTime)
{
    m_heartbeatTime = newHeartbeatTime;
}

bool SLCanOpenNode::updateOd()
{
    if(isConnected()) return false;

    // clear in function.
    createOd();

    return true;
}

int SLCanOpenNode::defaultTimeout() const
{
    return m_defaultTimeout;
}

void SLCanOpenNode::setDefaultTimeout(int newDefaultTimeout)
{
    m_defaultTimeout = newDefaultTimeout;
}

void SLCanOpenNode::slcanSerialReadyRead()
{
    pollSlcanProcessCO();
}

void SLCanOpenNode::slcanSerialBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes)

    pollSlcanProcessCO();
}

void SLCanOpenNode::pollSlcanProcessCO()
{
    if(m_co == nullptr){
        if(m_coProcessTimer != nullptr){
            m_coProcessTimer->stop();
        }
        return;
    }

    if(!slcan_opened(&m_sc)){
        return;
    }

    slcan_master_poll(&m_scm);
    CO_CANinterrupt(m_co->CANmodule);

    meas_clock::time_point cur_tp = meas_clock::now();
    std::chrono::microseconds dt_dur = std::chrono::duration_cast<std::chrono::microseconds>(cur_tp - m_coProcessTp);
    uint32_t dt = dt_dur.count();
    m_coProcessTp = cur_tp;

    //qDebug() << dt;

    CO_NMT_reset_cmd_t reset_cmd = CO_process(m_co, false, dt, nullptr);

    if(reset_cmd == CO_RESET_NOT){
        //qDebug() << "CO_NMT_NO_COMMAND";
    }else if(reset_cmd == CO_RESET_COMM){
        qDebug() << "CO_RESET_COMM";
    }else if(reset_cmd == CO_RESET_APP){
        qDebug() << "CO_RESET_APP";
    }else if(reset_cmd == CO_RESET_QUIT){
        qDebug() << "CO_RESET_QUIT";
    }

#if (((CO_CONFIG_SYNC)&CO_CONFIG_SYNC_ENABLE) != 0) ||\
    (((CO_CONFIG_PDO)&CO_CONFIG_RPDO_ENABLE) != 0) ||\
    (((CO_CONFIG_PDO)&CO_CONFIG_TPDO_ENABLE) != 0)
    bool syncWas = false;
#endif

#if ((CO_CONFIG_SYNC)&CO_CONFIG_SYNC_ENABLE) != 0
    syncWas = CO_process_SYNC(m_co, dt, nullptr);
#endif

#if ((CO_CONFIG_PDO)&CO_CONFIG_RPDO_ENABLE) != 0
    CO_process_RPDO(m_co, syncWas, dt, nullptr);
#endif

#if ((CO_CONFIG_PDO)&CO_CONFIG_TPDO_ENABLE) != 0
    CO_process_TPDO(m_co, syncWas, dt, nullptr);
#endif

    processSDOClient(dt);
}

void SLCanOpenNode::processSDOClient(uint32_t dt)
{
#if (((CO_CONFIG_SDO_CLI)&CO_CONFIG_SDO_CLI_ENABLE) != 0)
    if(m_co == nullptr || m_co->SDOclient == nullptr) return;

    //CO_SDOclient_t* sdo_cli = m_co->SDOclient;

    for(bool first = true;; first = false){
        if(!processFrontComm(first ? dt : 0)) break;
    }

#endif
}

bool SLCanOpenNode::processFrontComm(uint32_t dt)
{
    if(m_sdoComms.isEmpty()) return false;

    auto sdoc = m_sdoComms.head();

    size_t size_ret = 0;
    size_t size_to_ret = 0;
    CO_SDOclient_t* sdo_cli = m_co->SDOclient;
    CO_SDO_return_t sdo_ret = CO_SDO_RT_ok_communicationEnd;
    CO_SDO_abortCode_t sdo_abort_ret = CO_SDO_AB_NONE;

    if(sdoc->type() == SDOComm::DOWNLOAD){

        switch(sdoc->state()){
        case SDOComm::QUEUED:{
            uint32_t cobidCliToSrv = m_cobidClientToServer + sdoc->nodeId();
            uint32_t cobidSrvToCli = m_cobidServerToClient + sdoc->nodeId();
            sdo_ret = CO_SDOclient_setup(sdo_cli, cobidCliToSrv, cobidSrvToCli, sdoc->nodeId());
            if(sdo_ret != CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(SDOComm::ERROR_IO);
                return true;
            }
            sdoc->setState(SDOComm::INIT);
            }

        __attribute__ ((fallthrough));
        case SDOComm::INIT:
            sdo_ret = CO_SDOclientDownloadInitiate(sdo_cli,
                            sdoc->index(), sdoc->subIndex(), sdoc->transferSize(),
                            std::min(static_cast<uint>(sdoc->timeout() == 0 ? m_defaultTimeout : sdoc->timeout()),
                                     static_cast<uint>(UINT16_MAX)),
                            m_SDOclientBlockTransfer);
            if(sdo_ret < CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(SDOComm::ERROR_IO);
                return true;
            }else if(sdo_ret > CO_SDO_RT_ok_communicationEnd){
                break;
            }
            sdoc->setState(SDOComm::DATA);
            dt = 0;

        __attribute__ ((fallthrough));
        case SDOComm::DATA:
            size_ret = CO_SDOclientDownloadBufWrite(sdo_cli,
                            static_cast<uint8_t*>(sdoc->dataToBuffering()), sdoc->dataSizeToBuffering());
            sdoc->dataBuffered(size_ret);
            if(sdoc->dataBufferingDone()){
                sdoc->setState(SDOComm::RUN);
            }

        __attribute__ ((fallthrough));
        case SDOComm::RUN:
            sdo_ret = CO_SDOclientDownload(sdo_cli,
                            dt, sdoc->cancelled(), !sdoc->dataBufferingDone(), &sdo_abort_ret,
                            &size_ret, nullptr);
            sdoc->setDataTransfered(size_ret);
            if(sdo_ret == 0){
                if(sdoc->cancelled()){
                    sdoc->setError(SDOComm::ERROR_CANCEL);
                }else{
                    sdoc->setError(SDOComm::ERROR_NONE);
                }
                sdoc->setState(SDOComm::DONE);
            }else if(sdo_ret < 0){
                //qDebug() << size_ret << Qt::hex << sdo_abort_ret;
                SDOComm::Error finish_err = sdoCommError(sdo_abort_ret);
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(finish_err);
            }else{
                break;
            }

        __attribute__ ((fallthrough));
        case SDOComm::DONE:
            CO_SDOclientClose(sdo_cli);

        __attribute__ ((fallthrough));
        case SDOComm::IDLE:
            m_sdoComms.dequeue();
            sdoc->finish();
            return true;
        }
    }else{ // sdoc->type() == SDOCommunication::UPLOAD

        switch(sdoc->state()){
        case SDOComm::QUEUED:{
            uint32_t cobidCliToSrv = m_cobidClientToServer + sdoc->nodeId();
            uint32_t cobidSrvToCli = m_cobidServerToClient + sdoc->nodeId();
            sdo_ret = CO_SDOclient_setup(sdo_cli, cobidCliToSrv, cobidSrvToCli, sdoc->nodeId());
            if(sdo_ret != CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(SDOComm::ERROR_IO);
                return true;
            }
            sdoc->setState(SDOComm::INIT);
            }

        __attribute__ ((fallthrough));
        case SDOComm::INIT:
            sdo_ret = CO_SDOclientUploadInitiate(sdo_cli,
                            sdoc->index(), sdoc->subIndex(),
                            std::min(static_cast<uint>(sdoc->timeout() == 0 ? m_defaultTimeout : sdoc->timeout()),
                                     static_cast<uint>(UINT16_MAX)),
                            m_SDOclientBlockTransfer);
            if(sdo_ret < CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(SDOComm::ERROR_IO);
                return true;
            }else if(sdo_ret > CO_SDO_RT_ok_communicationEnd){
                break;
            }
            sdoc->setState(SDOComm::RUN);
            dt = 0;

        __attribute__ ((fallthrough));
        case SDOComm::RUN:
            sdo_ret = CO_SDOclientUpload(sdo_cli,
                            dt, sdoc->cancelled(), &sdo_abort_ret,
                            &size_to_ret, &size_ret,
                            nullptr);
            sdoc->setDataBuffered(size_ret);
            if(sdo_ret == 0){
                if(sdoc->cancelled()){
                    sdoc->setState(SDOComm::DONE);
                    sdoc->setError(SDOComm::ERROR_CANCEL);
                    return true;
                }
                sdoc->setState(SDOComm::DATA);
            }else if(sdo_ret < 0){
                //qDebug() << size_ret << Qt::hex << sdo_abort_ret;
                SDOComm::Error finish_err = sdoCommError(sdo_abort_ret);
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(finish_err);
                return true;
            }else{
                if(sdo_ret == CO_SDO_RT_blockUploadInProgress){
                    break;
                }
#if defined(SDO_COMM_READ_ERROR_ON_SIZE_MISMATCH) && SDO_COMM_READ_ERROR_ON_SIZE_MISMATCH == 1
                if(size_to_ret > 0){
                    if(size_to_ret != sdoc->transferSize()){
                        sdoc->setState(SDOCommunication::DONE);
                        sdoc->setError(SDOCommunication::ERROR_INVALID_SIZE);
                        return true;
                    }
                }
#endif
                if(size_ret == 0){
                    break;
                }
            }

        __attribute__ ((fallthrough));
        case SDOComm::DATA:
            size_ret = CO_SDOclientUploadBufRead(sdo_cli,
                            static_cast<uint8_t*>(sdoc->dataToTransfer()), sdoc->dataSizeToTransfer());
            sdoc->dataTransfered(size_ret);

            if(sdoc->dataTransferDone()){
                sdoc->setState(SDOComm::DONE);
                sdoc->setError(SDOComm::ERROR_NONE);
            }else{
                if(sdoc->state() == SDOComm::DATA && size_ret == 0){
#if defined(SDO_COMM_READ_ERROR_ON_SIZE_MISMATCH) && SDO_COMM_READ_ERROR_ON_SIZE_MISMATCH == 1
                    sdoc->setState(SDOCommunication::DONE);
                    sdoc->setError(SDOCommunication::ERROR_INVALID_SIZE);
#else
                    sdoc->setState(SDOComm::DONE);
                    sdoc->setError(SDOComm::ERROR_NONE);
#endif
                }else{
                    break;
                }
            }

        __attribute__ ((fallthrough));
        case SDOComm::DONE:
            CO_SDOclientClose(sdo_cli);

        __attribute__ ((fallthrough));
        case SDOComm::IDLE:
            m_sdoComms.dequeue();
            sdoc->finish();
            return true;
        }
    }

    return false;
}

SDOComm::Error SLCanOpenNode::sdoCommError(CO_SDO_abortCode_t code) const
{
    //qDebug() << Qt::hex << code;
    switch (code){
    default:
        break;
    case CO_SDO_AB_NONE:
        return SDOComm::ERROR_NONE;
    //case CO_SDO_AB_TOGGLE_BIT:
    case CO_SDO_AB_TIMEOUT:
        return SDOComm::ERROR_TIMEOUT;
    case CO_SDO_AB_CMD:
    case CO_SDO_AB_BLOCK_SIZE:
    case CO_SDO_AB_SEQ_NUM:
    case CO_SDO_AB_CRC:
        return SDOComm::ERROR_IO;
    case CO_SDO_AB_OUT_OF_MEM:
        return SDOComm::ERROR_OUT_OF_MEM;
    case CO_SDO_AB_UNSUPPORTED_ACCESS:
    case CO_SDO_AB_WRITEONLY:
    case CO_SDO_AB_READONLY:
        return SDOComm::ERROR_ACCESS;
    case CO_SDO_AB_NOT_EXIST:
        return SDOComm::ERROR_NOT_FOUND;
    case CO_SDO_AB_NO_MAP:
    case CO_SDO_AB_MAP_LEN:
        return SDOComm::ERROR_INVALID_VALUE;
    case CO_SDO_AB_PRAM_INCOMPAT:
    case CO_SDO_AB_DEVICE_INCOMPAT:
        return SDOComm::ERROR_GENERAL;
    case CO_SDO_AB_HW:
        return SDOComm::ERROR_IO;
    case CO_SDO_AB_TYPE_MISMATCH:
    case CO_SDO_AB_DATA_LONG:
    case CO_SDO_AB_DATA_SHORT:
        return SDOComm::ERROR_INVALID_SIZE;
    case CO_SDO_AB_SUB_UNKNOWN:
        return SDOComm::ERROR_NOT_FOUND;
    case CO_SDO_AB_INVALID_VALUE:
    case CO_SDO_AB_VALUE_HIGH:
    case CO_SDO_AB_VALUE_LOW:
    case CO_SDO_AB_MAX_LESS_MIN:
        return SDOComm::ERROR_INVALID_VALUE;
    case CO_SDO_AB_NO_RESOURCE:
        return SDOComm::ERROR_IO;
    case CO_SDO_AB_GENERAL:
        return SDOComm::ERROR_GENERAL;
    //case CO_SDO_AB_DATA_TRANSF:
    //case CO_SDO_AB_DATA_LOC_CTRL:
    //case CO_SDO_AB_DATA_DEV_STATE:
    case CO_SDO_AB_DATA_OD:
    case CO_SDO_AB_NO_DATA:
        return SDOComm::ERROR_NO_DATA;
    }

    return SDOComm::ERROR_UNKNOWN;
}

int SLCanOpenNode::coTimerInterval() const
{
    return m_coProcessTimer->interval();
}

void SLCanOpenNode::setCoTimerInterval(int newCoTimerInterval)
{
    m_coProcessTimer->setInterval(newCoTimerInterval);
}

SDOComm* SLCanOpenNode::read(NodeId devId, Index dataIndex, SubIndex dataSubIndex, void* data, size_t dataSize, SDOComm* sdocomm, int timeout)
{
    if(!isConnected()) return nullptr;

    if(data == nullptr || dataSize == 0) return nullptr;
    if(devId < 1 || devId > 127) return nullptr;

    SDOComm* sdoc = sdocomm;
    if(sdoc == nullptr){
        sdoc = new SDOComm();
    }
    sdoc->setNodeId(devId);
    sdoc->setIndex(dataIndex);
    sdoc->setSubIndex(dataSubIndex);
    sdoc->setData(data);
    if(sdoc->dataSize() < dataSize){
        sdoc->setDataSize(dataSize);
    }
    sdoc->setTransferSize(dataSize);
    sdoc->setTimeout((timeout == 0) ? m_defaultTimeout : timeout);

    if(!read(sdoc)){
        if(sdocomm == nullptr) delete sdoc;
        return nullptr;
    }

    return sdoc;
}

SDOComm* SLCanOpenNode::write(NodeId devId, Index dataIndex, SubIndex dataSubIndex, const void* data, size_t dataSize, SDOComm* sdocomm, int timeout)
{
    if(!isConnected()) return nullptr;

    if(data == nullptr || dataSize == 0) return nullptr;
    if(devId < 1 || devId > 127) return nullptr;

    SDOComm* sdoc = sdocomm;
    if(sdoc == nullptr){
        sdoc = new SDOComm();
    }
    sdoc->setNodeId(devId);
    sdoc->setIndex(dataIndex);
    sdoc->setSubIndex(dataSubIndex);
    sdoc->setData(const_cast<void*>(data));
    if(sdoc->dataSize() < dataSize){
        sdoc->setDataSize(dataSize);
    }
    sdoc->setTransferSize(dataSize);
    sdoc->setTimeout((timeout == 0) ? m_defaultTimeout : timeout);

    if(!write(sdoc)){
        if(sdocomm == nullptr) delete sdoc;
        return nullptr;
    }

    return sdoc;
}

bool SLCanOpenNode::read(SDOComm* sdocom)
{
    if(!isConnected()) return false;

    if(sdocom == nullptr) return false;
    if(sdocom->dataSize() == 0) return false;
    if(sdocom->data() == nullptr) return false;

    sdocom->resetTransferedSize();
    sdocom->resetBufferedSize();
    sdocom->setError(SDOComm::ERROR_NONE);
    sdocom->setCancel(false);
    sdocom->setType(SDOComm::UPLOAD);
    sdocom->setState(SDOComm::QUEUED);
    m_sdoComms.enqueue(sdocom);

    return true;
}

bool SLCanOpenNode::write(SDOComm* sdocom)
{
    if(!isConnected()) return false;

    if(sdocom == nullptr) return false;
    if(sdocom->dataSize() == 0) return false;
    if(sdocom->data() == nullptr) return false;

    sdocom->resetTransferedSize();
    sdocom->resetBufferedSize();
    sdocom->setError(SDOComm::ERROR_NONE);
    sdocom->setCancel(false);
    sdocom->setType(SDOComm::DOWNLOAD);
    sdocom->setState(SDOComm::QUEUED);
    m_sdoComms.enqueue(sdocom);

    return true;
}

bool SLCanOpenNode::cancel(SDOComm* sdoc)
{
    if(sdoc == nullptr) return true;

    auto it = std::find(m_sdoComms.begin(), m_sdoComms.end(), sdoc);

    if(it == m_sdoComms.end()) return true;

    if(isConnected() && it == m_sdoComms.begin()){
        sdoc->cancel();
        return false;
    }

    m_sdoComms.erase(it);

    return true;
}

void SLCanOpenNode::cancelAllSDOComms()
{
    while(!m_sdoComms.isEmpty()){
        auto sdoc = m_sdoComms.dequeue();
        sdoc->finish(SDOComm::ERROR_CANCEL);
    }
}

void SLCanOpenNode::createOd()
{
    m_od.clear();

    m_od.add_H1000_DevType();
    m_od.add_H1001_ErrReg();

#if ((CO_CONFIG_EM)&CO_CONFIG_EM_HISTORY) != 0
    m_od.add_H1003_PredefErrField();
#endif

#if ((CO_CONFIG_SYNC)&CO_CONFIG_SYNC_ENABLE) != 0
    m_od.add_H1005_CobidSync();
    m_od.add_H1006_CommCyclPeriod();
    m_od.add_H1007_SyncWindowLen();
#endif

#if ((CO_CONFIG_NODE_GUARDING)&CO_CONFIG_NODE_GUARDING_SLAVE_ENABLE) != 0
    m_od.add_H100C_GuardTime();
    m_od.add_H100D_LifetimeFactor();
#endif

#if ((CO_CONFIG_TIME)&CO_CONFIG_TIME_ENABLE) != 0
    m_od.add_H1012_CobidTime();
#endif

#if ((CO_CONFIG_EM)&CO_CONFIG_EM_PRODUCER) != 0
    m_od.add_H1014_CobidEmergency();
#if ((CO_CONFIG_EM)&CO_CONFIG_EM_PROD_INHIBIT) != 0
    m_od.add_H1015_InhibitTimeEmcy();
#endif
#endif

#if ((CO_CONFIG_HB_CONS)&CO_CONFIG_HB_CONS_ENABLE) != 0
    m_od.add_H1016_ConsumerHbTime();
#endif

    auto e_1017 = m_od.add_H1017_ProducerHbTime();
    if(e_1017.isValid()){
        e_1017.write(m_heartbeatTime);
    }

#if ((CO_CONFIG_SYNC)&CO_CONFIG_SYNC_ENABLE) != 0
    m_od.add_H1019_SyncCntOverflow();
#endif

#if (CO_CONFIG_SDO_SRV) != 0
    m_od.add_H1200_SdoServer1Param();
#endif

#if ((CO_CONFIG_SDO_CLI)&CO_CONFIG_SDO_CLI_ENABLE) != 0
    auto e_1280 = m_od.add_H1280_SdoClient1Param();
    if(e_1280.isValid()){
        e_1280.write(m_cobidClientToServer, 1);
        e_1280.write(m_cobidServerToClient, 2);
    }
#endif

#if ((CO_CONFIG_PDO)&CO_CONFIG_RPDO_ENABLE) != 0
    m_od.add_H1400_Rxpdo1Param();
    m_od.add_H1600_Rxpdo1Mapping();
#endif

#if ((CO_CONFIG_PDO)&CO_CONFIG_TPDO_ENABLE) != 0
    m_od.add_H1800_Txpdo1Param();
    m_od.add_H1A00_Txpdo1Mapping();
#endif

    m_od.make();
}
