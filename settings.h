#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QSerialPort>


class QSettings;


class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    struct General {
        uint updatePeriod;
    } general;

    struct Connection {
        QString portName;
        QSerialPort::BaudRate portBaud;
        QSerialPort::Parity portParity;
        QSerialPort::StopBits portStopBits;
        bool chinaAdapter;
        uint canBitrate;
        uint processInterval;
    } conn;

    struct CANopen {
        uint nodeId;
        uint clientId;
        uint cobidCliToSrv;
        uint cobidSrvToCli;
        bool useSdoBlockTransfer;
        uint cliTimeout;
        uint srvTimeout;
        uint sdoTimeout;
        uint hbFirstTime;
        uint hbPeriod;
    } co;

    struct Appearance {
        QColor windowColor;
    } appear;

public:
    bool save() const;
    bool load();
signals:

private:
    void saveGeneral(QSettings& s) const;
    void savePort(QSettings& s) const;
    void saveCo(QSettings& s) const;
    void saveAppear(QSettings& s) const;

    void loadGeneral(QSettings& s);
    void loadPort(QSettings& s);
    void loadCo(QSettings& s);
    void loadAppear(QSettings& s);
};

#endif // SETTINGS_H
