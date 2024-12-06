#include "canopenwin.h"
#include "ui_canopenwin.h"
#include "slcanopennode.h"
#include "covaluesholder.h"
#include "covaluetypes.h"
#include "sdovalueplot.h"
#include "sdovaluedial.h"
#include "sdovalueslider.h"
#include "sdovaluebar.h"
#include "sdovaluebutton.h"
#include "signalcurveprop.h"
#include "trendploteditdlg.h"
#include "signalcurveeditdlg.h"
#include "sdovaluedialeditdlg.h"
#include "sdovalueslidereditdlg.h"
#include "sdovaluebareditdlg.h"
#include "sdovaluebuttoneditdlg.h"
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

    QPalette darkPal(palette());
    darkPal.setColor(QPalette::Window, QColor(Qt::darkGray).darker(240));
    setPalette(darkPal);

    m_updIntervalms = 100;

    m_layout = new QGridLayout();
    centralWidget()->setLayout(m_layout);

    addAction(ui->actAddPlot);
    addAction(ui->actAddDial);
    addAction(ui->actAddSlider);
    addAction(ui->actAddBar);
    addAction(ui->actAddButton);
    //addAction(ui->actAdd);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_plotsMenu = new QMenu();
    m_plotsMenu->addAction(ui->actEditPlot);
    m_plotsMenu->addAction(ui->actDelPlot);

    m_dialsMenu = new QMenu();
    m_dialsMenu->addAction(ui->actEditDial);
    m_dialsMenu->addAction(ui->actDelDial);

    m_slidersMenu = new QMenu();
    m_slidersMenu->addAction(ui->actEditSlider);
    m_slidersMenu->addAction(ui->actDelSlider);

    m_barsMenu = new QMenu();
    m_barsMenu->addAction(ui->actEditBar);
    m_barsMenu->addAction(ui->actDelBar);

    m_buttonsMenu = new QMenu();
    m_buttonsMenu->addAction(ui->actEditButton);
    m_buttonsMenu->addAction(ui->actDelButton);

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

    m_dialDlg = new SDOValueDialEditDlg();

    m_sliderDlg = new SDOValueSliderEditDlg();

    m_barDlg = new SDOValueBarEditDlg();

    m_buttonDlg = new SDOValueButtonEditDlg();
}

CanOpenWin::~CanOpenWin()
{
    m_slcon->destroyCO();
    m_slcon->closePort();

    delete m_buttonDlg;

    delete m_barDlg;

    delete m_sliderDlg;

    delete m_dialDlg;

    delete m_trendDlg;
    delete m_signalCurveEditDlg;

    m_buttonsMenu->removeAction(ui->actDelButton);
    m_buttonsMenu->removeAction(ui->actEditButton);
    delete m_buttonsMenu;

    m_barsMenu->removeAction(ui->actDelBar);
    m_barsMenu->removeAction(ui->actEditBar);
    delete m_barsMenu;

    m_slidersMenu->removeAction(ui->actDelSlider);
    m_slidersMenu->removeAction(ui->actEditSlider);
    delete m_slidersMenu;

    m_dialsMenu->removeAction(ui->actDelDial);
    m_dialsMenu->removeAction(ui->actEditDial);
    delete m_dialsMenu;

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
        plt->setTextColor(m_trendDlg->textColor());

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

        plt->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(plt, &SDOValuePlot::customContextMenuRequested, this, &CanOpenWin::showPlotContextMenu);
        connect(m_slcon, &SLCanOpenNode::connected, plt, &SDOValuePlot::clear);

        m_layout->addWidget(plt, m_trendDlg->posRow(), m_trendDlg->posColumn(), m_trendDlg->sizeRows(), m_trendDlg->sizeColumns());
    }
}

