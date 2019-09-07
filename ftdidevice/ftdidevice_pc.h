#ifndef FTDIDEVICE_PC_H
#define FTDIDEVICE_PC_H

#include <QDebug>
#include <QtCore>

#if defined(Q_OS_WIN)
#include "windows.h"
#include "ftd2xx.h"

QStringList scan_pc()
{
    QStringList result;
    ulong numDevs;
    FT_STATUS status = FT_CreateDeviceInfoList(&numDevs);
    if (status == FT_OK)
    {
        if (numDevs > 0)
        {
            FT_DEVICE_LIST_INFO_NODE *devInfo = new FT_DEVICE_LIST_INFO_NODE[numDevs];
           if(devInfo)
           {
            if (FT_GetDeviceInfoList(devInfo,&numDevs) == FT_OK)
            {
                for (ulong i = 0; i < numDevs; i++)
                {
                    QString s = devInfo[i].SerialNumber;
                    if (!s.isEmpty()) result.append(s);
                }
            }
            //memory leaks detected!!! Ostapenko D.V.
            delete [] devInfo;
           }


        }
    }
    return result;
}

void close_pc(FT_HANDLE * handle)
{

#ifndef FAKE_DEVICE
  FT_Close(*handle);
#endif
  *handle = Q_NULLPTR;
}

bool open_pc(QString serialNum, uint64_t baudRate, uint8_t bits, uint8_t stopBits, uint8_t parity, uint16_t flow, uint8_t xon_char, uint8_t xoff_char , FT_HANDLE* handle, QString* _error = Q_NULLPTR)
{

   Q_ASSERT(handle);
    *handle = Q_NULLPTR;
    QString error;
    FT_STATUS stat = FT_OpenEx(reinterpret_cast<void*>(serialNum.toLocal8Bit().data()),FT_OPEN_BY_SERIAL_NUMBER,handle);
    if (stat == FT_OK)
    {
      stat = FT_SetBaudRate(*handle,quint32(baudRate));
      if(stat == FT_OK)
      {
       stat = FT_SetDataCharacteristics(*handle,bits, stopBits, parity);
       if(stat == FT_OK)
       {
         stat = FT_SetFlowControl(*handle, flow, xon_char, xoff_char);
         if(stat != FT_OK)
             error = "Can't setup flow control";
       }
       else
        error = "Can not set data characteristics";
      }
      else
        error = "Can not set baudrate";
    }
    else
     error = "Can not create handle";

    if(_error) *_error = error;

    if(stat != FT_OK)
    {
     if(*handle) FT_Close(*handle);
     *handle = Q_NULLPTR;
    }
    else
    FT_Purge(*handle,FT_PURGE_RX|FT_PURGE_TX);

    return stat == FT_OK;
}

qint64 bytesAvailable_pc(const FT_HANDLE handle)
{
    DWORD RxBytes = 0;
    DWORD TxBytes = 0;
    DWORD EventDWord;
    if (FT_GetStatus(handle,&RxBytes,&TxBytes,&EventDWord) != FT_OK) RxBytes = 0;
    return RxBytes;
}

qint64 bytesToWrite_pc(const FT_HANDLE handle)
{

    DWORD TxBytes = 0;
    DWORD RxBytes = 0;
    DWORD EventDWord;
    FT_GetStatus(handle,&RxBytes,&TxBytes,&EventDWord);
    return TxBytes;
}

qint64 readData_pc(char *data, qint64 maxlen, const FT_HANDLE handle, QString* error = Q_NULLPTR)
{

    DWORD BytesReceived = 0;
    qint64 bytesRead = qMin(maxlen,bytesAvailable_pc(handle));
    if (bytesRead && FT_Read(handle,reinterpret_cast<void*>(data),quint32(bytesRead),&BytesReceived) != FT_OK)
    {
        if (error) *error = "Read error";
        return -1;
    }
    return BytesReceived;
}

qint64 writeData_pc(const char *data, qint64 len, const FT_HANDLE handle, QString* error = Q_NULLPTR)
{
    DWORD bytesWritten;
    char * _data = const_cast<char*>(data);
    if (FT_Write(handle, reinterpret_cast<void*>(_data), quint32(len) , &bytesWritten) != FT_OK)
    {
        if(error) *error = "Write error";

        return -1;
    }
    return bytesWritten;
}

#endif

#if defined (Q_OS_LINUX)
#include "WinTypes.h"
#include <libftdi1/ftdi.h>
//#include <ftdi.h>
static const int def_vendor  = 0x0403;
static const int def_prod_id = 0x6001;

QStringList scan_pc()
{
    QStringList result;
    ftdi_context * ctx = ftdi_new();
    if(ctx)
    {
        ftdi_device_list * dev_list = Q_NULLPTR;
        int count = ftdi_usb_find_all(ctx, &dev_list, 0, 0);
        int idx   = 0;
        if(count>=0)
        {
          char serial[256] = {0};
          char descr [256] = {0};
          char manuf [256] = {0};
          ftdi_device_list * p = dev_list;
          while(p)
          {
            if(ftdi_usb_get_strings(ctx, p->dev, manuf , sizeof (manuf) , descr, sizeof (descr) , serial, sizeof(serial))<0)
            {
                qDebug()<< ftdi_get_error_string(ctx);
            }
            else
            {
              result.append(QString("%1,%2,%3").arg(serial).arg(descr).arg(idx));
            }
            ++idx;
            p = p->next;
          }
        }
        if(dev_list) ftdi_list_free(&dev_list);
        ftdi_free(ctx);
    }
    return result;
}

