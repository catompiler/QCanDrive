QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# debug
CONFIG += debug

# Qwt
CONFIG += qwt


INCLUDEPATH += CANopenNode/ \
               slcan/ \
               .

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050000

SOURCES += \
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
    OD.c \
    coobjectdict.cpp \
    covaluesholder.cpp \
    covaluetypes.cpp \
    main.cpp \
    canopenwin.cpp \
    reg_data.c \
    sdocomm.cpp \
    sdovalue.cpp \
    sdovaluedial.cpp \
    sdovaluedialeditdlg.cpp \
    sdovalueplot.cpp \
    sdovalueslider.cpp \
    sdovalueslidereditdlg.cpp \
    sequentialbuffer.cpp \
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
    trendploteditdlg.cpp

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
    OD.h \
    canopenwin.h \
    coobjectdict.h \
    cotypes.h \
    covaluesholder.h \
    covaluetypes.h \
    reg_data.h \
    sdocomm.h \
    sdocomm_data.h \
    sdovalue.h \
    sdovaluedial.h \
    sdovaluedialeditdlg.h \
    sdovalueplot.h \
    sdovalueslider.h \
    sdovalueslidereditdlg.h \
    sequentialbuffer.h \
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
    trendploteditdlg.h

FORMS += \
    canopenwin.ui \
    sdovaluedialeditdlg.ui \
    sdovalueslidereditdlg.ui \
    signalcurveeditdlg.ui \
    trendploteditdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