void CanOpenWin::on_actEditPlot_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_plotsMenu->pos());

    //qDebug() << pos;

    auto plt = findWidgetTypeAt<SDOValuePlot>(pos);

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
    m_trendDlg->setTextColor(plt->textColor());
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
        plt->setTextColor(m_trendDlg->textColor());

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

    auto plt = findWidgetTypeAt<SDOValuePlot>(pos);

    if(plt == nullptr) return;

    int pltLayIndex = m_layout->indexOf(plt);
    if(pltLayIndex == -1) return;

    m_layout->takeAt(pltLayIndex);

    plt->delAllSDOValues();
    delete plt;
}

void CanOpenWin::on_actAddDial_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_dialDlg->setName(tr("Прибор %1").arg(m_layout->count() + 1));

    if(m_dialDlg->exec()){
        auto dial = new SDOValueDial(m_valsHolder, nullptr);

        bool added = dial->setSDOValue(m_dialDlg->nodeId(), m_dialDlg->index(), m_dialDlg->subIndex(), m_dialDlg->type(), m_dialDlg->rangeMin(), m_dialDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка добавления показометра!"), tr("Невозможно добавить показометр: \"%1\"").arg(m_dialDlg->name()));
            return;
        }

        dial->setName(m_dialDlg->name());
        dial->setInsideBackColor(m_dialDlg->insideBackColor());
        dial->setOutsideBackColor(m_dialDlg->outsideBackColor());
        dial->setInsideScaleBackColor(m_dialDlg->insideScaleBackColor());
        dial->setTextScaleColor(m_dialDlg->textScaleColor());
        dial->setNeedleColor(m_dialDlg->needleColor());
        dial->setPenWidth(m_dialDlg->penWidth());
        dial->setPrecision(m_dialDlg->precision());

        dial->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(dial, &SDOValueDial::customContextMenuRequested, this, &CanOpenWin::showDialsContextMenu);

        m_layout->addWidget(dial, m_dialDlg->posRow(), m_dialDlg->posColumn(), m_dialDlg->sizeRows(), m_dialDlg->sizeColumns());
    }
}

void CanOpenWin::on_actEditDial_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_dialsMenu->pos());

    //qDebug() << pos;

    auto dial = findWidgetTypeAt<SDOValueDial>(pos);

    //qDebug() << dial;

    if(dial == nullptr) return;

    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;

    int dialLayIndex = m_layout->indexOf(dial);
    if(dialLayIndex == -1) return;

    m_layout->getItemPosition(dialLayIndex, &row, &col, &rowSpan, &colSpan);

    m_dialDlg->setPosRow(row);
    m_dialDlg->setPosColumn(col);
    m_dialDlg->setSizeRows(rowSpan);
    m_dialDlg->setSizeColumns(colSpan);

    m_dialDlg->setName(dial->name());
    m_dialDlg->setOutsideBackColor(dial->outsideBackColor());
    m_dialDlg->setInsideBackColor(dial->insideBackColor());
    m_dialDlg->setInsideScaleBackColor(dial->insideScaleBackColor());
    m_dialDlg->setTextScaleColor(dial->textScaleColor());
    m_dialDlg->setNeedleColor(dial->needleColor());
    m_dialDlg->setPenWidth(dial->penWidth());
    m_dialDlg->setPrecision(dial->precision());

    m_dialDlg->setRangeMin(dial->rangeMin());
    m_dialDlg->setRangeMax(dial->rangeMax());

    auto sdoval = dial->getSDOValue();

    m_dialDlg->setNodeId(sdoval->nodeId());
    m_dialDlg->setIndex(sdoval->index());
    m_dialDlg->setSubIndex(sdoval->subIndex());
    m_dialDlg->setType(dial->SDOValueType());

    if(m_dialDlg->exec()){
        dial->resetSDOValue();

        bool added = dial->setSDOValue(m_dialDlg->nodeId(), m_dialDlg->index(), m_dialDlg->subIndex(), m_dialDlg->type(), m_dialDlg->rangeMin(), m_dialDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка изменения показометра!"), tr("Невозможно изменить показометр: \"%1\"").arg(m_dialDlg->name()));
            return;
        }

        dial->setName(m_dialDlg->name());
        dial->setInsideBackColor(m_dialDlg->insideBackColor());
        dial->setOutsideBackColor(m_dialDlg->outsideBackColor());
        dial->setInsideScaleBackColor(m_dialDlg->insideScaleBackColor());
        dial->setTextScaleColor(m_dialDlg->textScaleColor());
        dial->setNeedleColor(m_dialDlg->needleColor());
        dial->setPenWidth(m_dialDlg->penWidth());
        dial->setPrecision(m_dialDlg->precision());

        m_layout->takeAt(dialLayIndex);
        m_layout->addWidget(dial, m_dialDlg->posRow(), m_dialDlg->posColumn(), m_dialDlg->sizeRows(), m_dialDlg->sizeColumns());
    }
}

