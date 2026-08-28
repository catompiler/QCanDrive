#include "sdoscopewgt.h"
#include "ui_sdoscopewgt.h"
#include "sdoscopechseditdlg.h"
#include "sdoscopeparamsdlg.h"
#include <QPushButton>
#include "oscopeplot.h"
#include "sdoscopedata.h"
#include "oscopeplotseriesdata.h"
#include "oscopedata.h"
#include "timeformatter.h"
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
#include <math.h>
#include <QTimer>



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
    ui->asHori->setUnitFormatter(new TimeFormatter());
    connect(ui->asHori, &OScopeAxisWgt::scaleChanged, this, &SDOScopeWgt::horiScaleChanged);
    connect(ui->asHori, &OScopeAxisWgt::offsetChanged, this, &SDOScopeWgt::horiOffsetChanged);

    m_chs_edit_dlg = new SDOScopeChsEditDlg();
    m_params_dlg = new SDOScopeParamsDlg();

    m_initialized = false;

    // Осциллограф.
    m_scope = new SDOScope();

    // Данные осциллографа.
    m_scope_data = new SDOScopeData(m_scope);

    auto plt = getPlot();
    plt->setData(m_scope_data);

    // Коннекты осциллографа.
    connect(m_scope, &SDOScope::finished, this, &SDOScopeWgt::sdoscopeFinished);
    connect(m_scope, &SDOScope::initialized, this, &SDOScopeWgt::sdoscopeInitialized);
    connect(m_scope, &SDOScope::updated, this, &SDOScopeWgt::sdoscopeUpdated);
    connect(m_scope, &SDOScope::errorOccured, this, &SDOScopeWgt::sdoscopeErrorOccured);
    connect(m_scope, &SDOScope::done, this, &SDOScopeWgt::sdoscopeDone);
    connect(m_scope, &SDOScope::readed, this, &SDOScopeWgt::sdoscopeReaded);
    connect(m_scope, &SDOScope::applied, this, &SDOScopeWgt::sdoscopeApplied);
    connect(m_scope, &SDOScope::appliedChannels, this, &SDOScopeWgt::sdoscopeAppliedChannels);
    connect(m_scope, &SDOScope::appliedCommon, this, &SDOScopeWgt::sdoscopeAppliedCommon);
    connect(m_scope, &SDOScope::appliedTrig, this, &SDOScopeWgt::sdoscopeAppliedTrig);

    // Таймер отложенного применения значения триггера.
    m_triggerValueChangedTmr = new QTimer();
    m_triggerValueChangedTmr->setInterval(TRIGGER_VALUE_APPLY_DELAY_MS);
    m_triggerValueChangedTmr->setSingleShot(true);
    connect(m_triggerValueChangedTmr, &QTimer::timeout, this, &SDOScopeWgt::triggerValueChangedTmr_timeout);

    // Триггер.
    connect(ui->twTrig, &OScopeTriggerWgt::triggerEnabledChanged, this, &SDOScopeWgt::triggerEnabledChanged);
    connect(ui->twTrig, &OScopeTriggerWgt::triggerTypeChanged, this, &SDOScopeWgt::triggerTypeChanged);
    connect(ui->twTrig, &OScopeTriggerWgt::triggerChannelChanged, this, &SDOScopeWgt::triggerChannelChanged);
    connect(ui->twTrig, &OScopeTriggerWgt::triggerValueChanged, this, &SDOScopeWgt::triggerValueChanged);

    // Коннекты виджета.
    connect(ui->pbRun, &QPushButton::clicked, this, &SDOScopeWgt::btnRun_clicked);
    connect(ui->pbSingle, &QPushButton::clicked, this, &SDOScopeWgt::btnSingle_clicked);
    connect(ui->pbAbort, &QPushButton::clicked, this, &SDOScopeWgt::btnAbort_clicked);
    connect(ui->pbAutoScale, &QPushButton::clicked, this, &SDOScopeWgt::btnAutoScale_clicked);
    connect(ui->cwCursors, &OScopeCursorsWgt::floatingEnabledChanged, this, &SDOScopeWgt::cursorsFloatingEnabledChanged);

    // Создадим интерфейс каналов.
    populateChannelsUi();

    // Заполним список каналов триггера.
    populateTriggerChannels();

    // Обновим интерфейс.
    refreshUi();

    // Синхронизируем график с интерфейсом.
    applyUiToPlot();

    // Обновление доступности элементов UI.
    updateUiEnabled();

    // Автоматический масштаб.
    autoScale();

    // Обновим график.
    // уже в autoScale();
    //plt->replot();
}

