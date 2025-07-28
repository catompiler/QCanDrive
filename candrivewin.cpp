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


CanDriveWin::CanDriveWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanDriveWin)
{
    ui->setupUi(this);
    ui->twMain->setStyleSheet(
                "QTabWidget::pane { border: 1px solid #222222; }"
                ); // none
    //
    ui->lRegList->insertWidget(0, ui->tbRegsEdit);

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

    ui->tvRegView->setRegListModel(ui->tvRegList->regListModel());

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

    //connect(ui->act, &QAction::triggered, this, &CanDriveWin::m_ui_act_triggered);

    applySettings();
    updateStatusBar();
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

    delete ui;

    delete m_valsHolder;
    delete m_slcon;
}

void CanDriveWin::CANopen_connected()
{
    qDebug() << "CANopen_connected()";
    updateStatusBar();
}

void CanDriveWin::CANopen_disconnected()
{
    qDebug() << "CANopen_disconnected()";
    updateStatusBar();
}

void CanDriveWin::m_ui_actQuit_triggered(bool checked)
{
    Q_UNUSED(checked)

    close();
}

void CanDriveWin::m_ui_actDebugExec_triggered(bool checked)
{
    Q_UNUSED(checked)
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
    //m_settingsDlg->set(m_settings->);

    if(m_settingsDlg->exec()){
        s->general.updatePeriod = m_settingsDlg->updatePeriod();
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

    QPalette pal(palette());
    pal.setColor(QPalette::Window, s->appear.windowColor);
    setPalette(pal);
    centralWidget()->setPalette(pal);
    ui->twMain->setPalette(pal);
    ui->tabCockpit->setPalette(pal);
    ui->cockpitWgt->setPalette(pal);
}

