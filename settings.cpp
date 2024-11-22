#include "settings.h"
#include <QSettings>
#include <QDir>


Settings::Settings(QObject *parent)
    : QObject{parent}
{
}

Settings::~Settings()
{
}

bool Settings::save() const
{
    QSettings s("settings.ini", QSettings::IniFormat);

    if(!s.isWritable()) return false;

    saveGeneral(s);
    savePort(s);
    saveCo(s);
    saveAppear(s);
    writeExporting(s);

    return s.status() == QSettings::NoError;
}

bool Settings::load()
{
    QSettings s("settings.ini", QSettings::IniFormat);

    loadGeneral(s);
    loadPort(s);
    loadCo(s);
    loadAppear(s);
    readExporting(s);

    return s.status() == QSettings::NoError;
}

void Settings::saveGeneral(QSettings& s) const
{
    s.beginGroup("common");

    s.setValue("updatePeriod", general.updatePeriod);

    s.endGroup();
}

void Settings::savePort(QSettings& s) const
{
    s.beginGroup("conn");

    s.setValue("portName",     conn.portName);
    s.setValue("baud",         static_cast<uint>(conn.portBaud));
    s.setValue("parity",       static_cast<uint>(conn.portParity));
    s.setValue("stopBits",     static_cast<uint>(conn.portStopBits));
    s.setValue("chinaAdapter", conn.chinaAdapter);
    s.setValue("canBitrate",   conn.canBitrate);
    s.setValue("processInterval",   conn.processInterval);

    s.endGroup();
}

void Settings::saveCo(QSettings& s) const
{
    s.beginGroup("co");

    s.setValue("nodeId",              co.nodeId);
    s.setValue("clientId",            co.clientId);
    s.setValue("cobidCliToSrv",       co.cobidCliToSrv);
    s.setValue("cobidSrvToCli",       co.cobidSrvToCli);
    s.setValue("useSdoBlockTransfer", co.useSdoBlockTransfer);
    s.setValue("cliTimeout",          co.cliTimeout);
    s.setValue("srvTimeout",          co.srvTimeout);
    s.setValue("sdoTimeout",          co.sdoTimeout);
    s.setValue("hbFirstTime",         co.hbFirstTime);
    s.setValue("hbPeriod",            co.hbPeriod);

    s.endGroup();
}

void Settings::saveAppear(QSettings& s) const
{
    s.beginGroup("appear");

    s.setValue("windowColor", appear.windowColor.rgb());

    s.endGroup();
}

void Settings::loadGeneral(QSettings& s)
{
    s.beginGroup("common");

    general.updatePeriod = s.value("updatePeriod", 0).toUInt();

    s.endGroup();
}

void Settings::loadPort(QSettings& s)
{
    s.beginGroup("conn");

    conn.portName     = s.value("portName", "").toString();
    conn.portBaud     = static_cast<QSerialPort::BaudRate>(s.value("baud", 115200).toUInt());
    conn.portParity   = static_cast<QSerialPort::Parity>(s.value("parity", 0).toUInt());
    conn.portStopBits = static_cast<QSerialPort::StopBits>(s.value("stopBits", 1).toUInt());
    conn.chinaAdapter = s.value("chinaAdapter", true).toBool();
    conn.canBitrate   = s.value("canBitrate", 125000).toUInt();
    conn.processInterval   = s.value("processInterval", 0).toUInt();

    s.endGroup();
}

void Settings::loadCo(QSettings& s)
{
    s.beginGroup("co");

    co.nodeId =              s.value("nodeId",              1).toUInt();
    co.clientId =            s.value("clientId",            127).toUInt();
    co.cobidCliToSrv =       s.value("cobidCliToSrv",       0x600).toUInt();
    co.cobidSrvToCli =       s.value("cobidSrvToCli",       0x580).toUInt();
    co.useSdoBlockTransfer = s.value("useSdoBlockTransfer", true).toBool();
    co.cliTimeout =          s.value("cliTimeout",          500).toUInt();
    co.srvTimeout =          s.value("srvTimeout",          500).toUInt();
    co.sdoTimeout =          s.value("sdoTimeout",          1000).toUInt();
    co.hbFirstTime =         s.value("hbFirstTime",         0).toUInt();
    co.hbPeriod =            s.value("hbPeriod",            0).toUInt();

    s.endGroup();
}