SDOScopeWgt::~SDOScopeWgt()
{
    auto plt = getPlot();
    plt->clear();

    delete m_triggerValueChangedTmr;
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

RegSelectDlg* SDOScopeWgt::regSelectDialog() const
{
    return m_chs_edit_dlg->regSelectDialog();
}

void SDOScopeWgt::setRegSelectDialog(RegSelectDlg* newRegSelectDialog)
{
    m_chs_edit_dlg->setRegSelectDialog(newRegSelectDialog);
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
    qDebug() << "SDOScopeWgt::single()";

    if(!m_initialized) return;

    if(!m_scope->run()){
        qDebug() << "SDOScope run failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка запуска осциллографа!"));
    }
}

void SDOScopeWgt::run()
{
    qDebug() << "SDOScopeWgt::run()";

    if(!m_initialized) return;

    if(!m_scope->run()){
        qDebug() << "SDOScope run failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка запуска осциллографа!"));
    }
}

void SDOScopeWgt::stopRun()
{
    qDebug() << "SDOScopeWgt::stopRun()";

    if(!m_initialized) return;

    if(!m_scope->stopRun()){
        qDebug() << "SDOScope stopRun failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка останова осциллографа!"));
    }
}

void SDOScopeWgt::abort()
{
    m_scope->abort();
}

void SDOScopeWgt::autoScale()
{
    auto plt = getPlot();

    int signals_count = plt->signalsCount();

    if(signals_count == 0) return;

    uint samples_count = m_scope->samplesCount();

    if(samples_count == 0) return;

    for(int i = 0; i < ui->twChannels->count(); i ++){
        OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
        if(!ocw) continue;
        if(!ocw->signalVisible()) continue;

        uint ch_n = ocw->channel();

        const SDOScope::Channel* ch = m_scope->channel(ch_n);
        if(!ch) continue;
        if(!ch->enabled()) continue;

        qreal min_val = ch->value(0);
        qreal max_val = min_val;
        for(uint i = 1; i < samples_count; i ++){
            qreal val = ch->value(i);
            min_val = qMin(min_val, val);
            max_val = qMax(max_val, val);
        }

        qreal vdiv = (max_val - min_val) / (OScopePlot::VGRID * 2);
        qreal voffset = (min_val + max_val) / 2;

        ocw->blockSignals(true);
        ocw->setVDiv(vdiv);
        ocw->setVOffset(voffset);
        ocw->blockSignals(false);
    }

    // Масштаб - вся осциллограмма.
    qreal hdiv = m_scope->Ts() * (m_scope->samplesCount()) / (OScopePlot::HGRID * 2);
    // Смещение - вся осциллограмма.
    qreal hoffset = - m_scope->Ts() * (static_cast<int>(m_scope->samplesCount() / 2) - static_cast<int>(m_scope->histSamplesCount()));

    ui->asHori->blockSignals(true);
    ui->asHori->setScale(hdiv);
    ui->asHori->setOffset(hoffset);
    ui->asHori->blockSignals(false);

    applyHoriUiToPlot();
    applyChannelsUiToPlot();

    plt->replot();
}

