#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "covaluesholder.h"
#include "covaluetypes.h"
#include "sdovalueplot.h"
#include "trendploteditdlg.h"
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QHBoxLayout>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_slcon = new SLCanOpenNode(this);
    connect(m_slcon, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_slcon, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_valsHolder = new CoValuesHolder(m_slcon);
    m_valsHolder->setUpdateInterval(100);
    connect(m_slcon, &SLCanOpenNode::connected, m_valsHolder, &CoValuesHolder::enableUpdating);
    connect(m_slcon, &SLCanOpenNode::disconnected, m_valsHolder, &CoValuesHolder::disableUpdating);

    m_trendDlg = new TrendPlotEditDlg();

    m_plot = new SDOValuePlot[2];
    m_plot[0].setTitle("Sine");
    m_plot[0].setValuesHolder(m_valsHolder);
    m_plot[0].setBufferSize(100);
    m_plot[0].addSDOValue(1, 0x2002, 1, COValue::I32, "Value", Qt::blue, 1);
    connect(m_slcon, &SLCanOpenNode::connected, &m_plot[0], &SDOValuePlot::clear);
    m_plot[1].setTitle("JAS");
    m_plot[1].setValuesHolder(m_valsHolder);
    m_plot[1].setBufferSize(100);
    m_plot[1].addSDOValue(1, 0x2002, 2, COValue::I32, "", Qt::red, 2);
    connect(m_slcon, &SLCanOpenNode::connected, &m_plot[1], &SDOValuePlot::clear);
    QHBoxLayout* lay = new QHBoxLayout();
    lay->addWidget(&m_plot[0]);
    lay->addWidget(&m_plot[1]);
    centralWidget()->setLayout(lay);
}

CanOpenWin::~CanOpenWin()
{
    m_slcon->destroyCO();
    m_slcon->closePort();
    delete[] m_plot;
    delete m_trendDlg;
    delete m_valsHolder;
    delete m_slcon;
    delete ui;
}

void CanOpenWin::on_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked)

    close();
}

void CanOpenWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_trendDlg->exec();
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

