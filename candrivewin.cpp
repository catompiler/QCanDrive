#include "candrivewin.h"
#include "ui_candrivewin.h"
#include "settings.h"
#include "settingsdlg.h"
#include "slcanopennode.h"
#include "covaluesholder.h"
#include "covaluetypes.h"
#include "cockpitwgt.h"
#include "reglisteditorwgt.h"
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


CanDriveWin::CanDriveWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanDriveWin)
{
    ui->setupUi(this);
    /*
    ui->twMain->setStyleSheet(
        // Панель QTabWidget
        "QTabWidget::pane { border: 1px solid #222222; }"
        // Прозрачный фон панелей QToolBox
        // https://forum.qt.io/topic/62760/background-color-of-qtoolbox-pages-inside-a-qtabwidget-in-windows-7/12
        "QToolBox,"
        "QToolBox > QScrollArea,"
        //"QToolBox > QScrollArea > #qt_scrollarea_viewport,"
        "QToolBox > QScrollArea > #qt_scrollarea_viewport > QWidget { background: transparent; }"
        );
    */
    //
    ui->lRegList->insertWidget(0, ui->tbRegsEdit);
    ui->lRegView->insertWidget(0, ui->tbRegsView);

    // populate statusbar.
    m_sblblConStatus = new QLabel();
    statusBar()->addPermanentWidget(m_sblblConStatus);

    m_settingsDlg = new SettingsDlg();

    Settings::get()->load();

    m_slcon = new SLCanOpenNode(nullptr);
    connect(m_slcon, &SLCanOpenNode::connected, this, &CanDriveWin::CANopen_connected);
    connect(m_slcon, &SLCanOpenNode::disconnected, this, &CanDriveWin::CANopen_disconnected);

    m_valsHolder = new CoValuesHolder(m_slcon, nullptr);
    connect(m_slcon, &SLCanOpenNode::connected, m_valsHolder, &CoValuesHolder::enableUpdating);
    connect(m_slcon, &SLCanOpenNode::disconnected, m_valsHolder, &CoValuesHolder::disableUpdating);

    ui->cockpitWgt->setValsHolder(m_valsHolder);
    ui->cockpitWgt->setRegSelectDialog(ui->tvRegList->regSelectDialog());

    ui->tvRegView->setSLCanOpenNode(m_slcon);
    ui->tvRegView->setRegListModel(ui->tvRegList->regListModel());

    ui->oscopeWgt->setSLCanOpenNode(m_slcon);
    ui->oscopeWgt->setRegListModel(ui->tvRegList->regListModel());
    ui->oscopeWgt->setRegSelectDialog(ui->tvRegList->regSelectDialog());

    // Коннекты действий.
    connect(ui->actDebugExec, &QAction::triggered, this, &CanDriveWin::m_ui_actDebugExec_triggered);
    connect(ui->actQuit, &QAction::triggered, this, &CanDriveWin::m_ui_actQuit_triggered);
    connect(ui->actSettings, &QAction::triggered, this, &CanDriveWin::m_ui_actSettings_triggered);
    connect(ui->actConnect, &QAction::triggered, this, &CanDriveWin::m_ui_actConnect_triggered);
    connect(ui->actDisconnect, &QAction::triggered, this, &CanDriveWin::m_ui_actDisconnect_triggered);

    connect(ui->actOpenCockpit, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::openCockpit);
    connect(ui->actSaveCockpit, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::saveCockpit);
    connect(ui->actClearCockpit, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::clearCockpit);

    connect(ui->actAddPlot, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addPlot);
    connect(ui->actAddDial, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addDial);
    connect(ui->actAddSlider, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addSlider);
    connect(ui->actAddBar, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addBar);
    connect(ui->actAddButton, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addButton);
    connect(ui->actAddIndicator, &QAction::triggered, ui->cockpitWgt, &CockpitWgt::addIndicator);


    connect(ui->actRegListOpen, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::openRegList);
    connect(ui->actRegListOpenAppend, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::appendRegList);
    connect(ui->actRegListSaveAs, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::saveAsRegList);
    connect(ui->actRegListExport, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::exportRegList);
    connect(ui->actRegListDelAll, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::delAll);
    connect(ui->actRegListAddItem, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::addItem);
    connect(ui->actRegListAddSubItem, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::addSubItem);
    connect(ui->actRegListDuplicateItem, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::duplicateItem);
    connect(ui->actRegListDelItem, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::delItem);
    connect(ui->actRegListMoveUp, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::moveUp);
    connect(ui->actRegListMoveDown, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::moveDown);
    connect(ui->actRegListExpandTree, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::expandTree);
    connect(ui->actRegListCollapseTree, &QAction::triggered, ui->tvRegList, &RegListEditorWgt::collapseTree);

    connect(ui->actRegsViewRefresh, &QAction::triggered, ui->tvRegView, &RegsViewWgt::refreshRegs);
    connect(ui->actRegsViewAutoRefresh, &QAction::toggled, ui->tvRegView, &RegsViewWgt::setRefreshingRegs);
    connect(ui->actRegsViewExpandTree, &QAction::triggered, ui->tvRegView, &RegsViewWgt::expandTree);
    connect(ui->actRegsViewCollapseTree, &QAction::triggered, ui->tvRegView, &RegsViewWgt::collapseTree);

    //connect(ui->act, &QAction::triggered, this, &CanDriveWin::m_ui_act_triggered);

    applySettings();
    updateStatusBar();

    Settings* s = Settings::get();
    if(!s->general.cockpitFile.isEmpty()){
        ui->cockpitWgt->openCockpitFile(s->general.cockpitFile);
    }
    if(!s->general.reglistFile.isEmpty()){
        ui->tvRegList->openRegListFile(s->general.reglistFile);
    }
}

