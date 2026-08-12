#include "sdoscopewgt.h"
#include "ui_sdoscopewgt.h"
#include "sdoscopechseditdlg.h"
#include "sdoscopeparamsdlg.h"
#include <QPushButton>
#include "oscopeplot.h"
#include "oscopeplotseriesdata.h"
#include "oscopedata.h"
#include "sdoscope.h"
#include "slcanopennode.h"
#include "reglistmodel.h"
#include "regutils.h"
#include "regtypes.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QDebug>
#include "sdoscopedata.h"
#include <math.h>



SDOScopeWgt::SDOScopeWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SDOScopeWgt)
{
    ui->setupUi(this);
    connect(ui->pbParams, &QPushButton::clicked, this, &SDOScopeWgt::btnScopeParams_clicked);
    connect(ui->pbChannels, &QPushButton::clicked, this, &SDOScopeWgt::btnScopeChannels_clicked);

    ui->asHori->setScaleMin(1e-5);
    ui->asHori->setScaleMax(1e+1);
    ui->asHori->setScaleTurns(2);
    ui->asHori->setUnit(
        tr("с"),
        QStringList(),
        QStringList() << tr("м") << tr("мк")
    );
    connect(ui->asHori, &OScopeAxisWgt::scaleChanged, this, &SDOScopeWgt::horiScaleChanged);
    connect(ui->asHori, &OScopeAxisWgt::offsetChanged, this, &SDOScopeWgt::horiOffsetChanged);

    m_chs_edit_dlg = new SDOScopeChsEditDlg();
    m_params_dlg = new SDOScopeParamsDlg();

    m_initialized = false;

    // Осциллограф.
    m_scope = new SDOScope();
    // Коннекты осциллографа.
    connect(m_scope, &SDOScope::initialized, this, &SDOScopeWgt::sdoscopeInitialized);
    connect(m_scope, &SDOScope::updated, this, &SDOScopeWgt::sdoscopeUpdated);
    connect(m_scope, &SDOScope::errorOccured, this, &SDOScopeWgt::sdoscopeErrorOccured);
    connect(m_scope, &SDOScope::done, this, &SDOScopeWgt::sdoscopeDone);
    connect(m_scope, &SDOScope::readed, this, &SDOScopeWgt::sdoscopeReaded);

    // Коннекты виджета.
    connect(ui->pbSingle, &QPushButton::clicked, this, &SDOScopeWgt::single);

    // Данные осциллографа.
    m_scope_data = new SDOScopeData(m_scope);

    auto plt = getPlot();
    plt->setData(m_scope_data);

    // Создадим интерфейс каналов.
    populateChannelsUi();

    // Обновим интерфейс.
    refreshUi();

    // Синхронизируем график с интерфейсом.
    applyUiToPlot();
}

SDOScopeWgt::~SDOScopeWgt()
{
    auto plt = getPlot();
    plt->clear();

    delete m_params_dlg;
    delete m_chs_edit_dlg;
    delete m_scope_data;
    delete m_scope;
    delete ui;
}

const RegListModel* SDOScopeWgt::regListModel() const
{
    return m_scope->regListModel();
}

RegListModel* SDOScopeWgt::regListModel()
{
    return m_scope->regListModel();
}

void SDOScopeWgt::setRegListModel(RegListModel* newRegListModel)
{
     m_scope->setRegListModel(newRegListModel);
}

SLCanOpenNode* SDOScopeWgt::getSLCanOpenNode()
{
     return m_scope->getSLCanOpenNode();
}

void SDOScopeWgt::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_scope->setSLCanOpenNode(slcon);
}

void SDOScopeWgt::connected()
{
    qDebug() << "SDOScopeWgt::connected()";

    if(!m_scope->init()){
        qDebug() << "SDOScope init failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка инициализации осциллографа!"));
    }
}

void SDOScopeWgt::disconnected()
{
    qDebug() << "SDOScopeWgt::disconnected()";

    m_initialized = false;
}