void CanOpenWin::on_actDelDial_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_dialsMenu->pos());

    //qDebug() << pos;

    auto dial = findWidgetTypeAt<SDOValueDial>(pos);

    if(dial == nullptr) return;

    int dialLayIndex = m_layout->indexOf(dial);
    if(dialLayIndex == -1) return;

    m_layout->takeAt(dialLayIndex);

    dial->resetSDOValue();
    delete dial;
}

void CanOpenWin::on_actAddSlider_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_sliderDlg->setName(tr("Прибор %1").arg(m_layout->count() + 1));

    if(m_sliderDlg->exec()){
        auto slider = new SDOValueSlider(m_valsHolder, nullptr);

        bool added = slider->setSDOValue(m_sliderDlg->nodeId(), m_sliderDlg->index(), m_sliderDlg->subIndex(), m_sliderDlg->type(), m_sliderDlg->rangeMin(), m_sliderDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка добавления слайдера!"), tr("Невозможно добавить слайдер: \"%1\"").arg(m_sliderDlg->name()));
            return;
        }

        slider->setName(m_sliderDlg->name());
        slider->setTroughColor(m_sliderDlg->troughColor());
        slider->setGrooveColor(m_sliderDlg->grooveColor());
        slider->setHandleColor(m_sliderDlg->handleColor());
        slider->setScaleColor(m_sliderDlg->scaleColor());
        slider->setTextColor(m_sliderDlg->textColor());
        slider->setPenWidth(m_sliderDlg->penWidth());
        slider->setSteps(m_sliderDlg->steps());
        slider->setHasTrough(m_sliderDlg->hasTrough());
        slider->setHasGroove(m_sliderDlg->hasGroove());
        slider->setOrientation(m_sliderDlg->orientation());

        slider->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(slider, &SDOValueSlider::customContextMenuRequested, this, &CanOpenWin::showSlidersContextMenu);

        m_layout->addWidget(slider, m_sliderDlg->posRow(), m_sliderDlg->posColumn(), m_sliderDlg->sizeRows(), m_sliderDlg->sizeColumns());
    }
}

