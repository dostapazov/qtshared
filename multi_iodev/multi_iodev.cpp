#include "multi_iodev.hpp"

constexpr const char * prop_host_name  = "host_name";
constexpr const char * prop_dev_params = "dev_params";
constexpr const char * prop_conn_type  = "con_type";

const char * const  QMultiIODev::con_names[] =
{
     "tcp"
    ,"udp"
#ifndef Q_OS_ANDROID
    ,"serial"
    ,"ftdi"
#endif
};

QMap<QString, QMultiIODev::conn_types_t> QMultiIODev::types;

QMultiIODev::QMultiIODev(QObject * parent)
            :QObject(parent)
{
 init_types();
 #ifdef MULTI_DEV_CONNECT_WIDGET
 #endif

}

QMultiIODev::~QMultiIODev()
{
 close();
}

QString    QMultiIODev::errorString() const
{
    return m_iodevice ? tr("%1 : %2").arg(host_name(m_iodevice)).arg(m_iodevice->errorString()) : tr("no device");
}

void QMultiIODev::init_types()
{
  if(types.isEmpty())
  {
    for(size_t i = 0;  i< sizeof(con_names)/sizeof(con_names[0]); i++)
      types.insert(QString(con_names[i]), QMultiIODev::conn_types_t(i));
  }
}

QString        QMultiIODev::conn_type(int type)
{
   const char * con_name = "unknown";
    if(size_t(type)< sizeof(con_names)/sizeof(con_names[0]))
    {
        con_name = con_names[type];
    }
   return QLatin1String(con_name);
}


QMultiIODev::conn_types_t QMultiIODev::conn_type(const QString & str)
{
  init_types();
  return (types.contains(str)) ? types[str] : unknown;
}

QList<QString> QMultiIODev::conn_types()
{
  init_types();
  QList<QString> res;
  for(size_t i = 0;  i< sizeof(con_names)/sizeof(con_names[0]); i++)
      res.append(QString(con_names[i]));
  return  res;
}

 QMultiIODev::conn_types_t   QMultiIODev::conn_type (QIODevice * dev)
 {
   conn_types_t ret(unknown);
   QVariant var = dev ? dev->property(prop_conn_type) : QVariant();
   if(var.isValid() && !var.isNull())
   {
     bool ok = false;
     uint32_t value = var.toUInt(&ok);
     if(ok) ret = conn_types_t(value);
   }
   return ret;
 }

QIODevice * QMultiIODev::do_create_device(const QString & dev_params)
{
 QStringList sl = dev_params.split(QLatin1Char('='));
 QIODevice * iodev = Q_NULLPTR;
 if(sl.size()>1)
 {
  auto c_type = conn_type(sl[0].trimmed());
  switch(c_type)
  {
     case tcp_ip : iodev = create_tcp_socket(sl[1].trimmed());break;
     case udp    : iodev = create_udp_socket(sl[1].trimmed());break;
     case ftdi   : iodev = create_ftdi      (sl[1].trimmed());break;
     case serial : iodev = create_serial    (sl[1].trimmed());break;
     default     : return Q_NULLPTR;
  }
  iodev->setProperty(prop_conn_type,int(c_type));
 }
 return  iodev;;
}

void QMultiIODev::init_socket(QAbstractSocket * socket, const QString & params )
{
    auto sl   = params.split(QLatin1Char(':'));
    QString host_name = sl[0].trimmed();
    QStringList m_params;
    socket->setProperty(prop_host_name, host_name);
    if(sl.count()>1) socket->setProperty(prop_dev_params,sl[1].trimmed());
}

QIODevice * QMultiIODev::create_tcp_socket(const QString & str)
{
   QTcpSocket * socket = new QTcpSocket();
   socket->bind(0,QAbstractSocket::BindFlag::ReuseAddressHint);
   init_socket(socket, str);
   connect(socket, &QAbstractSocket::connected   , this, &QMultiIODev::connected);
   connect(socket, &QAbstractSocket::disconnected, this, &QMultiIODev::disconnected);
   return socket;
}

QIODevice * QMultiIODev::create_udp_socket(const QString & str)
{
    QUdpSocket * socket = new QUdpSocket();
    socket->bind(0,QAbstractSocket::BindFlag::ReuseAddressHint);
    init_socket(socket, str);
    connect(socket, &QAbstractSocket::connected   , this, &QMultiIODev::connected);
    connect(socket, &QAbstractSocket::disconnected, this, &QMultiIODev::disconnected);
    return socket;
}


QIODevice * QMultiIODev::create_serial    (const QString & str)
{
#ifndef Q_OS_ANDROID
 QSerialPort * serial = new QSerialPort  ();
  auto sl = str.split(QLatin1Char(':'));
  QString  port_name =  sl[0].trimmed();
  QStringList params;
  if(sl.count()>1)   params = sl[1].trimmed().split(QLatin1Char(','));

  int32_t    speed     = int32_t (params.size()>0 ? params[0].toUInt() : uint(QSerialPort::Baud115200));
  uint8_t    data_bits = uint8_t (params.size()>1 ? params[1].toUInt() : uint(QSerialPort::Data8 ));
  uint8_t    stop_bits = uint8_t (params.size()>2 ? params[2].toUInt() : uint(QSerialPort::StopBits::OneStop) );
  uint8_t    parity    = uint8_t (params.size()>3 ? params[3].toUInt() : uint(QSerialPort::Parity::NoParity) );
  uint8_t    flow      = uint8_t (params.size()>4 ? params[4].toUInt() : uint(QSerialPort::FlowControl::NoFlowControl) );

  #ifdef Q_OS_WIN
  port_name = QString("\\\\.\\%1").arg(port_name);
  #endif

  serial->setPortName   (port_name);
  serial->setBaudRate   (speed);
  serial->setDataBits   (QSerialPort::DataBits   (data_bits));
  serial->setStopBits   (QSerialPort::StopBits   (stop_bits));
  serial->setParity     (QSerialPort::Parity     (parity)   );
  serial->setFlowControl(QSerialPort::FlowControl(flow)     );
  return serial;
#else
    Q_UNUSED(str)
    return Q_NULLPTR;
#endif
}

