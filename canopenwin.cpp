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

    bool is_open = m_sco->openPort("COM23", QSerialPort::Baud115200, QSerialPort::NoParity, QSerialPort::OneStop);
    if(is_open){
        qDebug() << "Opened!";

        bool co_created = m_sco->createCO();
        if(co_created){
            qDebug() << "Created!";

            m_sco->destroyCO();
        }else{
            qDebug() << "CO Fail :(";
        }
        m_sco->closePort();
    }else{
        qDebug() << "Fail :(";
    }
}

