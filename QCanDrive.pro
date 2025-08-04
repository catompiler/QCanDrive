QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# debug
CONFIG += debug

# Qwt
CONFIG += qwt

# flags
QMAKE_CXXFLAGS += -O0 -ggdb


INCLUDEPATH += CANopenNode/ \
               slcan/ \
               .

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    candrivewin.cpp \
    cockpitwgt.cpp \
    main.cpp \
    CANopenNode/301/CO_Emergency.c \
    CANopenNode/301/CO_HBconsumer.c \
    CANopenNode/301/CO_NMT_Heartbeat.c \
    CANopenNode/301/CO_Node_Guarding.c \
    CANopenNode/301/CO_ODinterface.c \
    CANopenNode/301/CO_PDO.c \
    CANopenNode/301/CO_SDOclient.c \
    CANopenNode/301/CO_SDOserver.c \
    CANopenNode/301/CO_SYNC.c \
    CANopenNode/301/CO_TIME.c \
    CANopenNode/301/CO_fifo.c \
    CANopenNode/301/crc16-ccitt.c \
    CANopenNode/CANopen.c \
    CO_driver_slcan_master.c \
    cockpitserializer.cpp \
    coobjectdict.cpp \
    covaluesholder.cpp \
    covaluetypes.cpp \
    canopenwin.cpp \
    reglisteditorwgt.cpp \
    regsviewmodel.cpp \
    regsviewwgt.cpp \
    sdocomm.cpp \
    sdovalue.cpp \
    sdovaluebar.cpp \
    sdovaluebareditdlg.cpp \
    sdovaluebutton.cpp \
    sdovaluebuttoneditdlg.cpp \
    sdovaluedial.cpp \
    sdovaluedialeditdlg.cpp \
    sdovalueindicator.cpp \
    sdovalueindicatoreditdlg.cpp \
    sdovalueplot.cpp \
    sdovalueslider.cpp \
    sdovalueslidereditdlg.cpp \
    sequentialbuffer.cpp \
    settings.cpp \
    settingsdlg.cpp \
    signalcurveeditdlg.cpp \
    signalcurveprop.cpp \
    signalcurvepropmodel.cpp \
    signalplot.cpp \
    signalseriesdata.cpp \
    slcan/slcan.c \
    slcan/slcan_can_ext_fifo.c \
    slcan/slcan_can_fifo.c \
    slcan/slcan_can_msg.c \
    slcan/slcan_cmd.c \
    slcan/slcan_cmd_buf.c \
    slcan/slcan_io_fifo.c \
    slcan/slcan_master.c \
    slcan/slcan_resp_out_fifo.c \
    slcan/slcan_slave.c \
    slcan_port_qt.cpp \
    slcanopennode.cpp \
    trendploteditdlg.cpp \
    exportdlg.cpp \
    flagseditdlg.cpp \
    flagseditmodel.cpp \
    regdelegate.cpp \
    regentry.cpp \
    regentrydlg.cpp \
    reglistcoexporter.cpp \
    reglistdataexporter.cpp \
    reglisteditorwin.cpp \
    reglistedsexporter.cpp \
    reglistexporter.cpp \
    reglistmodel.cpp \
    reglistregsexporter.cpp \
    reglistserializer.cpp \
    reglistxml2serializer.cpp \
    reglistxmlserializer.cpp \
    regobject.cpp \
    regselectdlg.cpp \
    regselectmodel.cpp \
    regtypes.cpp \
    regutils.cpp \
    regvar.cpp \
    regvarmodel.cpp \
    sellineedit.cpp