void CanOpenWin::on_actEditSlider_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_slidersMenu->pos());

    //qDebug() << pos;

    auto slider = findWidgetTypeAt<SDOValueSlider>(pos);

    //qDebug() << slider;

    if(slider == nullptr) return;

    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;

    int sliderLayIndex = m_layout->indexOf(slider);
    if(sliderLayIndex == -1) return;

    m_layout->getItemPosition(sliderLayIndex, &row, &col, &rowSpan, &colSpan);

    m_sliderDlg->setPosRow(row);
    m_sliderDlg->setPosColumn(col);
    m_sliderDlg->setSizeRows(rowSpan);
    m_sliderDlg->setSizeColumns(colSpan);

    m_sliderDlg->setName(slider->name());
    m_sliderDlg->setTroughColor(slider->troughColor());
    m_sliderDlg->setGrooveColor(slider->grooveColor());
    m_sliderDlg->setHandleColor(slider->handleColor());
    m_sliderDlg->setScaleColor(slider->scaleColor());
    m_sliderDlg->setTextColor(slider->textColor());
    m_sliderDlg->setPenWidth(slider->penWidth());
    m_sliderDlg->setSteps(slider->steps());
    m_sliderDlg->setHasTrough(slider->hasTrough());
    m_sliderDlg->setHasGroove(slider->hasGroove());
    m_sliderDlg->setOrientation(slider->orientation());

    m_sliderDlg->setRangeMin(slider->rangeMin());
    m_sliderDlg->setRangeMax(slider->rangeMax());

    auto sdoval = slider->getSDOValue();

    m_sliderDlg->setNodeId(sdoval->nodeId());
    m_sliderDlg->setIndex(sdoval->index());
    m_sliderDlg->setSubIndex(sdoval->subIndex());
    m_sliderDlg->setType(slider->SDOValueType());

    if(m_sliderDlg->exec()){
        slider->resetSDOValue();

        bool added = slider->setSDOValue(m_sliderDlg->nodeId(), m_sliderDlg->index(), m_sliderDlg->subIndex(), m_sliderDlg->type(), m_sliderDlg->rangeMin(), m_sliderDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка изменения слайдера!"), tr("Невозможно изменить слайдер: \"%1\"").arg(m_sliderDlg->name()));
            return;
        }

        slider->setName(m_sliderDlg->name());
        slider->setTroughColor(m_sliderDlg->troughColor());
        slider->setGrooveColor(m_sliderDlg->grooveColor());
        slider->setHandleColor(m_sliderDlg->handleColor());
        slider->setScaleColor(m_sliderDlg->scaleColor());
        slider->setTextColor(m_sliderDlg->textColor());
        slider->setPenWidth(m_sliderDlg->penWidth());
        slider->setSteps(m_sliderDlg->steps());
        slider->setHasTrough(m_sliderDlg->hasTrough());
        slider->setHasGroove(m_sliderDlg->hasGroove());
        slider->setOrientation(m_sliderDlg->orientation());

        m_layout->takeAt(sliderLayIndex);
        m_layout->addWidget(slider, m_sliderDlg->posRow(), m_sliderDlg->posColumn(), m_sliderDlg->sizeRows(), m_sliderDlg->sizeColumns());
    }
}

void CanOpenWin::on_actDelSlider_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_slidersMenu->pos());

    //qDebug() << pos;

    auto slider = findWidgetTypeAt<SDOValueSlider>(pos);

    if(slider == nullptr) return;

    int sliderLayIndex = m_layout->indexOf(slider);
    if(sliderLayIndex == -1) return;

    m_layout->takeAt(sliderLayIndex);

    slider->resetSDOValue();
    delete slider;
}

void CanOpenWin::on_actAddBar_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_barDlg->setName(tr("Прибор %1").arg(m_layout->count() + 1));

    if(m_barDlg->exec()){
        auto bar = new SDOValueBar(m_valsHolder, nullptr);

        bool added = bar->setSDOValue(m_barDlg->nodeId(), m_barDlg->index(), m_barDlg->subIndex(), m_barDlg->type(), m_barDlg->rangeMin(), m_barDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка добавления бара!"), tr("Невозможно добавить бар: \"%1\"").arg(m_barDlg->name()));
            return;
        }

        bar->setName(m_barDlg->name());
        bar->setBarBackColor(m_barDlg->barBackColor());
        bar->setBarColor(m_barDlg->barColor());
        bar->setBarAlarmColor(m_barDlg->barAlarmColor());
        bar->setScaleColor(m_barDlg->scaleColor());
        bar->setTextColor(m_barDlg->textColor());
        bar->setPenWidth(m_barDlg->penWidth());
        bar->setBarWidth(m_barDlg->barWidth());
        bar->setBorderWidth(m_barDlg->borderWidth());
        bar->setAlarmEnabled(m_barDlg->alarmEnabled());
        bar->setAlarmLevel(m_barDlg->alarmLevel());
        bar->setScalePosition(m_barDlg->scalePosition());
        bar->setOrientation(m_barDlg->orientation());

        bar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(bar, &SDOValueBar::customContextMenuRequested, this, &CanOpenWin::showBarsContextMenu);

        m_layout->addWidget(bar, m_barDlg->posRow(), m_barDlg->posColumn(), m_barDlg->sizeRows(), m_barDlg->sizeColumns());
    }
}