void Settings::loadAppear(QSettings& s)
{
    s.beginGroup("appear");

    appear.windowColor = QColor::fromRgb(s.value("windowColor", 0x333333).toUInt());

    s.endGroup();
}

void Settings::readExporting(QSettings& settings)
{
    auto& v = exporting;
    auto& s = settings;

    s.beginGroup("export");

    v.path = s.value("path", QDir::currentPath()).toString();
    v.dataName = s.value("dataName", "regs_data").toString();
    v.odName = s.value("odName", "OD").toString();
    v.edsVendorName = s.value("edsVendorName", "").toString();
    v.edsProductName = s.value("edsProductName", "").toString();
    //v.edsFileDescr = s.value("edsFileDescr", "").toString();
    v.edsFileAuthor = s.value("edsFileAuthor", "").toString();
    //v.edsVendorName = s.value("edsVendorName", "").toString();
    //v.edsProductName = s.value("edsProductName", "").toString();
    v.reglistFileName = s.value("reglistFileName", "reg_list_data.h").toString();
    v.regIdsFileName = s.value("regIdsFileName", "reg_ids.h").toString();
    v.reglistUserCode = s.value("reglistUserCode", "").toString();
    v.regIdsUserCode = s.value("regIdsUserCode", "").toString();
    //v.regIdsExport = s.value("regIdsExport", true).toBool();
    v.reglistExport = s.value("reglistExport", true).toBool();
    v.regdataDeclFileName = s.value("regdataDeclFileName", "$(DATA_NAME).h").toString();
    v.regdataImplFileName = s.value("regdataImplFileName", "$(DATA_NAME).c").toString();
    v.regdataDeclUserCode = s.value("regdataDeclUserCode", "").toString();
    v.regdataImplUserCode = s.value("regdataImplUserCode", "").toString();
    v.regDataExport = s.value("regDataExport", true).toBool();
    v.cohFileName = s.value("cohFileName", "(OD_NAME).h").toString();
    v.cocFileName = s.value("cocFileName", "(OD_NAME).c").toString();
    v.cohUserCode = s.value("cohUserCode", "").toString();
    v.cocUserCode = s.value("cocUserCode", "").toString();
    v.coExport = s.value("coExport", true).toBool();
    v.edsFileName = s.value("edsFileName", "regs.eds").toString();
    v.edsExport = s.value("edsExport", true).toBool();
}

void Settings::writeExporting(QSettings& settings) const
{
    auto& v = exporting;
    auto& s = settings;

    s.beginGroup("export");

    s.setValue("path", v.path);
    s.setValue("dataName", v.dataName);
    s.setValue("odName", v.odName);
    s.setValue("edsVendorName", v.edsVendorName);
    s.setValue("edsProductName", v.edsProductName);
    //s.setValue("edsFileDescr", v.edsFileDescr);
    s.setValue("edsFileAuthor", v.edsFileAuthor);
    //s.setValue("edsVendorName", v.edsVendorName);
    //s.setValue("edsProductName", v.edsProductName);
    s.setValue("reglistFileName", v.reglistFileName);
    s.setValue("regIdsFileName", v.regIdsFileName);
    s.setValue("reglistUserCode", v.reglistUserCode);
    s.setValue("regIdsUserCode", v.regIdsUserCode);
    //s.setValue("regIdsExport", v.regIdsExport);
    s.setValue("reglistExport", v.reglistExport);
    s.setValue("regdataDeclFileName", v.regdataDeclFileName);
    s.setValue("regdataImplFileName", v.regdataImplFileName);
    s.setValue("regdataDeclUserCode", v.regdataDeclUserCode);
    s.setValue("regdataImplUserCode", v.regdataImplUserCode);
    s.setValue("regDataExport", v.regDataExport);
    s.setValue("cohFileName", v.cohFileName);
    s.setValue("cocFileName", v.cocFileName);
    s.setValue("cohUserCode", v.cohUserCode);
    s.setValue("cocUserCode", v.cocUserCode);
    s.setValue("coExport", v.coExport);
    s.setValue("edsFileName", v.edsFileName);
    s.setValue("edsExport", v.edsExport);
}
