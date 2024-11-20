#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "sdovalue.h"
#include <QTimer>
#include <QDebug>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_sco = new SLCanOpenNode(this);
    connect(m_sco, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_sco, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_sdo_counter = new SDOValue(m_sco);
    m_sdo_counter->setNodeId(1);
    m_sdo_counter->setIndex(0x2002);
    m_sdo_counter->setSubIndex(0x1);
    m_sdo_counter->setDataSize(4);
    m_sdo_counter->setTimeout(1000);
    connect(m_sdo_counter, &SDOValue::errorOccured, this, [this](){
        SDOValue* sdoval = qobject_cast<SDOValue*>(sender());
        if(sdoval == nullptr) return;

        qDebug() << "error" << sdoval->error();
    });

    connect(m_sdo_counter, &SDOValue::readed, this, [this](){
        SDOValue* sdoval = qobject_cast<SDOValue*>(sender());
        if(sdoval == nullptr) return;

        qDebug() << sdoval->value<uint>();

        QTimer::singleShot(1000, this, [sdoval](){ sdoval->read(); });
    });
    connect(m_sco, &SLCanOpenNode::connected, m_sdo_counter, &SDOValue::read);
}

CanOpenWin::~CanOpenWin()
{
    delete m_sdo_counter;
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

}

void CanOpenWin::CANopen_disconnected()
{

}

