#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "covaluesholder.h"
#include "sdovalue.h"
#include "trendplot.h"
#include "sequentialbuffer.h"
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    SequentialBuffer* buf = new SequentialBuffer();
    buf->setSamplingPeriod(1.0);
    buf->setSize(10);
    buf->setAddressingMode(SequentialBuffer::CIRCULAR);

    SequentialBuffer* buf2 = new SequentialBuffer();
    buf2->setSamplingPeriod(1.0);
    buf2->setSize(10);
    buf2->setAddressingMode(SequentialBuffer::CIRCULAR);

    connect(this, &CanOpenWin::destroyed, this, [buf, buf2](){
        delete buf;
        delete buf2;
    });

    m_trend = new TrendPlot();
    qDebug() << "trend number:" << m_trend->addTrend(buf, 10);
    qDebug() << "trend number:" << m_trend->addTrend(buf2, 5);
    m_trend->setBrush(0, Qt::red);
    m_trend->setBrush(1, Qt::blue);
    setCentralWidget(m_trend);

    m_slcon = new SLCanOpenNode(this);
    connect(m_slcon, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_slcon, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_valsHolder = new CoValuesHolder(m_slcon);
    m_valsHolder->setUpdateInterval(100);
    connect(m_slcon, &SLCanOpenNode::connected, m_valsHolder, &CoValuesHolder::enableUpdating);
    connect(m_slcon, &SLCanOpenNode::disconnected, m_valsHolder, &CoValuesHolder::disableUpdating);

    auto sdoval = m_valsHolder->addSdoValue(1, 0x2002, 1, 4);
    connect(sdoval, &SDOValue::readed, this, [this, sdoval, buf, buf2](){
        qreal val = 0.01 * sdoval->value<int>();
        buf->put(val);
        buf2->put(val * val);
        qDebug() << val;
        qDebug() << buf->boundingRect();
        m_trend->setAxisScale( QwtAxis::XBottom, buf->boundingRect().left(), buf->boundingRect().right());
        qreal minVal = 0.0;
        for(int i = 0; i < m_trend->trendsCount(); i ++){
            qreal val = m_trend->boundingRect(i).top() - m_trend->boundingRect(i).height();
            if(i == 0) minVal = val;
            minVal = std::min(minVal, val);
        }
        qDebug() << "baseline" << minVal;
        for(int i = 0; i < m_trend->trendsCount(); i ++){
            m_trend->setBaseLine(i, minVal);
        }
        m_trend->replot();
    });
}

CanOpenWin::~CanOpenWin()
{
    delete m_valsHolder;
    delete m_slcon;
    delete m_trend;
    delete ui;
}

void CanOpenWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)
}

void CanOpenWin::on_actConnect_triggered(bool checked)
{
    Q_UNUSED(checked)

    if(m_slcon->isConnected()){
        qDebug() << "Already connected!";
        return;
    }

    bool is_open = m_slcon->openPort("COM23", QSerialPort::Baud115200, QSerialPort::NoParity, QSerialPort::OneStop);
    if(is_open){
        qDebug() << "Port opened!";

        bool co_created = m_slcon->createCO();
        if(co_created){
            qDebug() << "Connected!";
        }else{
            qDebug() << "CO Fail :(";
            m_slcon->closePort();
        }
    }else{
        qDebug() << "Fail :(";
    }
}

void CanOpenWin::on_actDisconnect_triggered(bool checked)
{
    Q_UNUSED(checked)

    if(!m_slcon->isConnected()){
        qDebug() << "Not connected!";
        return;
    }

    m_slcon->destroyCO();
    m_slcon->closePort();

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