CanDriveWin::~CanDriveWin()
{
    m_slcon->destroyCO();
    m_slcon->closePort();

    Settings::get()->save();

    delete m_settingsDlg;

    // statusbar widgets.
    statusBar()->removeWidget(m_sblblConStatus);
    delete m_sblblConStatus;

    ui->cockpitWgt->clearCockpit();
    delete ui->cockpitWgt;
    delete ui->tvRegView;
    delete ui->tvRegList;

    delete ui;

    delete m_valsHolder;
    delete m_slcon;
}

void CanDriveWin::CANopen_connected()
{
    qDebug() << "CANopen_connected()";

    ui->oscopeWgt->connected();

    updateStatusBar();
}

void CanDriveWin::CANopen_disconnected()
{
    qDebug() << "CANopen_disconnected()";

    ui->oscopeWgt->disconnected();

    updateStatusBar();
}

void CanDriveWin::m_ui_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked)

    close();
}

#include "sdoscope.h"

void CanDriveWin::m_ui_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)

    /*static*/ SDOScope* scope = nullptr;

    //if(scope == nullptr){

        scope = new SDOScope();

        scope->setRegListModel(ui->tvRegList->regListModel());
        scope->setSLCanOpenNode(m_slcon);
        //scope->setNodeId(Settings::get()->co.nodeId);

        // connect(scope, &SDOScope::finished, [scope](){
        //     qDebug() << "SDOScope finished";
        // });

        connect(scope, &SDOScope::errorOccured, [scope](){
            qDebug() << "SDOScope error" << (int)scope->error();
            scope->deleteLater();
        });

        connect(scope, &SDOScope::initialized, [scope](){
            qDebug() << scope->version() << scope->maxChannelsCount() << scope->maxSamplesCount() << scope->maxSampleRate();

            if(!scope->update()){
                qDebug() << "SDOScope update failed";
                scope->deleteLater();
            }
        });

        connect(scope, &SDOScope::updated, [scope](){
            qDebug() << "SDOScope updated";

            for(uint i = 0; i < scope->channelsCount(); i++){
                const SDOScope::Channel* ch = scope->channel(i);
                if(ch == nullptr){
                    qDebug() << "Channel" << i << "is NULL";
                    continue;
                }

                qDebug() << "Channel:" << i
                         << "enabled:" << ch->enabled()
                         << "regIndex:" << ch->regIndex()
                         << "regSubIndex:" << ch->regSubIndex()
                         << "dataType:" << (uint)ch->dataType()
                         << "baseValue:" << ch->baseValue()
                         << "samplesCount:" << ch->samplesCount();
            }

            SDOScope::Channel* ch;
            // // Фаза A.
            // ch = scope->channel(3);
            // ch->setRegIndex(0x2070);
            // ch->setRegSubIndex(0x0a);
            // ch->setEnabled(true);
            // // Фаза B.
            // ch = scope->channel(4);
            // ch->setRegIndex(0x2070);
            // ch->setRegSubIndex(0x0b);
            // ch->setEnabled(true);
            // // Фаза C.
            // ch = scope->channel(5);
            // ch->setRegIndex(0x2070);
            // ch->setRegSubIndex(0x0c);
            // ch->setEnabled(true);

            // Udc.
            ch = scope->channel(3);
            ch->setRegIndex(0x2700);
            ch->setRegSubIndex(26);
            ch->setEnabled(true);
            // Idc.
            ch = scope->channel(4);
            ch->setRegIndex(0x2700);
            ch->setRegSubIndex(27);
            ch->setEnabled(true);

            if(!scope->apply()){
                qDebug() << "SDOScope apply failed";
                scope->deleteLater();
            }
        });

        connect(scope, &SDOScope::applied, [scope](){
            qDebug() << "SDOScope applied";

            if(!scope->run()){
                qDebug() << "SDOScope run failed";
                scope->deleteLater();
            }
        });

        connect(scope, &SDOScope::done, [scope](){
            qDebug() << "SDOScope run done";

            if(!scope->read()){
                qDebug() << "SDOScope read failed";
                scope->deleteLater();
            }
        });

        connect(scope, &SDOScope::readed, [scope, this](){
            qDebug() << "SDOScope readed";

            QString fileName = QFileDialog::getSaveFileName(this, tr("Save scope data"), "", tr("CSV files (*.csv);;All files (*.*)"));

            if(fileName.isEmpty()){
                qDebug() << "No file selected, not saving data";
                scope->deleteLater();
                return;
            }

            QFile outFile(fileName);
            if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
                qDebug() << "Failed to open file for writing:" << outFile.errorString();
                scope->deleteLater();
                return;
            }

            QTextStream out(&outFile);

            const SDOScope::Channel* ch = nullptr;

            for(uint j = 0; j < scope->maxSamplesCount(); j++){
                for(uint i = 0; i < scope->channelsCount(); i ++){
                    ch = scope->channel(i);
                    if(ch == nullptr) continue;
                    if(!ch->enabled()) continue;

                    if(i != 0) out << ",";
                    out << ch->value(j);
                }
                out << Qt::endl;
            }

            QMessageBox::information(this, tr("Success"), tr("Scope data saved successfully!"));

            scope->deleteLater();
        });

        if(!scope->init()){
            qDebug() << "SDOScope init failed";
            scope->deleteLater();
        }
    //}

}

