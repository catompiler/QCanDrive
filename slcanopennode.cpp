#include "slcanopennode.h"
#include "coobjectdict.h"
#include "slcan_port_qt.h"
#include <QTimer>
#include <QDebug>



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
    if(!slcan_opened(&m_sc)) return;

    slcan_master_reset(&m_scm);
    slcan_close(&m_sc);
    slcan_reset(&m_sc);
}

bool SLCanOpenNode::createCO()
{
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

    for(;;){
        if(!processFrontComm(dt)) break;
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

    if(sdoc->type() == SDOCommunication::DOWNLOAD){

        switch(sdoc->state()){
        case SDOCommunication::QUEUED:{
            uint32_t cobidCliToSrv = m_cobidClientToServer + sdoc->nodeId();
            uint32_t cobidSrvToCli = m_cobidServerToClient + sdoc->nodeId();
            sdo_ret = CO_SDOclient_setup(sdo_cli, cobidCliToSrv, cobidSrvToCli, sdoc->nodeId());
            if(sdo_ret != CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(SDOCommunication::ERROR_IO);
                return true;
            }
            sdoc->setState(SDOCommunication::INIT);
            }

        __attribute__ ((fallthrough));
        case SDOCommunication::INIT:
            sdo_ret = CO_SDOclientDownloadInitiate(sdo_cli,
                            sdoc->index(), sdoc->subIndex(),
                            sdoc->size(), std::min(static_cast<uint>(sdoc->timeout()), static_cast<uint>(UINT16_MAX)),
                            m_SDOclientBlockTransfer);
            if(sdo_ret < CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(SDOCommunication::ERROR_IO);
                return true;
            }else if(sdo_ret > CO_SDO_RT_ok_communicationEnd){
                break;
            }
            sdoc->setState(SDOCommunication::DATA);

        __attribute__ ((fallthrough));
        case SDOCommunication::DATA:
            size_ret = CO_SDOclientDownloadBufWrite(sdo_cli,
                            static_cast<uint8_t*>(sdoc->dataToTransfer()), sdoc->dataSizeToTransfer());
            sdoc->dataTransfered(size_ret);
            if(sdoc->dataTransferDone()){
                sdoc->setState(SDOCommunication::RUN);
            }

        __attribute__ ((fallthrough));
        case SDOCommunication::RUN:
            sdo_ret = CO_SDOclientDownload(sdo_cli,
                            dt, sdoc->cancelled(), false, &sdo_abort_ret,
                            nullptr, nullptr);
            if(sdo_ret == 0){
                if(sdoc->cancelled()){
                    sdoc->setError(SDOCommunication::ERROR_CANCEL);
                }else{
                    sdoc->setError(SDOCommunication::ERROR_NONE);
                }
                sdoc->setState(SDOCommunication::DONE);
            }else if(sdo_ret < 0){
                SDOCommunication::Error finish_err = SDOCommunication::ERROR_IO;
                if(sdo_abort_ret == CO_SDO_AB_TIMEOUT){
                    finish_err = SDOCommunication::ERROR_TIMEOUT;
                }
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(finish_err);
            }else{
                break;
            }

        __attribute__ ((fallthrough));
        case SDOCommunication::DONE:
            CO_SDOclientClose(sdo_cli);

        __attribute__ ((fallthrough));
        case SDOCommunication::IDLE:
            m_sdoComms.dequeue();
            sdoc->finish();
            return true;
        }
    }else{ // sdoc->type() == SDOCommunication::UPLOAD

        switch(sdoc->state()){
        case SDOCommunication::QUEUED:{
            uint32_t cobidCliToSrv = m_cobidClientToServer + sdoc->nodeId();
            uint32_t cobidSrvToCli = m_cobidServerToClient + sdoc->nodeId();
            sdo_ret = CO_SDOclient_setup(sdo_cli, cobidCliToSrv, cobidSrvToCli, sdoc->nodeId());
            if(sdo_ret != CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(SDOCommunication::ERROR_IO);
                return true;
            }
            sdoc->setState(SDOCommunication::INIT);
            }

        __attribute__ ((fallthrough));
        case SDOCommunication::INIT:
            sdo_ret = CO_SDOclientUploadInitiate(sdo_cli,
                            sdoc->index(), sdoc->subIndex(),
                            std::min(static_cast<uint>(sdoc->timeout()), static_cast<uint>(UINT16_MAX)),
                            m_SDOclientBlockTransfer);
            if(sdo_ret < CO_SDO_RT_ok_communicationEnd){
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(SDOCommunication::ERROR_IO);
                return true;
            }else if(sdo_ret > CO_SDO_RT_ok_communicationEnd){
                break;
            }
            sdoc->setState(SDOCommunication::RUN);

        __attribute__ ((fallthrough));
        case SDOCommunication::RUN:
            sdo_ret = CO_SDOclientUpload(sdo_cli,
                            dt, sdoc->cancelled(), &sdo_abort_ret,
                            &size_to_ret, &size_ret,
                            nullptr);
            if(sdo_ret == 0){
                if(sdoc->cancelled()){
                    sdoc->setState(SDOCommunication::DONE);
                    sdoc->setError(SDOCommunication::ERROR_CANCEL);
                    return true;
                }
                sdoc->setState(SDOCommunication::DATA);
            }else if(sdo_ret < 0){
                SDOCommunication::Error finish_err = SDOCommunication::ERROR_IO;
                if(sdo_abort_ret == CO_SDO_AB_TIMEOUT){
                    finish_err = SDOCommunication::ERROR_TIMEOUT;
                }
                sdoc->setState(SDOCommunication::DONE);
                sdoc->setError(finish_err);
                return true;
            }else{
                if(sdo_ret == CO_SDO_RT_blockUploadInProgress){
                    break;
                }
                if(size_to_ret > 0){
                    if(size_to_ret != sdoc->size()){
                        sdoc->setState(SDOCommunication::DONE);
                        sdoc->setError(SDOCommunication::ERROR_INVALID_SIZE);
                        return true;
                    }
                }
                if(size_ret == 0){
                    break;
                }
            }

        __attribute__ ((fallthrough));
        case SDOCommunication::DATA:
            size_ret = CO_SDOclientUploadBufRead(sdo_cli,
                            static_cast<uint8_t*>(sdoc->dataToTransfer()), sdoc->dataSizeToTransfer());
            sdoc->dataTransfered(size_ret);

            if(!sdoc->dataTransferDone()){
                break;
            }
            sdoc->setState(SDOCommunication::DONE);
            sdoc->setError(SDOCommunication::ERROR_NONE);

        __attribute__ ((fallthrough));
        case SDOCommunication::DONE:
            CO_SDOclientClose(sdo_cli);

        __attribute__ ((fallthrough));
        case SDOCommunication::IDLE:
            m_sdoComms.dequeue();
            sdoc->finish();
            return true;
        }
    }

    return false;
}

int SLCanOpenNode::coTimerInterval() const
{
    return m_coProcessTimer->interval();
}

void SLCanOpenNode::setCoTimerInterval(int newCoTimerInterval)
{
    m_coProcessTimer->setInterval(newCoTimerInterval);
}

SDOCommunication* SLCanOpenNode::read(NodeId devId, Index dataIndex, SubIndex dataSubIndex, void* data, size_t dataSize, SDOCommunication* sdocomm, int timeout)
{
    if(!isConnected()) return nullptr;

    if(data == nullptr || dataSize == 0) return nullptr;
    if(devId < 1 || devId > 127) return nullptr;

    SDOCommunication* sdoc = sdocomm;
    if(sdoc == nullptr){
        sdoc = new SDOCommunication();
    }
    sdoc->setNodeId(devId);
    sdoc->setIndex(dataIndex);
    sdoc->setSubIndex(dataSubIndex);
    sdoc->setData(data);
    sdoc->setSize(dataSize);
    sdoc->setTimeout((timeout == 0) ? m_defaultTimeout : timeout);

    if(!read(sdoc)){
        if(sdocomm == nullptr) delete sdoc;
        return nullptr;
    }

    return sdoc;
}

SDOCommunication* SLCanOpenNode::write(NodeId devId, Index dataIndex, SubIndex dataSubIndex, const void* data, size_t dataSize, SDOCommunication* sdocomm, int timeout)
{
    if(!isConnected()) return nullptr;

    if(data == nullptr || dataSize == 0) return nullptr;
    if(devId < 1 || devId > 127) return nullptr;

    SDOCommunication* sdoc = sdocomm;
    if(sdoc == nullptr){
        sdoc = new SDOCommunication();
    }
    sdoc->setNodeId(devId);
    sdoc->setIndex(dataIndex);
    sdoc->setSubIndex(dataSubIndex);
    sdoc->setData(const_cast<void*>(data));
    sdoc->setSize(dataSize);
    sdoc->setTimeout((timeout == 0) ? m_defaultTimeout : timeout);

    if(!write(sdoc)){
        if(sdocomm == nullptr) delete sdoc;
        return nullptr;
    }

    return sdoc;
}

bool SLCanOpenNode::read(SDOCommunication* sdocom)
{
    if(sdocom == nullptr) return false;
    if(sdocom->size() == 0) return false;
    if(sdocom->data() == nullptr) return false;

    sdocom->resetDataTransfered();
    sdocom->setError(SDOCommunication::ERROR_NONE);
    sdocom->setCancel(false);
    sdocom->setType(SDOCommunication::UPLOAD);
    sdocom->setState(SDOCommunication::QUEUED);
    m_sdoComms.enqueue(sdocom);

    return true;
}

bool SLCanOpenNode::write(SDOCommunication* sdocom)
{
    if(sdocom == nullptr) return false;
    if(sdocom->size() == 0) return false;
    if(sdocom->data() == nullptr) return false;

    sdocom->resetDataTransfered();
    sdocom->setError(SDOCommunication::ERROR_NONE);
    sdocom->setCancel(false);
    sdocom->setType(SDOCommunication::DOWNLOAD);
    sdocom->setState(SDOCommunication::QUEUED);
    m_sdoComms.enqueue(sdocom);

    return true;
}

bool SLCanOpenNode::cancel(SDOCommunication* sdoc)
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
        sdoc->finish(SDOCommunication::ERROR_CANCEL);
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
