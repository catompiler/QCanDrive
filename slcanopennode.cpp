#include "slcanopennode.h"
#include "coobjectdict.h"
#include <QDebug>



SLCanOpenNode::SLCanOpenNode(QObject *parent)
    : QObject{parent}
{
    slcan_init(&m_sc);
    slcan_master_init(&m_scm, &m_sc);
    m_co = nullptr;
    createOd();
}

SLCanOpenNode::~SLCanOpenNode()
{
    slcan_master_deinit(&m_scm);
    slcan_deinit(&m_sc);
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

    return true;
}

void SLCanOpenNode::closePort()
{
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
    if(co_err != CO_ERROR_NO) return false;

    uint16_t firstHBTime_ms = 100;
    uint16_t SDOserverTimeoutTime_ms = 500;
    uint16_t SDOclientTimeoutTime_ms = 500;
    bool_t SDOclientBlockTransfer = true;
    uint8_t nodeId = NODE_ID;
    uint32_t errInfo;

    co_err =
    CO_CANopenInit(m_co, nullptr, nullptr, m_od.od(),
                   nullptr, CO_CONFIG_NMT, firstHBTime_ms,
                   SDOserverTimeoutTime_ms, SDOclientTimeoutTime_ms,
                   SDOclientBlockTransfer, nodeId, &errInfo);

    if(co_err != CO_ERROR_NO) return false;

    return true;
}

void SLCanOpenNode::destroyCO()
{
    if(m_co != nullptr){
        CO_delete(m_co);
        m_co = nullptr;
    }
    m_od.clear();
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

    m_od.add_H1017_ProducerHbTime();

#if ((CO_CONFIG_SYNC)&CO_CONFIG_SYNC_ENABLE) != 0
    m_od.add_H1019_SyncCntOverflow();
#endif

#if (CO_CONFIG_SDO_SRV) != 0
    m_od.add_H1200_SdoServer1Param();
#endif

#if ((CO_CONFIG_SDO_CLI)&CO_CONFIG_SDO_CLI_ENABLE) != 0
    m_od.add_H1280_SdoClient1Param();
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
