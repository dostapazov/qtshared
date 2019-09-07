#include "ihex.h"
#include "qblowfish.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif
//
//
//

QString uintToHex(quint64 val)
{
    return QString("%1").arg(val,0,16);
}

//
//
//
bool ihex_parseFile(QByteArray source, iHEX_FILE_t* res)
{
    if (!res) return false;
    QStringList lines = QString(source).split("\r\n", QString::SkipEmptyParts);

    if (!lines.count()) return false;

    bool result = true;
    iHEX_REC_t rec;

    res->startSeg = 0xffffffff;
    res->startOffs = 0x00;
    res->lastAddr = 0x00;
    res->entryPoint = 0x00;

    quint32 lNum = 1;
    qint8 segNum = -1;

    while (lines.count() && result)
    {
        result = ihex_parseString(lines.first(), &rec);
        if (result)
        {
            switch(rec.type)
            {
                case RT_EXT_ADDR:
                    if (size_t(rec.size) == sizeof(quint16))
                    {
                        quint32 eAddr = quint32(rec.data[0]);
                        eAddr = eAddr << 8;
                        eAddr += quint32(rec.data[1]);
                        eAddr = eAddr << 16;

                        //make new segment
                        iHEX_SEGMENT_t seg;
                        seg.size = 0;
                        seg.startAddr = eAddr;
                        res->segments.append(seg);
                        segNum = qint8( res->segments.count()-1);
                        break;
                    }
                // ??????????????????????????
                case RT_ENTER_POINT:
                    if (rec.size == sizeof(quint32))
                    {
                        res->entryPoint = quint32( rec.data[0]);
                        res->entryPoint <<=  8;
                        res->entryPoint |= quint32(rec.data[1]);
                        res->entryPoint <<=  8;
                        res->entryPoint |= quint32(rec.data[2]);
                        res->entryPoint <<=  8;
                        res->entryPoint |= quint32(rec.data[3]);
                    }
                    break;
                case RT_DATA:
                {

                    if (segNum > -1)
                    {
                        res->segments[segNum].size += rec.size;
                        res->segments[segNum].records.append(rec);
                    }
                    break;
                }

            }
        }
        else qDebug() << "Error parsing line " << lNum << lines.first();

        lNum++;
        lines.removeFirst();
    }

    if (!result) res->segments.clear();
    else //normalise
    {
        //calculate first segment;
        quint32 minSeg = 0xffffffff;
        for (int i = 0; i < res->segments.count(); i++)
            minSeg = qMin(minSeg, res->segments.at(i).startAddr);

        res->startSeg = minSeg;

        //normalise segments
        for (int i = 0; i < res->segments.count(); i++)
        {
            res->segments[i].startAddr -= res->startSeg;
            if (res->segments[i].startAddr == 0) //first segment, search for first record
            {
                res->startOffs = 0xffff;
                foreach(iHEX_REC_t rec, res->segments[i].records)
                    res->startOffs = qMin(res->startOffs, rec.addr);
            }
        }

        //calculate last segment
        quint32 maxSeg = 0;
        quint8 maxNum = 0;
        for (int i = 0; i < res->segments.count(); i++)
        {
            maxSeg = qMax(maxSeg, res->segments[i].startAddr);
            if (maxSeg == res->segments[i].startAddr) maxNum = quint8(i);
        }

        //calculate last addr
        quint16 maxAddr = 0;
        foreach(iHEX_REC_t rec, res->segments[maxNum].records)
        {
            maxAddr = qMax(maxAddr, quint16(rec.addr + rec.size));
        }

        res->lastAddr = maxSeg+maxAddr;

    }
    return result;
}

bool ihex_parseString(QString source, iHEX_REC_t* res)
{
    if (!res) return false;
    bool result = true;
    QByteArray buf;

    QString t = source.trimmed().toUpper();
    if (t.left(1) != ":") return false;
    t = t.remove(0,1);

    //parity check
    if (t.length()%2) return false;

    //convert to binary
    while (t.length() > 1)
    {
        quint8 val = quint8 (t.left(2).toUInt(&result, 16));
        buf.append(char(val));
        if (!result) return false;
        t.remove(0,2);
    }

    //extract size
    res->size = quint8(buf[0]);

    //extract address
    res->addr = quint8(buf[1]);
    res->addr <<= 8;
    res->addr +=quint8(buf[2]);

    //extract type
    res->type = quint8(buf[3]);

    //extract data
    res->data.resize(res->size);
    memcpy(res->data.data(), buf.constData() + sizeof(res->size) + sizeof(res->addr) + sizeof(res->type), res->size);

    //extract crc
    quint8 pcrc = quint8(buf[sizeof(res->size) + sizeof(res->addr) + sizeof(res->type) + res->size]);

    //check for size
    if (buf.count() != int(res->size + sizeof(res->size) + sizeof(res->addr) + sizeof(pcrc) + sizeof(res->type))) return false;

    //check for crc
    quint8 crc = ihex_crc(buf, buf.count() - int(sizeof(pcrc)));
    if (crc != pcrc) return false;

    return true;
}