void CanOpenWin::on_actEditBar_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_barsMenu->pos());

    //qDebug() << pos;

    auto bar = findWidgetTypeAt<SDOValueBar>(pos);

    //qDebug() << bar;

    if(bar == nullptr) return;

    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;

    int barLayIndex = m_layout->indexOf(bar);
    if(barLayIndex == -1) return;

    m_layout->getItemPosition(barLayIndex, &row, &col, &rowSpan, &colSpan);

    m_barDlg->setPosRow(row);
    m_barDlg->setPosColumn(col);
    m_barDlg->setSizeRows(rowSpan);
    m_barDlg->setSizeColumns(colSpan);

    m_barDlg->setName(bar->name());
    m_barDlg->setBarBackColor(bar->barBackColor());
    m_barDlg->setBarColor(bar->barColor());
    m_barDlg->setBarAlarmColor(bar->barAlarmColor());
    m_barDlg->setScaleColor(bar->scaleColor());
    m_barDlg->setTextColor(bar->textColor());
    m_barDlg->setPenWidth(bar->penWidth());
    m_barDlg->setBarWidth(bar->barWidth());
    m_barDlg->setBorderWidth(bar->borderWidth());
    m_barDlg->setAlarmEnabled(bar->alarmEnabled());
    m_barDlg->setAlarmLevel(bar->alarmLevel());
    m_barDlg->setScalePosition(bar->scalePosition());
    m_barDlg->setOrientation(bar->orientation());

    m_barDlg->setRangeMin(bar->rangeMin());
    m_barDlg->setRangeMax(bar->rangeMax());

    auto sdoval = bar->getSDOValue();

    m_barDlg->setNodeId(sdoval->nodeId());
    m_barDlg->setIndex(sdoval->index());
    m_barDlg->setSubIndex(sdoval->subIndex());
    m_barDlg->setType(bar->SDOValueType());

    if(m_barDlg->exec()){
        bar->resetSDOValue();

        bool added = bar->setSDOValue(m_barDlg->nodeId(), m_barDlg->index(), m_barDlg->subIndex(), m_barDlg->type(), m_barDlg->rangeMin(), m_barDlg->rangeMax());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка изменения бара!"), tr("Невозможно изменить бар: \"%1\"").arg(m_barDlg->name()));
            return;
        }

        bar->setName(m_barDlg->name());
        bar->setBarBackColor(m_barDlg->barBackColor());
        bar->setBarColor(m_barDlg->barColor());
        bar->setBarAlarmColor(m_barDlg->barAlarmColor());
        bar->setScaleColor(m_barDlg->scaleColor());
        bar->setTextColor(m_barDlg->textColor());
        bar->setPenWidth(m_barDlg->penWidth());
        bar->setBarWidth(m_barDlg->barWidth());
        bar->setBorderWidth(m_barDlg->borderWidth());
        bar->setAlarmEnabled(m_barDlg->alarmEnabled());
        bar->setAlarmLevel(m_barDlg->alarmLevel());
        bar->setScalePosition(m_barDlg->scalePosition());
        bar->setOrientation(m_barDlg->orientation());

        m_layout->takeAt(barLayIndex);
        m_layout->addWidget(bar, m_barDlg->posRow(), m_barDlg->posColumn(), m_barDlg->sizeRows(), m_barDlg->sizeColumns());
    }
}