HEADERS += \
    CANopenNode/301/CO_Emergency.h \
    CANopenNode/301/CO_HBconsumer.h \
    CANopenNode/301/CO_NMT_Heartbeat.h \
    CANopenNode/301/CO_Node_Guarding.h \
    CANopenNode/301/CO_ODinterface.h \
    CANopenNode/301/CO_PDO.h \
    CANopenNode/301/CO_SDOclient.h \
    CANopenNode/301/CO_SDOserver.h \
    CANopenNode/301/CO_SYNC.h \
    CANopenNode/301/CO_TIME.h \
    CANopenNode/301/CO_config.h \
    CANopenNode/301/CO_driver.h \
    CANopenNode/301/CO_fifo.h \
    CANopenNode/301/crc16-ccitt.h \
    CANopenNode/CANopen.h \
    CO_driver_target.h \
    candrivewin.h \
    canopenwin.h \
    cockpitserializer.h \
    cockpitwgt.h \
    coobjectdict.h \
    cotypes.h \
    covaluesholder.h \
    covaluetypes.h \
    reglisteditorwgt.h \
    regsviewmodel.h \
    regsviewwgt.h \
    sdocomm.h \
    sdocomm_data.h \
    sdovalue.h \
    sdovaluebar.h \
    sdovaluebareditdlg.h \
    sdovaluebutton.h \
    sdovaluebuttoneditdlg.h \
    sdovaluedial.h \
    sdovaluedialeditdlg.h \
    sdovalueindicator.h \
    sdovalueindicatoreditdlg.h \
    sdovalueplot.h \
    sdovalueslider.h \
    sdovalueslidereditdlg.h \
    sequentialbuffer.h \
    settings.h \
    settingsdlg.h \
    signalcurveeditdlg.h \
    signalcurveprop.h \
    signalcurvepropmodel.h \
    signalplot.h \
    signalplotprop.h \
    signalseriesdata.h \
    slcan/slcan.h \
    slcan/slcan_can_ext_fifo.h \
    slcan/slcan_can_fifo.h \
    slcan/slcan_can_msg.h \
    slcan/slcan_cmd.h \
    slcan/slcan_cmd_buf.h \
    slcan/slcan_defs.h \
    slcan/slcan_err.h \
    slcan/slcan_future.h \
    slcan/slcan_io_fifo.h \
    slcan/slcan_master.h \
    slcan/slcan_port.h \
    slcan/slcan_resp_out.h \
    slcan/slcan_resp_out_fifo.h \
    slcan/slcan_serial_io.h \
    slcan/slcan_slave.h \
    slcan/slcan_slave_status.h \
    slcan/slcan_utils.h \
    slcan_conf.h \
    slcan_port_qt.h \
    slcanopennode.h \
    trendploteditdlg.h \
    exportdlg.h \
    first_arg_type.hpp \
    flagseditdlg.h \
    flagseditmodel.h \
    is_base_creator.hpp \
    regdelegate.h \
    regentry.h \
    regentrydlg.h \
    reglistcoexporter.h \
    reglistdataexporter.h \
    reglisteditorwin.h \
    reglistedsexporter.h \
    reglistexporter.h \
    reglistmodel.h \
    reglistregsexporter.h \
    reglistserializer.h \
    reglistxml2serializer.h \
    reglistxmlserializer.h \
    regobject.h \
    regselectdlg.h \
    regselectmodel.h \
    regtypes.h \
    regutils.h \
    regvar.h \
    regvarmodel.h \
    sellineedit.h

FORMS += \
    candrivewin.ui \
    canopenwin.ui \
    sdovaluebareditdlg.ui \
    sdovaluebuttoneditdlg.ui \
    sdovaluedialeditdlg.ui \
    sdovalueindicatoreditdlg.ui \
    sdovalueslidereditdlg.ui \
    settingsdlg.ui \
    signalcurveeditdlg.ui \
    trendploteditdlg.ui \
    exportdlg.ui \
    flagseditdlg.ui \
    regentrydlg.ui \
    reglisteditorwin.ui \
    regselectdlg.ui

RESOURCES += \
    candrive.qrc \
    qt_canopennode_slcan.qrc \
    reglisteditor.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

