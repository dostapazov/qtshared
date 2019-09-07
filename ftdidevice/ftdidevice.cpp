#include <QDebug>
#include <QStringList>
#include <QThread>
#include <utility>
#include "qcoreevent.h"
#include "ftdidevice.h"


quint64 QFtdiDevice::m_default_speed = FT_BAUD_921600;

QFtdiDevice::QFtdiDevice(QObject *parent) :
    QIODevice(parent)
{
    setOpenMode(NotOpen);
    handle = FTDI_ZERO_HANDLE;


//    connect(&reader, SIGNAL(timeout()), SLOT(onCheckForRead()));
}


void QFtdiDevice::set_params(QString ftdi_name, quint32 baudRate, quint8 bits, quint8 stopBits, quint8 parity, quint16 flow, quint8 xon_char, quint8 xoff_char)
{
    m_ftdi_name   = std::move(ftdi_name);
    m_baud        = baudRate;
    m_data_bits   = bits;
    m_stop_bits   = stopBits;
    m_parity      = parity;
    m_flow        = flow;
    m_xon_char    = xon_char;
    m_xoff_char   = xoff_char;
}

qint64 QFtdiDevice::bytesAvailable() const
{
 char   buf[1024];
 qint64 len;

 auto  dev = const_cast<QFtdiDevice *>(this);
 while( (len = dev->do_read_data(buf, sizeof(buf))) )
   {
     m_rx_data.append(buf,int(len));
   }
 return qint64(m_rx_data.size());
}


QStringList QFtdiDevice::scan()
{
    QStringList result;
    DWORD numDevs = 0;
    FT_STATUS status = QFtdiLibrary::FT_CreateDeviceInfoList(&numDevs);
    if (status == FT_OK && numDevs)
    {
        if (numDevs > 0)
        {
           FT_DEVICE_LIST_INFO_NODE *info = new FT_DEVICE_LIST_INFO_NODE[numDevs];
           result.reserve(int(numDevs));
           if(info)
           {
            if (QFtdiLibrary::FT_GetDeviceInfoList(info,&numDevs) == FT_OK)
            {
                FT_DEVICE_LIST_INFO_NODE * beg_info = info;
                FT_DEVICE_LIST_INFO_NODE * end_info = info + numDevs;
                while(beg_info < end_info)
                {
                  if(*beg_info->SerialNumber)
                      result.append(QString::fromLocal8Bit(info->SerialNumber));

                  ++beg_info;
                }
            }
            delete [] info;
           }
        }
    }
    else {
        qDebug()<<tr("FT_STATUS %1").arg(status);
    }
    return result;
}

bool QFtdiDevice::open(QIODevice::OpenMode mode)
{

 QString err;
 if (mode <= ReadWrite)
 {

 if(!m_ftdi_name.isEmpty())
 {

  FT_STATUS status =  QFtdiLibrary::FT_OpenEx(reinterpret_cast<void*>(m_ftdi_name.toLocal8Bit().data()),FT_OPEN_BY_SERIAL_NUMBER,&handle);
  if(status == FT_OK)
  {
      status = QFtdiLibrary::FT_SetBaudRate(handle, m_baud);
      if(status == FT_OK)
      {
        status = QFtdiLibrary::FT_SetDataCharacteristics(handle, m_data_bits, m_stop_bits, m_parity);
        if(status == FT_OK)
        {
          status = QFtdiLibrary::FT_SetFlowControl(handle, m_flow, m_xon_char, m_xoff_char);
          if(status == FT_OK)
          {
            status = QFtdiLibrary::FT_Purge(handle, FT_PURGE_RX|FT_PURGE_TX);
          }
          else
           err = tr("Error setup device flow control");

        }
        else
         err = tr("Error setup device charcteristics");
      }
      else
       err = tr("Error setup device baud rate");

  }
  else
     err = tr("Error open device");

   if (status == FT_OK)
   {
     m_poller_timer_id = startTimer(std::chrono::milliseconds(m_poller_timeout), Qt::TimerType::PreciseTimer);
     setOpenMode(mode);
   }
   else
   {

     if(handle) close();
   }

 }
 else
    err = tr("No device name");
 }
 else
  err = tr("Unsuported open mode");
 setErrorString(err);
 return handle ;
}

void QFtdiDevice::close()
{

 if (handle)
   {
     emit aboutToClose();
     QFtdiLibrary::FT_Purge(handle, FT_PURGE_RX|FT_PURGE_TX);
     QFtdiLibrary::FT_Close(handle);
   }

 if(m_poller_timer_id)
 {
   killTimer(m_poller_timer_id);
   m_poller_timer_id = 0;
 }
    handle = FTDI_ZERO_HANDLE;
    setOpenMode(QIODevice::NotOpen);
}


void   QFtdiDevice::timerEvent(QTimerEvent * event)
{
  if(event && event->timerId() == m_poller_timer_id)
     {
      event->accept();
      check_for_read();
     }
}

void QFtdiDevice::check_for_read()
{

    qint64 ba = bytesAvailable();

    if (ba)
    {
        emit readyRead();
    }
}

//
// Protected
//

qint64 QFtdiDevice::readData(char *data, qint64 maxlen)
{
  qint64 rd_len = 0;
    if(data && maxlen && !m_rx_data.isEmpty())
   {
      rd_len = qMin(maxlen, qint64(this->m_rx_data.size()) );
      memcpy(data, m_rx_data.constData(), size_t(rd_len));
      m_rx_data.remove(0, int(rd_len) );
   }
 return rd_len;
}

qint64 QFtdiDevice::do_read_data (char *data, quint32 maxlen)
{

    DWORD rd_bytes = 0;
    DWORD stub;
    FT_STATUS status;
    status = QFtdiLibrary::FT_GetStatus( handle, &rd_bytes, &stub, &stub );
    if(status == FT_OK && rd_bytes)
    {
        maxlen = qMin(maxlen, quint32(rd_bytes));
        status = QFtdiLibrary::FT_Read(handle, data, maxlen, &rd_bytes);
    }
    if (status != FT_OK) {setErrorString(tr("Read error"));rd_bytes = 0;}
    return qint64(rd_bytes);
}

qint64 QFtdiDevice::writeData(const char *data, qint64 len)
{

    DWORD res;
    char * d = const_cast<char*>(data);
    FT_STATUS status = QFtdiLibrary::FT_Write(handle,  d  ,uint32_t( len ), &res );
    if(status != FT_OK)
    {
        setErrorString(tr("Error write data"));
        return -1;
    }
    //qDebug()<<tr ("%1 bytes writen %2").arg(m_serial_name).arg(res);
    emit bytesWritten(res);
    return qint64(res);
}