bool ihex_unpack(iHEX_FILE_t* source, QByteArray* dest)
{
    if (!(source && dest)) return false;

    quint32 dumpSize = source->lastAddr - source->startOffs;

    dest->clear();
    dest->resize(int(dumpSize));

    memset(dest->data(), 0xff, dumpSize);

    //sort segments
    QMap<quint32, int> segMap;
    for (int i = 0; i < source->segments.count(); i++)
        segMap.insert(source->segments[i].startAddr, i);

    QList<quint32> segList = segMap.keys();
    qSort(segList);

    //FOR DEBUG
    //segList.removeLast();

    //process segments by ascending order
    foreach(quint32 sAddr, segList)
    {
        iHEX_SEGMENT_t seg = source->segments[segMap[sAddr]];
        for (int i = 0; i < seg.records.count(); i++)
        {
            iHEX_REC_t rec = seg.records[i];
            quint32 dumpOffset = rec.addr + seg.startAddr - source->startOffs;
            memcpy(dest->data()+dumpOffset, rec.data.constData(), rec.size);
        }
    }

    return true;
}

bool ihex_pack(iHEX_FILE_t* dest, QByteArray* source, quint32 startAddr)
{
    Q_UNUSED(startAddr);
    if (!(dest && source)) return false;
/*
    dest->content.clear();
    quint32 ptr = 0;

    iHEX_REC_t rec;

    //make start record
    rec.addr = 0;
    rec.type = RT_EXT_ADDR;
    rec.size = 2;
    rec.data.clear();
    rec.data.resize(rec.size);
    rec.data[0] = ((char)((quint8)((startAddr & 0xFF000000) >> 24)));
    rec.data[1] = ((char)((quint8)((startAddr & 0xFF0000) >> 16)));

    //qDebug() << Q_FUNC_INFO << startAddr << ihex_rec_to_string(rec);

    dest->content.append(rec);

    //make data records
    while (ptr < source->count())
    {
        rec.data.clear();
        rec.addr = ptr;
        rec.type = RT_DATA;
        rec.size = ((source->count() - ptr) > 16)?16:(source->count() - ptr);
        rec.data.resize(rec.size);
        memcpy(rec.data.data(), source->constData() + ptr, rec.size);
        dest->content.append(rec);

        ptr += rec.size;
    }

    //finalise entry pointer record
    rec.data.clear();
    quint32 abs_addr = startAddr + ptr;
    rec.addr = 0;
    rec.type = RT_ENTER_POINT;
    rec.size = sizeof(abs_addr);
    rec.data.resize(rec.size);
    rec.data[0] = (char)((quint8)((abs_addr & 0xff000000) >> 24));
    rec.data[1] = (char)((quint8)((abs_addr & 0xff0000) >> 16));
    rec.data[2] = (char)((quint8)((abs_addr & 0xff00) >> 8));
    rec.data[3] = (char)((quint8)(abs_addr & 0xff));
    dest->content.append(rec);


    //make EOF record
    rec.addr = 0;
    rec.size = 0;
    rec.type = RT_EOF;
    rec.data.clear();
    dest->content.append(rec);
*/
    return true;
}

QString ihex_rec_to_string(iHEX_REC_t rec, quint16 startAddr)
{
    quint16 addr = startAddr + rec.addr;
    QString res = ":";
    res += QString("%1").arg(rec.size,2,16,QChar('0')).toUpper();
    res += QString("%1").arg((addr & 0xFF00) >> 8,2,16,QChar('0')).toUpper();
    res += QString("%1").arg(addr & 0xFF,2,16,QChar('0')).toUpper();
    res += QString("%1").arg(rec.type,2,16,QChar('0')).toUpper();

    for (int i = 0; i < rec.size; i++)
        res += QString("%1").arg(quint8(rec.data[i]),2,16,QChar('0')).toUpper();

    res += ihex_crc(res);
    res += "\r\n";
    return res;
}

QByteArray ihex_file_to_string(iHEX_FILE_t file)
{
    Q_UNUSED(file);
    QByteArray res;
    /*foreach (iHEX_REC_t rec, file.content)
    {
        res.append(ihex_rec_to_string(rec, file.startAddr));
    }*/
    return res;
}

quint8 ihex_crc(QByteArray data, int len)
{
    quint8 res = 0;
    int length = len ? len : data.count();
    for (int i = 0; i < length; i++) res += quint8(data[i]);
    res = 0 - res;

    return res;
}

QString ihex_crc(QString source)
{
    //convert to binary
    QByteArray buf;

    QString t = source.trimmed().toUpper();
    if (t.left(1) != ":") return QString();
    t = t.remove(0,1);

    //parity check
    if (t.length()%2) return QString();

    //convert to binary
    while (t.length() > 1)
    {
        bool result = true;
        quint8 val = quint8(t.left(2).toUInt(&result, 16));
        buf.append(char(val));
        if (!result) return QString();
        t.remove(0,2);
    }

    //calc crc
    quint8 crc = ihex_crc(buf, buf.count());
    return QString("%1").arg(crc,2,16,QChar('0')).toUpper();
}

