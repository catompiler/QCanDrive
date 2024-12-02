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
#include <QMenu>


CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);

    m_updIntervalms = 100;

    m_layout = new QGridLayout();
    centralWidget()->setLayout(m_layout);

    m_plotsMenu = new QMenu();
    m_plotsMenu->addAction(ui->actEditPlot);
    m_plotsMenu->addAction(ui->actDelPlot);

    m_slcon = new SLCanOpenNode(nullptr);
    connect(m_slcon, &SLCanOpenNode::connected, this, &CanOpenWin::CANopen_connected);
    connect(m_slcon, &SLCanOpenNode::disconnected, this, &CanOpenWin::CANopen_disconnected);

    m_valsHolder = new CoValuesHolder(m_slcon, nullptr);
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

    delete m_trendDlg;
    delete m_signalCurveEditDlg;

    m_plotsMenu->removeAction(ui->actDelPlot);
    m_plotsMenu->removeAction(ui->actEditPlot);
    delete m_plotsMenu;

    // remove all added widgets.
    QLayoutItem *child = nullptr;
    while ((child = m_layout->takeAt(0)) != nullptr) {
        if(child->widget()) delete child->widget(); // delete the widget
        if(child) delete child;   // delete the layout item
    }

    delete m_layout;
    delete ui;

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

void CanOpenWin::on_actAddPlot_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_trendDlg->setPlotName(tr("График %1").arg(m_layout->count() + 1));
    m_trendDlg->setSignalsCount(0);

    if(m_trendDlg->exec()){
        if(m_trendDlg->signalsCount() <= 0) return;

        SDOValuePlot* plt = new SDOValuePlot(m_trendDlg->plotName(), m_valsHolder);

        plt->setPeriod(static_cast<qreal>(m_updIntervalms) / 1000);
        plt->setBufferSize(static_cast<size_t>(m_trendDlg->samplesCount()));
        plt->setBackground(m_trendDlg->backColor());

        int transp = m_trendDlg->transparency();
        plt->setDefaultAlpha( (transp > 0) ? static_cast<qreal>(transp) / 100 : -1.0);

        for(int i = 0; i < m_trendDlg->signalsCount(); i ++){
            auto& sig = m_trendDlg->signalCurveProp(i);

            bool added = plt->addSDOValue(sig.nodeId, sig.index, sig.subIndex, sig.type, sig.name);
            if(!added){
                QMessageBox::warning(this, tr("Ошибка добавления сигнала!"), tr("Невозможно добавить сигнал: \"%1\"").arg(sig.name));
                continue;
            }

            QPen pen;
            pen.setStyle(sig.penStyle);
            pen.setColor(sig.penColor);
            pen.setWidthF(sig.penWidth);
            plt->setPen(i, pen);

            QBrush brush;
            brush.setStyle(sig.brushStyle);
            brush.setColor(sig.brushColor);
            plt->setBrush(i, brush);

            plt->setZ(i, i);
        }

        plt->clear();

        connect(m_slcon, &SLCanOpenNode::connected, plt, &SDOValuePlot::clear);
        m_layout->addWidget(plt, m_trendDlg->posRow(), m_trendDlg->posColumn(), m_trendDlg->sizeRows(), m_trendDlg->sizeColumns());

        plt->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(plt, &SDOValuePlot::customContextMenuRequested, this, &CanOpenWin::showPlotContextMenu);
    }
}

