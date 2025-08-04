#ifndef SLCANOPENNODE_H
#define SLCANOPENNODE_H

#include <QObject>
#include <QSerialPort>
#include <QQueue>
#include <chrono>
#include "slcan/slcan_master.h"
#include "CANopen.h"
#include "coobjectdict.h"
#include "sdocomm.h"


class QTimer;



class SLCanOpenNode : public QObject
{
    Q_OBJECT
public:

    using NodeId = quint8;
    using Index = quint16;
    using SubIndex = quint8;

    explicit SLCanOpenNode(QObject *parent = nullptr);
    ~SLCanOpenNode();

    bool openPort(const QString& name, QSerialPort::BaudRate baud,
                  QSerialPort::Parity parity, QSerialPort::StopBits stopBits);
    void closePort();

    bool createCO(uint newBitrate = 125);
    void destroyCO();

    bool isConnected() const;

    quint16 firstHBTime() const;
    void setFirstHBTime(quint16 newFirstHBTime);

    quint16 SDOserverTimeout() const;
    void setSDOserverTimeout(quint16 newSDOserverTimeout);

    quint16 SDOclientTimeout() const;
    void setSDOclientTimeout(quint16 newSDOclientTimeout);

    bool SDOclientBlockTransfer() const;
    void setSDOclientBlockTransfer(bool newSDOclientBlockTransfer);

    quint8 nodeId() const;
    void setNodeId(NodeId newNodeId);

    uint32_t cobidClientToServer() const;
    void setCobidClientToServer(uint32_t newCobidClientToServer);

    uint32_t cobidServerToClient() const;
    void setCobidServerToClient(uint32_t newCobidServerToClient);

    uint16_t heartbeatTime() const;
    void setHeartbeatTime(uint16_t newHeartbeatTime);

    // SLCan poll & CO process timer interval in ms.
    int coTimerInterval() const;
    void setCoTimerInterval(int newCoTimerInterval);

    int defaultTimeout() const;
    void setDefaultTimeout(int newDefaultTimeout);

    bool adapterNoAnswers() const;
    void setAdapterNoAnswers(bool newNoAnswers);

    bool updateOd();

    /*
     * read & write:
     * timeout == -1 -> max timeout (65535).
     * timeout ==  0 -> default timeout.
     * sdocomm == nullptr -> alloc new sdo comm.
     */

    SDOComm* read(NodeId devId, Index dataIndex, SubIndex dataSubIndex,
                               void* data, size_t dataSize, SDOComm* sdocomm = nullptr, int timeout = -1);

    SDOComm* write(NodeId devId, Index dataIndex, SubIndex dataSubIndex,
                               const void* data, size_t dataSize, SDOComm* sdocomm = nullptr, int timeout = -1);

    bool read(SDOComm* sdocom);
    bool write(SDOComm* sdocom);

    // return true if sdoc removed(not in) from queue and can be deleted or reused.
    // when return true - not finish sdo comm.
    bool cancel(SDOComm* sdoc);

signals:
    void connected();
    void disconnected();

private slots:
    void slcanSerialReadyRead();
    void slcanSerialBytesWritten(qint64 bytes);
    void pollSlcanProcessCO();

private:
    slcan_t m_sc;
    slcan_master_t m_scm;
    COObjectDict m_od;
    CO_t* m_co;

    QTimer* m_coProcessTimer;

    using meas_clock = std::chrono::steady_clock;
    meas_clock::time_point m_coProcessTp;

    quint16 m_firstHBTime;
    quint16 m_SDOserverTimeout;
    quint16 m_SDOclientTimeout;
    bool m_SDOclientBlockTransfer;
    NodeId m_nodeId;
    uint32_t m_cobidClientToServer;
    uint32_t m_cobidServerToClient;
    uint16_t m_heartbeatTime;
    int m_defaultTimeout;

    QQueue<SDOComm*> m_sdoComms;

    void processSDOClient(uint32_t dt);
    bool processFrontComm(uint32_t dt);
    SDOComm::Error sdoCommError(CO_SDO_abortCode_t code) const;
    void cancelAllSDOComms();
    void createOd();
    //void dumpSdoComm(SDOComm* sdoc) const;
};

#endif // SLCANOPENNODE_H
