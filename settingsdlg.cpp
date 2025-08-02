#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QColorDialog>
#include <QColor>
#include <QSerialPortInfo>



SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);

    setWindowColor(QColor(Qt::darkGray).darker(240));

    populatePortNames();
    populatePortBauds();
    populatePortParitys();
    populatePortStopBits();
    populateCanBitrates();
}

SettingsDlg::~SettingsDlg()
{
    delete ui;
}

uint SettingsDlg::updatePeriod() const
{
    return ui->sbUpdatePeriod->value();
}

void SettingsDlg::setUpdatePeriod(uint newUpdatePeriod)
{
    ui->sbUpdatePeriod->setValue(newUpdatePeriod);
}

uint SettingsDlg::regsRefreshPeriod() const
{
    return ui->sbRegsRefreshPeriod->value();
}

void SettingsDlg::setRegsRefreshPeriod(uint newRegsRefreshPeriod)
{
    ui->sbRegsRefreshPeriod->setValue(newRegsRefreshPeriod);
}

QString SettingsDlg::portName() const
{
    return ui->cbPortName->currentText();
}

void SettingsDlg::setPortName(const QString& newPortName)
{
    ui->cbPortName->setCurrentText(newPortName);
}

QSerialPort::BaudRate SettingsDlg::portBaud() const
{
    bool ok = false;
    auto res =  static_cast<QSerialPort::BaudRate>(ui->cbBaud->currentData().toUInt(&ok));
    if(ok) return res;
    return QSerialPort::UnknownBaud;
}

void SettingsDlg::setPortBaud(QSerialPort::BaudRate newPortBaud)
{
    ui->cbBaud->setCurrentIndex(ui->cbBaud->findData(static_cast<uint>(newPortBaud)));
}

QSerialPort::Parity SettingsDlg::portParity() const
{
    bool ok = false;
    auto res =  static_cast<QSerialPort::Parity>(ui->cbParity->currentData().toUInt(&ok));
    if(ok) return res;
    return QSerialPort::UnknownParity;
}

void SettingsDlg::setPortParity(const QSerialPort::Parity& newPortParity)
{
    ui->cbParity->setCurrentIndex(ui->cbParity->findData(static_cast<uint>(newPortParity)));
}

QSerialPort::StopBits SettingsDlg::portStopBits() const
{
    bool ok = false;
    auto res =  static_cast<QSerialPort::StopBits>(ui->cbStopBits->currentData().toUInt(&ok));
    if(ok) return res;
    return QSerialPort::UnknownStopBits;
}

void SettingsDlg::setPortStopBits(const QSerialPort::StopBits& newPortStopBits)
{
    ui->cbStopBits->setCurrentIndex(ui->cbStopBits->findData(static_cast<uint>(newPortStopBits)));
}

bool SettingsDlg::chinaAdapter() const
{
    return ui->cbChinaAdapter->isChecked();
}

void SettingsDlg::setChinaAdapter(bool newChinaAdapter)
{
    ui->cbChinaAdapter->setChecked(newChinaAdapter);
}

uint SettingsDlg::canBitrate() const
{
    bool ok = false;
    auto res =  static_cast<uint>(ui->cbCanBitrate->currentData().toUInt(&ok));
    if(ok) return res;
    return 0;
}

void SettingsDlg::setCanBitrate(uint newCanBitrate)
{
    ui->cbCanBitrate->setCurrentIndex(ui->cbCanBitrate->findData(newCanBitrate));
}

uint SettingsDlg::processInterval() const
{
    return ui->sbProcessInterval->value();
}

void SettingsDlg::setProcessInterval(uint newProcessInterval)
{
    ui->sbProcessInterval->setValue(newProcessInterval);
}

CO::NodeId SettingsDlg::nodeId() const
{
    return ui->sbDevNodeId->value();
}

void SettingsDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbDevNodeId->setValue(newNodeId);
}

CO::NodeId SettingsDlg::clientNodeId() const
{
    return ui->sbCliNodeId->value();
}

void SettingsDlg::setClientNodeId(CO::NodeId newClientNodeId)
{
    ui->sbCliNodeId->setValue(newClientNodeId);
}

uint SettingsDlg::cobidCliToSrv() const
{
    return ui->sbCobIdCliToSrv->value();
}