void CanOpenWin::on_actEditPlot_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_plotsMenu->pos());

    //qDebug() << pos;

    auto plt = findSDOValuePlotAt(pos);

    if(plt == nullptr) return;

    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;

    int pltLayIndex = m_layout->indexOf(plt);
    if(pltLayIndex == -1) return;

    m_layout->getItemPosition(pltLayIndex, &row, &col, &rowSpan, &colSpan);

    m_trendDlg->setPosRow(row);
    m_trendDlg->setPosColumn(col);
    m_trendDlg->setSizeRows(rowSpan);
    m_trendDlg->setSizeColumns(colSpan);

    m_trendDlg->setPlotName(plt->name());
    m_trendDlg->setBackColor(plt->background().color());
    m_trendDlg->setTransparency(static_cast<int>(plt->defaultAlpha() * 100));
    m_trendDlg->setSamplesCount(static_cast<int>(plt->bufferSize()));

    m_trendDlg->setSignalsCount(plt->SDOValuesCount());

    for(int i = 0; i < plt->SDOValuesCount(); i ++){
        SignalCurveProp sig;
        auto sdoval = plt->SDOValue(i);

        sig.nodeId = sdoval->nodeId();
        sig.index = sdoval->index();
        sig.subIndex = sdoval->subIndex();
        sig.type = plt->SDValueType(i);

        sig.name = plt->signalName(i);
        sig.penColor = plt->pen(i).color();
        sig.penStyle = plt->pen(i).style();
        sig.penWidth = plt->pen(i).widthF();
        sig.brushColor = plt->brush(i).color();
        sig.brushStyle = plt->brush(i).style();

        m_trendDlg->setSignalCurveProp(i, sig);
    }

    if(m_trendDlg->exec()){
        if(m_trendDlg->signalsCount() <= 0) return;

        plt->delAllSDOValues();

        //qDebug() << m_trendDlg->signalsCount() << plt->signalsCount() << plt->SDOValuesCount();

        plt->setPeriod(static_cast<qreal>(m_updIntervalms) / 1000);
        plt->setBufferSize(static_cast<size_t>(m_trendDlg->samplesCount()));
        plt->setBackground(m_trendDlg->backColor());

        int transp = m_trendDlg->transparency();
        plt->setDefaultAlpha( (transp > 0) ? static_cast<qreal>(transp) / 100 : -1.0);

        for(int i = 0; i < m_trendDlg->signalsCount(); i ++){
            auto& sig = m_trendDlg->signalCurveProp(i);

            bool added = plt->addSDOValue(sig.nodeId, sig.index, sig.subIndex, sig.type, sig.name);
            if(!added){
                QMessageBox::warning(this, tr("Ошибка добавления сигнала!"), tr("Невозможно добавить сигнал: \"%1\"").arg(sig.name));
                continue;
            }

            QPen pen;
            pen.setStyle(sig.penStyle);
            pen.setColor(sig.penColor);
            pen.setWidthF(sig.penWidth);
            plt->setPen(i, pen);

            QBrush brush;
            brush.setStyle(sig.brushStyle);
            brush.setColor(sig.brushColor);
            plt->setBrush(i, brush);

            plt->setZ(i, i);
        }

        plt->clear();

        m_layout->takeAt(pltLayIndex);
        m_layout->addWidget(plt, m_trendDlg->posRow(), m_trendDlg->posColumn(), m_trendDlg->sizeRows(), m_trendDlg->sizeColumns());
    }
}

void CanOpenWin::on_actDelPlot_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_plotsMenu->pos());

    //qDebug() << pos;

    auto plt = findSDOValuePlotAt(pos);

    if(plt == nullptr) return;

    int pltLayIndex = m_layout->indexOf(plt);
    if(pltLayIndex == -1) return;

    m_layout->takeAt(pltLayIndex);

    plt->delAllSDOValues();
    delete plt;
}

void CanOpenWin::CANopen_connected()
{
    qDebug() << "CANopen_connected()";
}

void CanOpenWin::CANopen_disconnected()
{
    qDebug() << "CANopen_disconnected()";
}

SDOValuePlot* CanOpenWin::findSDOValuePlotAt(const QPoint& pos)
{
    SDOValuePlot* plt = nullptr;

    for(int i = 0; i < m_layout->count(); i ++){
        auto item = m_layout->itemAt(i);
        if(QWidget* wgt = item->widget()){
            if(wgt->rect().contains(pos)){
                plt = qobject_cast<SDOValuePlot*>(wgt);
                if(plt != nullptr) break;
            }
        }
    }

    return plt;
}

void CanOpenWin::showPlotContextMenu(const QPoint& pos)
{
    auto plt = qobject_cast<SDOValuePlot*>(sender());
    if(plt == nullptr) return;

    m_plotsMenu->exec(plt->mapToGlobal(pos));
}

