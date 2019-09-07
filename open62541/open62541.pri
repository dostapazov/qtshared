CONFIG += c++11
INCLUDEPATH += $$PWD/include
windows:{
INCLUDEPATH += d:/open62541/include
LIBS += -LD:\open62541\lib -lopen62541 -lws2_32

}

HEADERS += \
    $$PWD/include/qtopcua_client.hpp
SOURCES += $$PWD/src/qtopcua_client.cpp \
    $$PWD/src/qtopcua_browse.cpp