void SettingsDlg::setCobidCliToSrv(uint newCobidCliToSrv)
{
    ui->sbCobIdCliToSrv->setValue(newCobidCliToSrv);
}

uint SettingsDlg::cobidSrvToCli() const
{
    return ui->sbCobIdSrvToCli->value();
}

void SettingsDlg::setCobidSrvToCli(uint newCobidSrvToCli)
{
    ui->sbCobIdSrvToCli->setValue(newCobidSrvToCli);
}

bool SettingsDlg::useSdoBlockTransfer() const
{
    return ui->cbSdoBlockTransfer->isChecked();
}

void SettingsDlg::setUseSdoBlockTransfer(bool newUseSdoBlockTransfer)
{
    ui->cbSdoBlockTransfer->setChecked(newUseSdoBlockTransfer);
}

uint SettingsDlg::clientTimeout() const
{
    return ui->sbCliTimeout->value();
}

void SettingsDlg::setClientTimeout(uint newClientTimeout)
{
    ui->sbCliTimeout->setValue(newClientTimeout);
}

uint SettingsDlg::serverTimeout() const
{
    return ui->sbSrvTimeout->value();
}

void SettingsDlg::setServerTimeout(uint newServerTimeout)
{
    ui->sbSrvTimeout->setValue(newServerTimeout);
}

uint SettingsDlg::sdoTimeout() const
{
    return ui->sbSdoTimeout->value();
}

void SettingsDlg::setSdoTimeout(uint newSdoTimeout)
{
    ui->sbSdoTimeout->setValue(newSdoTimeout);
}

uint SettingsDlg::hbFirstTime() const
{
    return ui->sbHBTimeFirst->value();
}

void SettingsDlg::setHbFirstTime(uint newHbFirstTime)
{
    ui->sbHBTimeFirst->setValue(newHbFirstTime);
}

uint SettingsDlg::hbPeriod() const
{
    return ui->sbHBTime->value();
}

void SettingsDlg::setHbPeriod(uint newHbPeriod)
{
    ui->sbHBTime->setValue(newHbPeriod);
}

QColor SettingsDlg::windowColor() const
{
    const QPalette& pal = ui->frWindowBackColor->palette();
    return pal.window().color();
}

void SettingsDlg::setWindowColor(const QColor& newWindowColor)
{
    QPalette pal = ui->frWindowBackColor->palette();
    pal.setColor(QPalette::Window, newWindowColor);
    ui->frWindowBackColor->setPalette(pal);
}

void SettingsDlg::on_tbWindowBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frWindowBackColor);
}

void SettingsDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SettingsDlg::populatePortNames()
{
    ui->cbPortName->clear();

    auto ports = QSerialPortInfo::availablePorts();
    for(auto& p: ports){
        ui->cbPortName->addItem(p.portName());
    }
}

void SettingsDlg::populatePortBauds()
{
    auto bauds = QSerialPortInfo::standardBaudRates();

    ui->cbBaud->clear();

    for(auto& baud: bauds){
        ui->cbBaud->addItem(tr("%1").arg(baud), baud);
    }
}

void SettingsDlg::populatePortParitys()
{
    ui->cbParity->clear();

    ui->cbParity->addItem(tr("Нет"), static_cast<uint>(QSerialPort::NoParity));
    ui->cbParity->addItem(tr("Чётный"), static_cast<uint>(QSerialPort::EvenParity));
    ui->cbParity->addItem(tr("Нечётный"), static_cast<uint>(QSerialPort::OddParity));
}

void SettingsDlg::populatePortStopBits()
{
    ui->cbStopBits->clear();

    ui->cbStopBits->addItem(tr("1"), static_cast<uint>(QSerialPort::OneStop));
    ui->cbStopBits->addItem(tr("2"), static_cast<uint>(QSerialPort::TwoStop));
}

void SettingsDlg::populateCanBitrates()
{
    static const uint bitrates[] = {
        10, 20, 50, 100, 125, 250, 500, 800, 1000
    };

    ui->cbCanBitrate->clear();

    for(uint bitrate: bitrates){
        ui->cbCanBitrate->addItem(tr("%1 кбит/с").arg(bitrate), bitrate);
    }
}