#ifdef _MSC_VER
#pragma warning(push)
#endif

QIODevice * QMultiIODev::create_ftdi      (const QString & str)
{
#ifndef Q_OS_ANDROID
 QFtdiDevice * ftdi = new QFtdiDevice();
 if(ftdi)
 {

    auto sl = str.split(QLatin1Char(':'));
    QString ftdi_name =  sl[0].trimmed();
    QStringList params;
    if(sl.count()>1)   params = sl[1].trimmed().split(QLatin1Char(','));

    int32_t    speed     = int32_t (params.size()>0 ? params[0].toUInt() : uint(FT_BAUD_115200) );
    uint8_t    data_bits = uint8_t (params.size()>1 ? params[1].toUInt() : uint(FT_BITS_8     ) );
    uint8_t    stop_bits = uint8_t (params.size()>2 ? params[2].toUInt() : uint(FT_STOP_BITS_1) );
    uint8_t    parity    = uint8_t (params.size()>3 ? params[3].toUInt() : uint(FT_PARITY_NONE) );
    uint16_t   flow      = uint8_t (params.size()>4 ? params[4].toUInt() : uint(FT_FLOW_NONE  ) );

    ftdi->set_params(ftdi_name, uint32_t(speed)
                     , data_bits
                     , stop_bits
                     , parity
                     , flow
                     );
  }
  return ftdi;
 #else
   Q_UNUSED(str)
   return Q_NULLPTR;
 #endif
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif


void      QMultiIODev::close       ()
{
  if(this->m_iodevice)
  {
      disconnect_from_host();
      //qDebug()<<"QMultiIoDev : destroy object "<< this->conn_type(conn_type(m_iodevice)) <<" host_name " <<host_name(m_iodevice);
      m_iodevice->close();
      //m_iodevice->deleteLater();
      delete m_iodevice;
      m_iodevice = Q_NULLPTR;
  }
}

bool       QMultiIODev::create_device(const QString & dev_params)
{
   if(m_iodevice) close();
      m_iodevice = do_create_device(dev_params);
   if(m_iodevice)
   {
    connect(m_iodevice, &QIODevice::readyRead    , this, &QMultiIODev::readyRead);
    connect(m_iodevice, &QIODevice::bytesWritten , this, &QMultiIODev::writed   );
    return true;
   }
  return false;
}


bool  QMultiIODev::open_device  (QIODevice::OpenMode mode)
{
    if(m_iodevice)
      {
        if(m_iodevice->open(mode))
          {
            connect_to_host(300) ;
            return true;
          }
      }
  return false;
}

bool   QMultiIODev::is_connected ()
{
  if(is_open())
  {
    auto socket = dynamic_cast<QAbstractSocket *>(m_iodevice);
    if(!socket)
        return true; // FTDI && SerialPort is always connected if open
    else
        return socket->state() == QAbstractSocket::SocketState::ConnectedState;
  }
 return false;
}

bool   QMultiIODev::connect_to_host     (int wait)
{
 if(m_iodevice)
 {
  auto socket = dynamic_cast<QAbstractSocket*>( m_iodevice );
  if(socket)
  {
    QString host = host_name(m_iodevice);
    quint16 port = quint16(m_iodevice->property(prop_dev_params).toUInt());
    socket->connectToHost(host, port);
    if(wait)
        return socket->waitForConnected(wait);
    return  socket->state() == QAbstractSocket::SocketState::ConnectedState; //waitForConnected(wait);

  }
  else
  {
    emit connected();
  }
 }
 return false;
}


bool       QMultiIODev::disconnect_from_host  ()
{
 auto socket =  dynamic_cast<QAbstractSocket*>(m_iodevice);
 if(socket)
     socket->disconnectFromHost();
   else
     emit disconnected();
  return true;
}


qint64    QMultiIODev::write       (const void * data, qint64 len)
{
 qint64 sz = (m_iodevice && data)  ? len : qint64(0);
 if(sz)
 {
    sz = m_iodevice->write(reinterpret_cast<const char*>(data), sz);
 }
 return sz;
}


void QMultiIODev::flush()
{
 switch(conn_type(m_iodevice))
 {
   case tcp_ip:
   case udp   :
       {
         QAbstractSocket * s = dynamic_cast<QAbstractSocket *>(m_iodevice) ;
         if(s)
          s->flush();
       }break;
#ifndef Q_OS_ANDROID
   case serial:
      {
       QSerialPort * sp = dynamic_cast<QSerialPort* >(m_iodevice);
       if(sp) sp->flush();
      }break;
#endif
   default : break;
 }
}

QString    QMultiIODev::host_name(QIODevice * io_device)
{
    QString host_name;
    if(io_device)
    {
      QVariant var = io_device->property(prop_host_name);
      if(var.isValid() && !var.isNull())
          host_name = var.toString();
      else
      {
#ifndef Q_OS_ANDROID
        auto sp = dynamic_cast<QSerialPort*>(io_device);
        if(sp)
           host_name = sp->portName();
        else
        {
          auto ftdi = dynamic_cast<QFtdiDevice*>(io_device);
          if(ftdi) host_name = ftdi->ftdiName();
        }
#endif
      }
    }
    return host_name;
}



