QT += widgets
QT       += core gui serialport
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Serial_Interface/fileline_provider.cpp \
    Serial_Interface/port_communicator.cpp \
    Serial_Interface/portdiscover.cpp \
    Serial_Interface/serial_interface.cpp \
    Serial_Interface/settings.cpp \
    main.cpp \
    mainwindow.cpp \


HEADERS += \
    Serial_Interface/QDebugtools.h \
    Serial_Interface/SerialTypes.h \
    Serial_Interface/fileline_provider.h \
    Serial_Interface/port_communicator.h \
    Serial_Interface/portdiscover.h \
    Serial_Interface/serial_interface.h \
    Serial_Interface/settings.h \
    mainwindow.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