void CanDriveWin::m_ui_actConnect_triggered(bool checked)
{
    Q_UNUSED(checked)

    Settings *s = Settings::get();

    if(m_slcon->isConnected()){
        qDebug() << "Already connected!";
        return;
    }

    bool is_open = m_slcon->openPort(s->conn.portName, s->conn.portBaud, s->conn.portParity, s->conn.portStopBits);
    if(is_open){
        qDebug() << "Port opened!";

        bool co_created = m_slcon->createCO(s->conn.canBitrate);
        if(co_created){
            qDebug() << "Connected!";
        }else{
            qDebug() << "CO Fail :(";

            QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка инициализации CANopen!"));

            m_slcon->closePort();
        }
    }else{
        qDebug() << "Fail :(";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка открытия порта!"));
    }

    updateStatusBar();
}

void CanDriveWin::m_ui_actDisconnect_triggered(bool checked)
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

void CanDriveWin::m_ui_actSettings_triggered(bool checked)
{
    Q_UNUSED(checked)

    Settings *s = Settings::get();

    m_settingsDlg->setUpdatePeriod(s->general.updatePeriod);
    m_settingsDlg->setRegsRefreshPeriod(s->general.regsRefreshPeriod);
    m_settingsDlg->setCockpitFile(s->general.cockpitFile);
    m_settingsDlg->setReglistFile(s->general.reglistFile);
    m_settingsDlg->setPortName(s->conn.portName);
    m_settingsDlg->setPortBaud(s->conn.portBaud);
    m_settingsDlg->setPortParity(s->conn.portParity);
    m_settingsDlg->setPortStopBits(s->conn.portStopBits);
    m_settingsDlg->setChinaAdapter(s->conn.chinaAdapter);
    m_settingsDlg->setCanBitrate(s->conn.canBitrate);
    m_settingsDlg->setProcessInterval(s->conn.processInterval);
    m_settingsDlg->setNodeId(s->co.nodeId);
    m_settingsDlg->setClientNodeId(s->co.clientId);
    m_settingsDlg->setCobidCliToSrv(s->co.cobidCliToSrv);
    m_settingsDlg->setCobidSrvToCli(s->co.cobidSrvToCli);
    m_settingsDlg->setUseSdoBlockTransfer(s->co.useSdoBlockTransfer);
    m_settingsDlg->setClientTimeout(s->co.cliTimeout);
    m_settingsDlg->setServerTimeout(s->co.srvTimeout);
    m_settingsDlg->setSdoTimeout(s->co.sdoTimeout);
    m_settingsDlg->setHbFirstTime(s->co.hbFirstTime);
    m_settingsDlg->setHbPeriod(s->co.hbPeriod);
    m_settingsDlg->setWindowColor(s->appear.windowColor);
    m_settingsDlg->setOscopeEntryIndex(s->oscope.index);
    m_settingsDlg->setOscopeVersionSubIndex(s->oscope.subIndex);
    //m_settingsDlg->set(m_settings->);

    if(m_settingsDlg->exec()){
        s->general.updatePeriod = m_settingsDlg->updatePeriod();
        s->general.regsRefreshPeriod = m_settingsDlg->regsRefreshPeriod();
        s->general.cockpitFile = m_settingsDlg->cockpitFile();
        s->general.reglistFile = m_settingsDlg->reglistFile();
        s->conn.portName = m_settingsDlg->portName();
        s->conn.portBaud = m_settingsDlg->portBaud();
        s->conn.portParity = m_settingsDlg->portParity();
        s->conn.portStopBits = m_settingsDlg->portStopBits();
        s->conn.chinaAdapter = m_settingsDlg->chinaAdapter();
        s->conn.canBitrate = m_settingsDlg->canBitrate();
        s->conn.processInterval = m_settingsDlg->processInterval();
        s->co.nodeId = m_settingsDlg->nodeId();
        s->co.clientId = m_settingsDlg->clientNodeId();
        s->co.cobidCliToSrv = m_settingsDlg->cobidCliToSrv();
        s->co.cobidSrvToCli = m_settingsDlg->cobidSrvToCli();
        s->co.useSdoBlockTransfer = m_settingsDlg->useSdoBlockTransfer();
        s->co.cliTimeout = m_settingsDlg->clientTimeout();
        s->co.srvTimeout = m_settingsDlg->serverTimeout();
        s->co.sdoTimeout = m_settingsDlg->sdoTimeout();
        s->co.hbFirstTime = m_settingsDlg->hbFirstTime();
        s->co.hbPeriod = m_settingsDlg->hbPeriod();
        s->appear.windowColor = m_settingsDlg->windowColor();
        s->oscope.index = m_settingsDlg->oscopeEntryIndex();
        s->oscope.subIndex = m_settingsDlg->oscopeVersionSubIndex();

        applySettings();
    }
}

