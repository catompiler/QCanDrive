QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    regarray.cpp \
    regentry.cpp \
    regentrydlg.cpp \
    reglisteditorwin.cpp \
    reglistmodel.cpp \
    regobject.cpp \
    regrecord.cpp \
    regtypes.cpp \
    regvar.cpp \
    regvardelegate.cpp \
    regvarmodel.cpp

HEADERS += \
    is_base_creator.hpp \
    regarray.h \
    regentry.h \
    regentrydlg.h \
    reglisteditorwin.h \
    reglistmodel.h \
    regobject.h \
    regrecord.h \
    regtypes.h \
    regvar.h \
    regvardelegate.h \
    regvarmodel.h

FORMS += \
    regentrydlg.ui \
    reglisteditorwin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