void SDOScopeWgt::sdoscopeFinished()
{
    qDebug() << "SDOScopeWgt::sdoscopeFinished()";

    // Обновление доступности элементов UI.
    updateUiEnabled();
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

    // Обновим графики каналов.
    plt->updateData();

    // Заполним интерфейс каналов.
    populateChannelsUi();

    // Заполним список каналов триггера.
    populateTriggerChannels();

    // Обновим интерфейс.
    refreshUi();

    // Синхронизируем график с интерфейсом.
    applyUiToPlot();

    // Автоматический масштаб.
    autoScale();

    // Обновим график.
    // уже в autoScale();
    //plt->replot();
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

    // Сбросим кешированные границы.
    plt->invalidateAllBounds();

    // Обновим график.
    plt->replot();

    // Если кнопка SINGLE активна - переведём её в не нажатое состояние.
    if(ui->pbSingle->isChecked()){
        ui->pbSingle->setChecked(false);
    }

    // Если кнопка RUN активна - запустим снова.
    if(ui->pbRun->isChecked()){
        run();
    }
}

void SDOScopeWgt::sdoscopeApplied()
{
    qDebug() << "SDOScopeWgt::sdoscopeApplied()";

    // Очистим каналы.
    //m_scope->clear();

    auto plt = getPlot();

    // Обновим графики каналов.
    plt->refreshData();

    populateChannelsUi();
    populateTriggerChannels();

    refreshUi();
    applyUiToPlot();

    // Обновим график.
    plt->replot();
}

void SDOScopeWgt::sdoscopeAppliedChannels()
{
    qDebug() << "SDOScopeWgt::sdoscopeAppliedChannels()";

    // Очистим каналы.
    //m_scope->clear();

    auto plt = getPlot();

    // Обновим графики каналов.
    plt->refreshData();

    populateChannelsUi();
    populateTriggerChannels();

    refreshChannelsUi();
    refreshTriggerUi();
    applyChannelsUiToPlot();
    applyTriggerUiToPlot();

    // Обновим график.
    plt->replot();
}

void SDOScopeWgt::sdoscopeAppliedCommon()
{
    qDebug() << "SDOScopeWgt::sdoscopeAppliedCommon()";
}

void SDOScopeWgt::sdoscopeAppliedTrig()
{
    qDebug() << "SDOScopeWgt::sdoscopeAppliedTrig()";

    //applyTriggerUiToPlot();
}

void SDOScopeWgt::horiScaleChanged(double value)
{
    auto plt = getPlot();
    plt->setHDiv(value);
    // Обновим график.
    plt->replot();
}

void SDOScopeWgt::horiOffsetChanged(double value)
{
    auto plt = getPlot();
    plt->setHOffset(value);
    // Обновим график.
    plt->replot();
}

