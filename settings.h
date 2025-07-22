#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QSerialPort>
#include "cotypes.h"
#include "covaluetypes.h"


class QSettings;


class Settings : public QObject
{
    Q_OBJECT
public:
    // TODO: make ctor private
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

    struct SDOValuePlot {
        uint samplesCount;
        QString plotName;
        QColor backColor;
        QColor textColor;
        int transparency;
        int posRow;
        int posColumn;
        int sizeRows;
        int sizeColumns;
        int signalsCount;
    };

    struct SDOValuePlotCurve {
        QString name;
        CO::NodeId nodeId;
        CO::Index index;
        CO::SubIndex subIndex;
        COValue::Type type;
        QColor penColor;
        Qt::PenStyle penStyle;
        qreal penWidth;
        QColor brushColor;
        Qt::BrushStyle brushStyle;
    };

    struct _S_Export{
        // general export.
        QString path;
        QString dataName;
        QString odName;
        QString edsVendorName;
        QString edsProductName;
        //QString edsFileDescr;
        QString edsFileAuthor;
        //QString edsVendorName;
        //QString edsProductName;
        // reglistexporter.
        QString reglistFileName;
        QString regIdsFileName;
        QString reglistUserCode;
        QString regIdsUserCode;
        //bool regIdsExport;
        bool reglistExport;
        // regdata exporter.
        QString regdataDeclFileName;
        QString regdataImplFileName;
        QString regdataDeclUserCode;
        QString regdataImplUserCode;
        bool regDataExport;
        // co exporter.
        QString cohFileName;
        QString cocFileName;
        QString cohUserCode;
        QString cocUserCode;
        bool coExport;
        // eds exporter.
        QString edsFileName;
        bool edsExport;
    } exporting;

    static Settings* get();

    bool save() const;
    bool load();

signals:

private:
    //explicit Settings(QObject *parent = nullptr);

    void saveGeneral(QSettings& s) const;
    void savePort(QSettings& s) const;
    void saveCo(QSettings& s) const;
    void saveAppear(QSettings& s) const;

    void loadGeneral(QSettings& s);
    void loadPort(QSettings& s);
    void loadCo(QSettings& s);
    void loadAppear(QSettings& s);

    void readExporting(QSettings& settings);
    void writeExporting(QSettings& settings) const;
};

#endif // SETTINGS_H