void close_pc(FT_HANDLE * handle)
{
  if(handle && *handle)
  {
       ftdi_context* fctx = reinterpret_cast<ftdi_context*>(*handle);
       ftdi_usb_close(fctx);
       ftdi_free(fctx);
      *handle = Q_NULLPTR;
  }
}

bool open_pc(QString serialNum, quint64 baudRate, quint8 bits, quint8 stopBits, quint8 parity, FT_HANDLE* handle, QString* error )
{

    Q_ASSERT(handle);

    ftdi_version_info ver = ftdi_get_library_version();
    qDebug()<< QString("libftdi version %1 snapshot %2").arg(ver.version_str).arg(ver.snapshot_str);

    *handle = Q_NULLPTR;
    ftdi_context * fctx = ftdi_new();
    if(fctx)
      {
        QStringList sl = serialNum.split(QChar(','), QString::SplitBehavior::SkipEmptyParts);
        std::string  serial = sl[0].toStdString();
        std::string  descr  = sl.count()>1 ? sl[1].toStdString() : std::string();
//        quint32 idx = 0;
//        if(sl.count()>2) idx = sl[2].toUInt();

         int res   = -5;
//         int count =  3;
//         while(res == -5 && count--)
//         {
//          res =   ftdi_usb_open_desc(fctx,def_vendor,def_prod_id,descr.c_str(), serial.c_str());
//          QThread::msleep(300);
//         }
            res =  ftdi_usb_open_desc(fctx,def_vendor,def_prod_id,descr.c_str(), serial.c_str());


        if(res<0)
          {
            if(error) *error = ftdi_get_error_string(fctx);
            qDebug()<<QString("open %1 error : %2").arg(serialNum).arg(ftdi_get_error_string(fctx));
            ftdi_free(fctx);

          }
        else
          {
             ftdi_usb_purge_buffers(fctx);
             ftdi_set_bitmode(fctx,0xFF, BITMODE_RESET);// Make serial mode
             ftdi_set_baudrate(fctx,int(baudRate));
             ftdi_set_line_property(fctx,ftdi_bits_type(bits),ftdi_stopbits_type(stopBits),ftdi_parity_type(parity));
            *handle = fctx;
          }
      }
    return *handle ? true : false;
}



//https://www.intra2net.com/en/developer/libftdi/documentation/group__libftdi.html
// see function ftdi_poll_modem_status

//Layout of the first byte:
//B0..B3 - must be 0
//B4 Clear to send (CTS) 0 = inactive 1 = active
//B5 Data set ready (DTS) 0 = inactive 1 = active
//B6 Ring indicator (RI) 0 = inactive 1 = active
//B7 Receive line signal detect (RLSD) 0 = inactive 1 = active
//Layout of the second byte:

//B0 Data ready (DR)
//B1 Overrun error (OE)
//B2 Parity error (PE)
//B3 Framing error (FE)
//B4 Break interrupt (BI)
//B5 Transmitter holding register (THRE)
//B6 Transmitter empty (TEMT)
//B7 Error in RCVR FIFO


#pragma pack(push,1)
union ftdi_status
{
  struct{

       struct {
              unsigned char reserved:3;
              unsigned char cts :1;
              unsigned char dts :1;
              unsigned char ri  :1;
              unsigned char rlsd:1;
              }b0;

         struct
             {
              unsigned char dr  :1;
              unsigned char oe  :1;
              unsigned char pe  :1;
              unsigned char fe  :1;
              unsigned char bi  :1;
              unsigned char thre:1;
              unsigned char temt:1;
              unsigned char rcvr_fifo:1;
              }b1;
        } stat;
  unsigned short status;
};

#pragma pack(pop)



qint64 bytesToWrite_pc(const FT_HANDLE handle)
{
  qint64 res = 0;
  if(handle)
    {
      ftdi_context * fctx = reinterpret_cast<ftdi_context *>(handle);
      res = fctx->writebuffer_chunksize;
    }

  return res;
}

qint64 readData_pc(char *data, qint64 maxlen, const FT_HANDLE handle, QString* error )
{

    Q_UNUSED(error);
    qint64 rdbytes = 0;
    if(handle && data && maxlen)
      {
       ftdi_context * fctx = reinterpret_cast<ftdi_context *>(handle);
       int res = ftdi_read_data(fctx,reinterpret_cast<quint8*>(data),int(maxlen));
       if(res<0)
         {
           if(error) *error = ftdi_get_error_string(fctx);
         }
         else
         {
          rdbytes = qint64(res);
         }
      }
    return rdbytes;
}

qint64 writeData_pc(const char *data, qint64 len, const FT_HANDLE handle, QString* error = Q_NULLPTR)
{
    DWORD bytesWritten = 0;

    if(handle && data && len)
      {
        ftdi_context * fctx = reinterpret_cast<ftdi_context *>(handle);
        int wrb = ftdi_write_data(fctx,reinterpret_cast<const quint8*>(data),int(len));
        if(wrb<0)
          {
            if(error) *error = ftdi_get_error_string(fctx);
          }
          else
          {
           bytesWritten = DWORD(wrb);
          }
      }
    return bytesWritten;
}


#endif
#endif // FTDIDEVICE_PC_H