void CanOpenWin::on_actDelBar_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_barsMenu->pos());

    //qDebug() << pos;

    auto bar = findWidgetTypeAt<SDOValueBar>(pos);

    if(bar == nullptr) return;

    int barLayIndex = m_layout->indexOf(bar);
    if(barLayIndex == -1) return;

    m_layout->takeAt(barLayIndex);

    bar->resetSDOValue();
    delete bar;
}

void CanOpenWin::on_actAddButton_triggered(bool checked)
{
    Q_UNUSED(checked)

    m_buttonDlg->setText(tr("Кнопка %1").arg(m_layout->count() + 1));

    if(m_buttonDlg->exec()){
        auto button = new SDOValueButton(m_valsHolder, nullptr);

        bool added = button->setSDOValue(m_buttonDlg->nodeId(), m_buttonDlg->index(), m_buttonDlg->subIndex(), m_buttonDlg->type());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка добавления кнопки!"), tr("Невозможно добавить кнопку: \"%1\"").arg(m_buttonDlg->text()));
            return;
        }

        button->setText(m_buttonDlg->text());
        button->setButtonColor(m_buttonDlg->buttonColor());
        button->setBorderColor(m_buttonDlg->borderColor());
        button->setIndicatorColor(m_buttonDlg->indicatorColor());
        button->setActivateColor(m_buttonDlg->activateColor());
        button->setHighlightColor(m_buttonDlg->highlightColor());
        button->setTextColor(m_buttonDlg->textColor());
        button->setBorderWidth(m_buttonDlg->borderWidth());
        button->setIndicatorEnabled(m_buttonDlg->indicatorEnabled());
        button->setFontPointSize(m_buttonDlg->fontPointSize());
        button->setFontCapitalization(m_buttonDlg->fontCapitalization());
        button->setFontBold(m_buttonDlg->fontBold());
        button->setActivatedValueMask(m_buttonDlg->activatedValueMask());

        button->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(button, &SDOValueButton::customContextMenuRequested, this, &CanOpenWin::showButtonsContextMenu);

        m_layout->addWidget(button, m_buttonDlg->posRow(), m_buttonDlg->posColumn(), m_buttonDlg->sizeRows(), m_buttonDlg->sizeColumns());
    }
}

