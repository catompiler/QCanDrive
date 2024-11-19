#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include <QDebug>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_sco = new SLCanOpenNode(this);
}

CanOpenWin::~CanOpenWin()
{
    delete m_sco;
    delete ui;
}

void CanOpenWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)

    auto comm = m_sco->transfer(SDOCommunication::UPLOAD, 1, 0x2000, 0x0, &m_tmp, 4, 1000);
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

