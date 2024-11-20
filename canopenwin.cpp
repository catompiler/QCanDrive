#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "sdovalue.h"
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_sco = new SLCanOpenNode(this);
    connect(m_sco, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_sco, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_sdo_val = new SDOValue(m_sco);
    m_sdo_val->setNodeId(1);
    m_sdo_val->setIndex(0x2004);
    m_sdo_val->setSubIndex(0x0);
    m_sdo_val->setDataSize(32);
    m_sdo_val->setTransferSize(16);
    m_sdo_val->setTimeout(1000);
    for(int i = 0; i < static_cast<int>(m_sdo_val->dataSize()); i ++){
        m_sdo_val->setValueAt<quint8>(i, i);
    }

    connect(m_sdo_val, &SDOValue::errorOccured, this, [this](){
        SDOValue* sdoval = qobject_cast<SDOValue*>(sender());
        if(sdoval == nullptr) return;

        qDebug() << ((sdoval->transferType() == SDOCommunication::UPLOAD) ? "read" : "write")
                 << "error" << sdoval->error();
    });

    connect(m_sdo_val, &SDOValue::written, this, [this](){
        SDOValue* sdoval = qobject_cast<SDOValue*>(sender());
        if(sdoval == nullptr) return;

        qDebug() << "written" << sdoval->transferedDataSize();

        /*for(int i = 0; i < static_cast<int>(sdoval->dataSize()); i ++){
            sdoval->setValueAt<quint8>(i, 0);
        }*/
        //qDebug() << sdoval->value<uint>();

        QTimer::singleShot(1000, this, [sdoval](){ sdoval->read(); });
    });

    connect(m_sdo_val, &SDOValue::readed, this, [this](){
        SDOValue* sdoval = qobject_cast<SDOValue*>(sender());
        if(sdoval == nullptr) return;

        qDebug() << "readed" << sdoval->transferedDataSize();

        QStringList values;

        for(int i = 0; i < static_cast<int>(sdoval->transferedDataSize()); i ++){
            values.append(QString::number(sdoval->valueAt<quint8>(i)));
        }

        qDebug() << values.join(", ");
        //qDebug() << sdoval->value<uint>();

        QTimer::singleShot(1000, this, [sdoval](){ sdoval->read(); });
    });

    connect(m_sco, &SLCanOpenNode::connected, m_sdo_val, &SDOValue::write);
    //connect(m_sco, &SLCanOpenNode::connected, m_sdo_val, &SDOValue::read);
    /*connect(m_sco, &SLCanOpenNode::connected, this, [this](){
        if(!m_sdo_val->write()){
            qDebug() << "write() fail! state:" << m_sdo_val->state();
        }
    });*/
}

CanOpenWin::~CanOpenWin()
{
    delete m_sdo_val;
    delete m_sco;
    delete ui;
}

void CanOpenWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_tmp = 1;
    auto comm = m_sco->read(1, 0x2000, 0x0, &m_tmp, 4);
    if(comm != nullptr){
        connect(comm, &SDOCommunication::finished, this, [this](){
            SDOCommunication* comm = qobject_cast<SDOCommunication*>(sender());

            if(comm == nullptr){
                qDebug() << "NULL sender!";
                return;
            }

            comm->deleteLater();

            if(comm->error() == SDOCommunication::ERROR_NONE){
                qDebug() << "Readed:" << m_tmp;
            }else{
                qDebug() << "Error:" << static_cast<uint>(comm->error());
            }
        });
    }
}

void CanOpenWin::on_actConnect_triggered(bool checked)
{
    Q_UNUSED(checked)

    if(m_sco->isConnected()){
        qDebug() << "Already connected!";
        return;
    }

    bool is_open = m_sco->openPort("COM23", QSerialPort::Baud115200, QSerialPort::NoParity, QSerialPort::OneStop);
    if(is_open){
        qDebug() << "Port opened!";

        bool co_created = m_sco->createCO();
        if(co_created){
            qDebug() << "Connected!";
        }else{
            qDebug() << "CO Fail :(";
            m_sco->closePort();
        }
    }else{
        qDebug() << "Fail :(";
    }
}

void CanOpenWin::on_actDisconnect_triggered(bool checked)
{
    Q_UNUSED(checked)

    if(!m_sco->isConnected()){
        qDebug() << "Not connected!";
        return;
    }

    m_sco->destroyCO();
    m_sco->closePort();

    qDebug() << "Disconnected!";
}

void CanOpenWin::CANopen_connected()
{
    qDebug() << "CANopen_connected()";
}

void CanOpenWin::CANopen_disconnected()
{
    qDebug() << "CANopen_disconnected()";
}

