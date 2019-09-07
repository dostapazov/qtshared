INCLUDEPATH += $$PWD
DEFINES += QFTDIDEVICE

HEADERS += \
    $$PWD/ftdidevice.h \
    $$PWD/ftdilib.h

SOURCES += \
    $$PWD/ftdidevice.cpp \
    $$PWD/ftdilib.cpp

windows{

HEADERS += \
        $$PWD/ftd2xx.h

contains( DEFINES, FTDI_IMPLICIT_LINK ){
contains(QT_ARCH, x86_64){
  message( using amd64 ftd2xx lib)
  LIBS += $$PWD/amd64/ftd2xx.lib
}
else{
 message( using I386 ftd2xx lib)
 LIBS += $$PWD/i386/ftd2xx.lib
}
}

linux{
HEADERS += $$PWD/WinTypes.h
LIBS += -lftdi1
}





}