void CanDriveWin::updateStatusBar()
{
    if(m_slcon->isConnected()){
        m_sblblConStatus->setPixmap(QPixmap(":/images/icons/Connected-16.png"));
    }else{
        m_sblblConStatus->setPixmap(QPixmap(":/images/icons/Disconnected-16.png"));
    }
}

void CanDriveWin::applySettings()
{
    Settings *s = Settings::get();

    m_valsHolder->setUpdateInterval(s->general.updatePeriod);
    m_slcon->setAdapterNoAnswers(s->conn.chinaAdapter);
    m_slcon->setCoTimerInterval(s->conn.processInterval);
    m_slcon->setFirstHBTime(s->co.hbFirstTime);
    m_slcon->setHeartbeatTime(s->co.hbPeriod);
    m_slcon->setSDOserverTimeout(s->co.srvTimeout);
    m_slcon->setSDOclientTimeout(s->co.cliTimeout);
    m_slcon->setDefaultTimeout(s->co.sdoTimeout);
    m_slcon->setCobidClientToServer(s->co.cobidCliToSrv);
    m_slcon->setCobidServerToClient(s->co.cobidSrvToCli);
    m_slcon->setSDOclientBlockTransfer(s->co.useSdoBlockTransfer);
    m_slcon->setNodeId(s->co.clientId);
    //m_->set(m_settings->);

    ui->tvRegView->setNodeId(s->co.nodeId);
    ui->tvRegView->setRegsRefreshPeriod(s->general.regsRefreshPeriod);

    QPalette pal(palette());
    pal.setColor(QPalette::Window, s->appear.windowColor);
    setPalette(pal);
    centralWidget()->setPalette(pal);
    ui->twMain->setPalette(pal);
    ui->tabCockpit->setPalette(pal);
    ui->cockpitWgt->setPalette(pal);

    ui->oscopeWgt->setEntryIndex(s->oscope.index);
    ui->oscopeWgt->setVersionSubIndex(s->oscope.subIndex);
}

