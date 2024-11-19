#ifndef SLCANOPENNODE_H
#define SLCANOPENNODE_H

#include <QObject>
#include <QSerialPort>
#include "slcan/slcan_master.h"
#include "CANopen.h"
#include "coobjectdict.h"


class SLCanOpenNode : public QObject
{
    Q_OBJECT
public:
    explicit SLCanOpenNode(QObject *parent = nullptr);
    ~SLCanOpenNode();

    bool openPort(const QString& name, QSerialPort::BaudRate baud,
                  QSerialPort::Parity parity, QSerialPort::StopBits stopBits);
    void closePort();

    bool createCO();
    void destroyCO();

signals:

private:
    slcan_t m_sc;
    slcan_master_t m_scm;
    COObjectDict m_od;
    CO_t* m_co;

    void createOd();
};

#endif // SLCANOPENNODE_H