void SDOScopeWgt::triggerEnabledChanged(bool newEnabled)
{
    m_scope->setTriggerEnabled(newEnabled);

    if(!m_scope->applyTrig()){
        qDebug() << "SDOScope applyTrig failed";
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();

    auto plt = getPlot();
    plt->setTriggerMarkEnabled(newEnabled);
    plt->replot();
}

void SDOScopeWgt::triggerTypeChanged(int newType)
{
    m_scope->setTriggerType(static_cast<SDOScope::TriggerType>(newType));

    if(!m_scope->applyTrig()){
        qDebug() << "SDOScope applyTrig failed";
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();
}

void SDOScopeWgt::triggerValueChanged(qreal newValue)
{
    if(auto ch = m_scope->channel(ui->twTrig->triggerChannel()); ch != nullptr){

        DataType chDataType = ch->dataType();
        qreal chBaseValue = ch->baseValue();
        // Если установлено 0 - относительные единицы.
        if(chBaseValue == 0.0) chBaseValue = 1.0;

        qreal valPU = newValue / chBaseValue;
        valPU = qBound(RegTypes::iqMinimum(chDataType),
                       valPU,
                       RegTypes::iqMaximum(chDataType));
        //qDebug() << valPU << RegTypes::iqValueToRaw(valPU, chDataType);

        m_scope->setTriggerValue(RegTypes::iqValueToRaw(valPU, chDataType));

        m_triggerValueChangedTmr->start();
    }

    auto plt = getPlot();
    plt->setTriggerMarkValue(newValue);
    plt->replot();
}

void SDOScopeWgt::triggerChannelChanged(uint newChannel)
{
    m_scope->setTriggerChannel(newChannel);

    updateTriggerUiByChannel();

    if(!m_scope->applyTrig()){
        qDebug() << "SDOScope applyTrig failed";
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();

    auto plt = getPlot();
    plt->setTriggerMarkChannel(newChannel);
    plt->replot();
}

void SDOScopeWgt::triggerValueChangedTmr_timeout()
{
    if(!m_scope->applyTrig()){
        qDebug() << "SDOScope applyTrig failed";
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();
}

void SDOScopeWgt::cursorsFloatingEnabledChanged(bool newEnabled)
{
    getPlot()->setCursorsFloatingEnabled(newEnabled);
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

        if(!m_scope->applyCommon()){
            qDebug() << "SDOScope applyCommon failed";

            QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка применения параметров осциллографа!"));
        }

        // Обновление доступности элементов UI.
        updateUiEnabled();
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

            OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
            if(ocw){
                ocw->setSignalName(ch_data.name);
            }else{
                plt->setSignalName(i, ch_data.name);
            }
            ch->setEnabled(ch_data.enabled);
            ch->setRegIndex(RegUtils::getIndex(ch_data.regFullIndex));
            ch->setRegSubIndex(RegUtils::getSubIndex(ch_data.regFullIndex));
            ch->setDataType(ch_data.dataType);
            ch->setBaseValue(ch_data.baseValue);
        }

        if(!m_scope->applyChannels()){
            qDebug() << "SDOScope applyChannels failed";

            QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка настройки каналов осциллографа!"));
        }

        // Обновление доступности элементов UI.
        updateUiEnabled();
    }
}

void SDOScopeWgt::btnRun_clicked(bool checked)
{
    if(checked){
        run();
    }else{
        if(m_scope->isRunning()) stopRun();
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();
}

void SDOScopeWgt::btnSingle_clicked(bool checked)
{
    if(checked){
        single();
    }else{
        if(m_scope->isRunning()) stopRun();
    }

    // Обновление доступности элементов UI.
    updateUiEnabled();
}

void SDOScopeWgt::btnAbort_clicked(bool checked)
{
    Q_UNUSED(checked);

    abort();

    // Обновление доступности элементов UI.
    updateUiEnabled();
}

void SDOScopeWgt::btnAutoScale_clicked(bool checked)
{
    Q_UNUSED(checked);

    autoScale();
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

    OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->currentWidget());
    int curTabChannel = (ocw != nullptr) ? ocw->channel() : -1;

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

    // Restore active channel tab.
    for(int i = 0; i < ui->twChannels->count(); i ++){
        OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
        if(ocw && (ocw->channel() == curTabChannel)){
            ui->twChannels->setCurrentIndex(i);
            break;
        }
    }

    ui->twChannels->setUpdatesEnabled(true);
}

void SDOScopeWgt::populateTriggerChannels()
{
    ui->twTrig->blockSignals(true);

    ui->twTrig->clearTriggerChannels();

    for(uint i = 0; i < m_scope->channelsCount(); i ++){
        if(m_scope->channel(i)->enabled()){
            ui->twTrig->addTriggerChannel(tr("Канал %1").arg(i + 1), i);
        }
    }

    ui->twTrig->blockSignals(false);
}

void SDOScopeWgt::updateUiEnabled()
{
    bool init = m_initialized;
    bool busy = m_scope->isBusy();
    bool trig = m_scope->isApplyingTrig();
    //bool run  = m_scope->isRunning();
    //bool read = m_scope->isReading();
    //bool run_read = run || read;
    bool has_signals = getPlot()->hasVisibleSignals();

    ui->pbRun->setEnabled(init && (!busy || ui->pbRun->isChecked()));
    ui->pbSingle->setEnabled(init && (!busy || ui->pbSingle->isChecked()));
    ui->pbParams->setEnabled(init && !busy);
    ui->pbChannels->setEnabled(init && !busy);
    ui->twTrig->setEnabled(init && !trig);
    ui->pbAutoScale->setEnabled(has_signals);
    ui->pbAbort->setEnabled(busy);
    ui->gbCursors->setEnabled(has_signals);
}

void SDOScopeWgt::refreshUi()
{
    refreshChannelsUi();
    refreshTriggerUi();
    refreshCursorsUi();

    auto plt = getPlot();
    ui->asHori->blockSignals(true);
    ui->asHori->setScale(plt->hDiv());
    ui->asHori->setOffset(plt->hOffset());
    ui->asHori->blockSignals(false);
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

void SDOScopeWgt::refreshTriggerUi()
{
    ui->twTrig->blockSignals(true);

    ui->twTrig->setTriggerEnabled(m_scope->triggerEnabled());
    ui->twTrig->setTriggerType(m_scope->triggerType());
    ui->twTrig->setTriggerChannel(m_scope->triggerChannel());

    updateTriggerUiByChannel();

    ui->twTrig->blockSignals(false);
}

void SDOScopeWgt::refreshCursorsUi()
{
    auto plt = getPlot();

    ui->cwCursors->setFloatingEnabled(plt->cursorsFloatingEnabled());
}

void SDOScopeWgt::updateTriggerUiByChannel()
{
    if(auto ch = m_scope->channel(ui->twTrig->triggerChannel()); ch != nullptr){

        DataType chDataType = ch->dataType();
        qreal chBaseValue = ch->baseValue();
        // Если установлено 0 - относительные единицы.
        if(chBaseValue == 0.0) chBaseValue = 1.0;

        ui->twTrig->setTriggerValueMinimum(chBaseValue * RegTypes::iqMinimum(chDataType));
        ui->twTrig->setTriggerValueMaximum(chBaseValue * RegTypes::iqMaximum(chDataType));
        ui->twTrig->setTriggerValueDecimals(RegTypes::iqDecimals(chDataType));
        ui->twTrig->setTriggerValueSingleStep(RegTypes::iqPrecision(chDataType));
        ui->twTrig->setTriggerValue(chBaseValue * RegTypes::iqValueFromRaw<qreal>(m_scope->triggerValue(), chDataType));
    }
}

void SDOScopeWgt::applyUiToPlot()
{
    applyHoriUiToPlot();

    applyChannelsUiToPlot();

    applyTriggerUiToPlot();

    applyCursorsUiToPlot();
}

void SDOScopeWgt::applyHoriUiToPlot()
{
    auto plt = getPlot();

    plt->setHOffset(ui->asHori->offset());
    plt->setHDiv(ui->asHori->scale());
}

void SDOScopeWgt::applyChannelsUiToPlot()
{
    for(int i = 0; i < ui->twChannels->count(); i ++){
        OScopeChannelWgt* ocw = qobject_cast<OScopeChannelWgt*>(ui->twChannels->widget(i));
        if(ocw){
            ocw->applyValues();
        }
    }
}

void SDOScopeWgt::applyTriggerUiToPlot()
{
    auto plt = getPlot();

    plt->setTriggerMarkEnabled(ui->twTrig->triggerEnabled());
    plt->setTriggerMarkChannel(ui->twTrig->triggerChannel());
    plt->setTriggerMarkValue(ui->twTrig->triggerValue());

    plt->replot();
}

void SDOScopeWgt::applyCursorsUiToPlot()
{
    auto plt = getPlot();

    plt->setCursorsFloatingEnabled(ui->cwCursors->floatingEnabled());
}
