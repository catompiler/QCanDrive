#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "covaluesholder.h"
#include "covaluetypes.h"
#include "sdovalueplot.h"
#include "signalcurveprop.h"
#include "trendploteditdlg.h"
#include "signalcurveeditdlg.h"
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_updIntervalms = 100;

    m_layout = new QGridLayout();
    centralWidget()->setLayout(m_layout);

    m_slcon = new SLCanOpenNode(this);
    connect(m_slcon, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_slcon, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_valsHolder = new CoValuesHolder(m_slcon);
    m_valsHolder->setUpdateInterval(m_updIntervalms);
    connect(m_slcon, &SLCanOpenNode::connected, m_valsHolder, &CoValuesHolder::enableUpdating);
    connect(m_slcon, &SLCanOpenNode::disconnected, m_valsHolder, &CoValuesHolder::disableUpdating);

    m_signalCurveEditDlg = new SignalCurveEditDlg();
    m_trendDlg = new TrendPlotEditDlg();
    m_trendDlg->setSignalCurveEditDialog(m_signalCurveEditDlg);

    /*m_plot = new SDOValuePlot[2];
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
    m_layout->addWidget(&m_plot[0], 0, 0, 1, 1);
    m_layout->addWidget(&m_plot[1], 0, 1, 2, 1);*/
}

CanOpenWin::~CanOpenWin()
{
    m_slcon->destroyCO();
    m_slcon->closePort();
    //delete[] m_plot;
    delete m_layout;
    delete ui;
    delete m_trendDlg;
    delete m_signalCurveEditDlg;
    delete m_valsHolder;
    delete m_slcon;
}

void CanOpenWin::on_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked)

    close();
}

void CanOpenWin::on_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_trendDlg->setPlotName(tr("График %1").arg(m_layout->count() + 1));
    m_trendDlg->setSignalsCount(0);

    if(m_trendDlg->exec()){
        if(m_trendDlg->signalsCount() <= 0) return;
        auto& d = m_trendDlg;

        SDOValuePlot* plt = new SDOValuePlot(d->plotName(), m_valsHolder);

        plt->setPeriod(static_cast<qreal>(m_updIntervalms) / 1000);
        plt->setBufferSize(static_cast<size_t>(d->samplesCount()));
        plt->setBackground(d->backColor());

        int transp = d->transparency();
        plt->setDefaultAlpha( (transp > 0) ? static_cast<qreal>(transp) / 100 : -1.0);

        for(int i = 0; i < d->signalsCount(); i ++){
            auto& sig = d->signalCurveProp(i);

            bool added = plt->addSDOValue(sig.nodeId, sig.index, sig.subIndex, sig.type, sig.name);
            if(!added){
                QMessageBox::warning(this, tr("Ошибка добавления сигнала!"), tr("Невозможно добавить сигнал: \"%1\"").arg(sig.name));
                continue;
            }

            int n = plt->SDOValueSignalNumber(static_cast<size_t>(i));

            QPen pen;
            pen.setStyle(sig.penStyle);
            pen.setColor(sig.penColor);
            plt->setPen(n, pen);

            QBrush brush;
            brush.setStyle(sig.brushStyle);
            brush.setColor(sig.brushColor);
            plt->setBrush(n, brush);

            plt->setZ(n, i);
        }

        plt->clear();

        connect(m_slcon, &SLCanOpenNode::connected, plt, &SDOValuePlot::clear);
        m_layout->addWidget(plt, d->posRow(), d->posColumn(), d->sizeRows(), d->sizeColumns());
    }
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