void SDOScopeWgt::single()
{
    if(!m_initialized) return;

    if(!m_scope->run()){
        qDebug() << "SDOScope run failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка запуска осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeInitialized()
{
    qDebug() << "SDOScopeWgt::sdoscopeInitialized()";

    m_initialized = true;

    if(!m_scope->update()){
        qDebug() << "SDOScope update failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка обновления осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeUpdated()
{
    qDebug() << "SDOScopeWgt::sdoscopeUpdated()";

    // Заполним график каналами осциллографа.

    auto plt = getPlot();

    // Обновим каналы.
    plt->updateData();

    // Заполним интерфейс каналов.
    populateChannelsUi();

    // Обновим интерфейс.
    refreshUi();

    // Синхронизируем график с интерфейсом.
    applyUiToPlot();
}

void SDOScopeWgt::sdoscopeErrorOccured()
{
    qDebug() << "SDOScopeWgt::sdoscopeErrorOccured()" << static_cast<int>(m_scope->error());

    QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка работы осциллографа!\n%1").arg(static_cast<int>(m_scope->error())));
}

void SDOScopeWgt::sdoscopeDone()
{
    if(!m_scope->read()){
        qDebug() << "SDOScope read failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка чтения осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeReaded()
{
    // Обновим график.

    auto plt = getPlot();

    plt->invalidateAllBounds();
    plt->replot();
}

void SDOScopeWgt::horiScaleChanged(double value)
{
    auto plt = getPlot();
    plt->setHDiv(value);
    plt->replot();
}

void SDOScopeWgt::horiOffsetChanged(double value)
{
    auto plt = getPlot();
    plt->setHOffset(value);
    plt->replot();
}

void SDOScopeWgt::btnScopeParams_clicked(bool checked)
{
    Q_UNUSED(checked);

    m_params_dlg->setPrescaler(m_scope->prescaler());
    m_params_dlg->setMaxSamplesCount(m_scope->maxSamplesCount());
    m_params_dlg->setSamplesCount(m_scope->samplesCount());
    m_params_dlg->setHistSamplesCount(m_scope->histSamplesCount());

    if(m_params_dlg->exec()){
        m_scope->setPrescaler(m_params_dlg->prescaler());
        m_scope->setSamplesCount(m_params_dlg->samplesCount());
        m_scope->setHistSamplesCount(m_params_dlg->histSamplesCount());
    }
}

void SDOScopeWgt::btnScopeChannels_clicked(bool checked)
{
    Q_UNUSED(checked);

    auto plt = getPlot();

    {
        SDOScopeChsEditDlg::ChannelsData chs_data;
        chs_data.resize(m_scope->maxChannelsCount());

        for(int i = 0; i < chs_data.size(); i ++){
            SDOScope::Channel* ch = m_scope->channel(i);
            if(ch == nullptr) continue;

            SDOScopeChsEditDlg::ChannelData& ch_data = chs_data[i];
            QString ch_name = plt->signalName(i);
            ch_data.name = ch_name.isEmpty() ? tr("Канал %1").arg(i) : ch_name;
            ch_data.enabled = ch->enabled();
            ch_data.regFullIndex = RegUtils::makeFullIndex(ch->regIndex(), ch->regSubIndex());
            ch_data.dataType = ch->dataType();
            ch_data.baseValue = ch->baseValue();
        }

        m_chs_edit_dlg->setChannelsData(chs_data);
    }


    if(m_chs_edit_dlg->exec()){
        const SDOScopeChsEditDlg::ChannelsData& chs_data = m_chs_edit_dlg->channelsData();

        for(int i = 0; i < chs_data.size(); i ++){
            SDOScope::Channel* ch = m_scope->channel(i);
            if(ch == nullptr) continue;

            const SDOScopeChsEditDlg::ChannelData& ch_data = chs_data[i];

            plt->setSignalName(i, ch_data.name);
            ch->setEnabled(ch_data.enabled);
            ch->setRegIndex(RegUtils::getIndex(ch_data.regFullIndex));
            ch->setRegSubIndex(RegUtils::getSubIndex(ch_data.regFullIndex));
            ch->setDataType(ch_data.dataType);
            ch->setBaseValue(ch_data.baseValue);
        }
    }
}

OScopePlot* SDOScopeWgt::getPlot()
{
    return ui->oscplt;
}

const OScopePlot* SDOScopeWgt::getPlot() const
{
    return ui->oscplt;
}

void SDOScopeWgt::populateChannelsUi()
{
    ui->twChannels->setUpdatesEnabled(false);

    // Clear tabs.
    while(ui->twChannels->count() != 0){
        QWidget* tabWidget = ui->twChannels->widget(0);
        ui->twChannels->removeTab(0);
        if(tabWidget) delete tabWidget;
    }

    auto plt = getPlot();

    // Add new tabs.
    for(uint i = 0; i < m_scope->channelsCount(); i ++){
        OScopeChannelWgt* ocw = new OScopeChannelWgt(ui->twChannels);
        ocw->setPlot(plt);
        ocw->setChannel(i);
        //ocw->updateValues();
        ui->twChannels->addTab(ocw, tr("Канал %1").arg(i + 1));
    }

    ui->twChannels->setUpdatesEnabled(true);
}

void SDOScopeWgt::refreshUi()
{
    refreshChannelsUi();

    auto plt = getPlot();
    ui->asHori->setScale(plt->hDiv());
    ui->asHori->setOffset(plt->hOffset());
}

void SDOScopeWgt::refreshChannelsUi()
{
    ui->twChannels->setUpdatesEnabled(false);

    for(int i = 0; i < ui->twChannels->count(); i ++){
        auto ch = m_scope->channel(i);

        bool ch_enabled = ch && ch->enabled();
        ui->twChannels->setTabVisible(i, ch != nullptr);
        ui->twChannels->setTabEnabled(i, ch_enabled);

        OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
        if(ocw){
            ocw->updateValues();
        }
    }

    ui->twChannels->setUpdatesEnabled(true);
}

void SDOScopeWgt::applyUiToPlot()
{
    auto plt = getPlot();

    plt->setHOffset(ui->asHori->offset());
    plt->setHDiv(ui->asHori->scale());

    for(int i = 0; i < ui->twChannels->count(); i ++){
        OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
        if(ocw){
            ocw->applyValues();
        }
    }

    plt->replot();
}
