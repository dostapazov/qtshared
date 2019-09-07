INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/qhexstring_p.h \
    $$PWD/qstylecache_p.h \
    $$PWD/qstylehelper_p.h


contains(QMAKE_LFLAGS , -static){
}else{
SOURCES += \
    $$PWD/qstylehelper.cpp
}
