INCLUDEPATH +=  $$PWD \
              $$PWD/motif \

include($$PWD/plastique/plastique.pri)
include($$PWD/cleanlooks/cleanlooks.pri)
include($$PWD/shared/shared.pri)
windows{
include($$PWD/dotnetstyle/dotnetstyle.pri)
}


HEADERS += \
    $$PWD/styles.h

SOURCES += \
    $$PWD/styles.cpp

CONFIG += c++11
