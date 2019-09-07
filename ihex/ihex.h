#ifndef IHEX_H
#define IHEX_H
#include <QtCore>

#define _DINF_SIGNATURE_ "{DEVINF}"
#define _FLDR_SIGNATURE_ "{LDRINF}"

enum
{
    RT_DATA = 0,
    RT_EOF,
    RT_SEGMENT,
    RT_SSEG_REC,
    RT_EXT_ADDR,
    RT_ENTER_POINT
};

#pragma pack(1)

typedef struct
{
    quint8  signature[8];
    quint8	vid;
    quint8	pid;
    quint8	hw_major;
    quint8	hw_minor;
    quint8	sw_major;
    quint8	sw_minor;
}DEV_INFO_t;


typedef struct
{
    quint8  coreID;      //старшие 4 бита - код семейства (0-L, 1-M и т.д.), младшие - код модели. Например 0x01 - cortex-L1, 0x10 - cortex-M0.
    quint8	sw_minor;   //версия
    quint8	sw_major;   //загрузчика
    quint8	vid;
    quint8	pid;
    quint8	hw_minor;
    quint8	hw_major;
}LDR_INFO_t;
/*!
  coreID, sw_minor и sw_major заполняется разработчиком. Остальные поля - автоматически при прошивке.
*/

typedef struct
{
    quint8 size;
    quint16 addr;
    quint8 type;
    QByteArray data;
}iHEX_REC_t;
#pragma pack()

typedef struct
{
    quint32 startAddr;
    quint32 size;
    QList<iHEX_REC_t> records;
}iHEX_SEGMENT_t;


typedef struct
{
    quint32 startSeg;
    quint16 startOffs;
    quint32 lastAddr;
    quint32 entryPoint;
    QList<iHEX_SEGMENT_t> segments;
}iHEX_FILE_t;


bool ihex_parseFile(QByteArray source, iHEX_FILE_t* res);
bool ihex_parseString(QString source, iHEX_REC_t* res);

bool ihex_unpack(iHEX_FILE_t* source, QByteArray* dest);
bool ihex_pack(iHEX_FILE_t* dest, QByteArray* source, quint32 startAddr = 0);

QString ihex_rec_to_string(iHEX_REC_t rec, quint16 startAddr);
QByteArray ihex_file_to_string(iHEX_FILE_t file);

quint8 ihex_crc(QByteArray data, int len = 0);
QString ihex_crc(QString source);


#endif // IHEX_H
