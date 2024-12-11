#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include <QSerialPort>
#include "cotypes.h"


namespace Ui {
class SettingsDlg;
}

class SettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDlg(QWidget *parent = nullptr);
    ~SettingsDlg();

    uint updatePeriod() const;
    void setUpdatePeriod(uint newUpdatePeriod);

    QString portName() const;
    void setPortName(const QString& newPortName);

    QSerialPort::BaudRate portBaud() const;
    void setPortBaud(QSerialPort::BaudRate newPortBaud);

    QSerialPort::Parity portParity() const;
    void setPortParity(const QSerialPort::Parity& newPortParity);

    QSerialPort::StopBits portStopBits() const;
    void setPortStopBits(const QSerialPort::StopBits& newPortStopBits);

    bool chinaAdapter() const;
    void setChinaAdapter(bool newChinaAdapter);

    uint canBitrate() const;
    void setCanBitrate(uint newCanBitrate);

    uint processInterval() const;
    void setProcessInterval(uint newProcessInterval);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    CO::NodeId clientNodeId() const;
    void setClientNodeId(CO::NodeId newClientNodeId);

    uint cobidCliToSrv() const;
    void setCobidCliToSrv(uint newCobidCliToSrv);

    uint cobidSrvToCli() const;
    void setCobidSrvToCli(uint newCobidSrvToCli);

    bool useSdoBlockTransfer() const;
    void setUseSdoBlockTransfer(bool newUseSdoBlockTransfer);

    uint clientTimeout() const;
    void setClientTimeout(uint newClientTimeout);

    uint serverTimeout() const;
    void setServerTimeout(uint newServerTimeout);

    uint sdoTimeout() const;
    void setSdoTimeout(uint newSdoTimeout);

    uint hbFirstTime() const;
    void setHbFirstTime(uint newHbFirstTime);

    uint hbPeriod() const;
    void setHbPeriod(uint newHbPeriod);

    QColor windowColor() const;
    void setWindowColor(const QColor& newWindowColor);

private slots:
    void on_tbWindowBackColorSel_clicked(bool checked = false);

private:
    Ui::SettingsDlg *ui;

    void peekColor(QWidget* colHolder);

    void populatePortNames();
    void populatePortBauds();
    void populatePortParitys();
    void populatePortStopBits();
    void populateCanBitrates();
};

#endif // SETTINGSDLG_H
