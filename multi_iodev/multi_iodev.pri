INCLUDEPATH += $$PWD
CONFIG += thread c++11
QT += network

!android{
  QT += serialport
  include(../ftdidevice/ftdidevice.pri)
}



HEADERS += \
    $$PWD/multi_iodev.hpp \
    $$PWD/miodevworker.h
SOURCES += \
    $$PWD/multi_iodev.cpp \
    $$PWD/miodevworker.cpp

contains(DEFINES, MULTI_IODEV_CONFIG_WIDGET){
HEADERS   += $$PWD/mutli_iodev_params.h
SOURCES   += $$PWD/mutli_iodev_params.cpp
FORMS     += $$PWD/mutli_iodev_params.ui
RESOURCES += $$PWD/multi_iodev_params.qrc
}