void CanOpenWin::on_actEditButton_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_buttonsMenu->pos());

    //qDebug() << pos;

    auto button = findWidgetTypeAt<SDOValueButton>(pos);

    //qDebug() << button;

    if(button == nullptr) return;

    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;

    int buttonLayIndex = m_layout->indexOf(button);
    if(buttonLayIndex == -1) return;

    m_layout->getItemPosition(buttonLayIndex, &row, &col, &rowSpan, &colSpan);

    m_buttonDlg->setPosRow(row);
    m_buttonDlg->setPosColumn(col);
    m_buttonDlg->setSizeRows(rowSpan);
    m_buttonDlg->setSizeColumns(colSpan);

    m_buttonDlg->setText(button->text());
    m_buttonDlg->setButtonColor(button->buttonColor());
    m_buttonDlg->setBorderColor(button->borderColor());
    m_buttonDlg->setIndicatorColor(button->indicatorColor());
    m_buttonDlg->setActivateColor(button->activateColor());
    m_buttonDlg->setHighlightColor(button->highlightColor());
    m_buttonDlg->setTextColor(button->textColor());
    m_buttonDlg->setBorderWidth(button->borderWidth());
    m_buttonDlg->setIndicatorEnabled(button->indicatorEnabled());
    m_buttonDlg->setFontPointSize(button->fontPointSize());
    m_buttonDlg->setFontCapitalization(button->fontCapitalization());
    m_buttonDlg->setFontBold(button->fontBold());
    m_buttonDlg->setActivatedValueMask(button->activatedValueMask());

    auto sdoval = button->getSDOValue();

    m_buttonDlg->setNodeId(sdoval->nodeId());
    m_buttonDlg->setIndex(sdoval->index());
    m_buttonDlg->setSubIndex(sdoval->subIndex());
    m_buttonDlg->setType(button->SDOValueType());

    if(m_buttonDlg->exec()){
        button->resetSDOValue();

        bool added = button->setSDOValue(m_buttonDlg->nodeId(), m_buttonDlg->index(), m_buttonDlg->subIndex(), m_buttonDlg->type());
        if(!added){
            QMessageBox::critical(this, tr("Ошибка изменения кнопки!"), tr("Невозможно изменить кнопку: \"%1\"").arg(m_buttonDlg->text()));
            return;
        }

        button->setText(m_buttonDlg->text());
        button->setButtonColor(m_buttonDlg->buttonColor());
        button->setBorderColor(m_buttonDlg->borderColor());
        button->setIndicatorColor(m_buttonDlg->indicatorColor());
        button->setActivateColor(m_buttonDlg->activateColor());
        button->setHighlightColor(m_buttonDlg->highlightColor());
        button->setTextColor(m_buttonDlg->textColor());
        button->setBorderWidth(m_buttonDlg->borderWidth());
        button->setIndicatorEnabled(m_buttonDlg->indicatorEnabled());
        button->setFontPointSize(m_buttonDlg->fontPointSize());
        button->setFontCapitalization(m_buttonDlg->fontCapitalization());
        button->setFontBold(m_buttonDlg->fontBold());
        button->setActivatedValueMask(m_buttonDlg->activatedValueMask());

        m_layout->takeAt(buttonLayIndex);
        m_layout->addWidget(button, m_buttonDlg->posRow(), m_buttonDlg->posColumn(), m_buttonDlg->sizeRows(), m_buttonDlg->sizeColumns());
    }
}

void CanOpenWin::on_actDelButton_triggered(bool checked)
{
    Q_UNUSED(checked)

    const QPoint& pos = centralWidget()->mapFromGlobal(m_buttonsMenu->pos());

    //qDebug() << pos;

    auto button = findWidgetTypeAt<SDOValueButton>(pos);

    if(button == nullptr) return;

    int buttonLayIndex = m_layout->indexOf(button);
    if(buttonLayIndex == -1) return;

    m_layout->takeAt(buttonLayIndex);

    button->resetSDOValue();
    delete button;
}

void CanOpenWin::CANopen_connected()
{
    qDebug() << "CANopen_connected()";
}

void CanOpenWin::CANopen_disconnected()
{
    qDebug() << "CANopen_disconnected()";
}

void CanOpenWin::showPlotContextMenu(const QPoint& pos)
{
    auto plt = qobject_cast<SDOValuePlot*>(sender());
    if(plt == nullptr) return;

    m_plotsMenu->exec(plt->mapToGlobal(pos));
}

void CanOpenWin::showDialsContextMenu(const QPoint& pos)
{
    auto dial = qobject_cast<SDOValueDial*>(sender());
    if(dial == nullptr) return;

    m_dialsMenu->exec(dial->mapToGlobal(pos));
}

void CanOpenWin::showSlidersContextMenu(const QPoint& pos)
{
    auto slider = qobject_cast<SDOValueSlider*>(sender());
    if(slider == nullptr) return;

    m_slidersMenu->exec(slider->mapToGlobal(pos));
}

void CanOpenWin::showBarsContextMenu(const QPoint& pos)
{
    auto bar = qobject_cast<SDOValueBar*>(sender());
    if(bar == nullptr) return;

    m_barsMenu->exec(bar->mapToGlobal(pos));
}

void CanOpenWin::showButtonsContextMenu(const QPoint& pos)
{
    auto btn = qobject_cast<SDOValueButton*>(sender());
    if(btn == nullptr) return;

    m_buttonsMenu->exec(btn->mapToGlobal(pos));
}

