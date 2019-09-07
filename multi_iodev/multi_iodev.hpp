#ifndef Q_MULTI_IODEV_INC_
#define Q_MULTI_IODEV_INC_

#include <QIODevice>
#include <QMap>
#include <qtcpsocket.h>
#include <qudpsocket.h>
#ifndef Q_OS_ANDROID
#include <qserialport.h>
#include "ftdidevice.h"
#endif


/**
 * @brief The QMultiIODev class
 * holds socket, serial or ftdi usb connections
 * dev_params
 * for socket tcp  = addr:port
 * for serial serial = com1:speed 9600, data_bits 8, stop bits 1, parity 0, flow 0
 * for ftdi   ftdi   = AB106P:speed 9600, data_bits 8, stop bits 1, parity 0, flow 0
 */



class QMultiIODev:public QObject
{
   Q_OBJECT
   public:
    enum conn_types_t {unknown=-1, tcp_ip=0, udp = 1, serial = 2, ftdi = 3 };

    explicit  QMultiIODev(QObject * parent = Q_NULLPTR);
    virtual  ~QMultiIODev();

    bool       create_device(const QString & dev_params);
    bool       open_device  (QIODevice::OpenMode mode = QIODevice::ReadWrite) ;
    bool       is_open      (){return m_iodevice ? m_iodevice->isOpen() : false;}
    bool       is_connected ();
    void       close();
    qint64     bytesAvail  ();
    QByteArray readAll     ();
    qint64     write       (const QByteArray & data);
    qint64     write       (const void * data, qint64 len);
    void       flush       ();
    QIODevice* device  (){return m_iodevice;}

    bool       waitForReadyRead   (int msecs);
    bool       waitForBytesWritten(int msecs);
    QString    errorString() const ;
               QString        host_name();
    static     QString        host_name(QIODevice * io_device);
    static     QList<QString> conn_types();
    static     QString        conn_type(int type);
    static     conn_types_t   conn_type (const QString & str);
    static     conn_types_t   conn_type (QIODevice * dev);
signals:
    void readyRead   ();
    void writed      (qint64);
    void connected   ();
    void disconnected();
protected:


    QIODevice * do_create_device    (const QString & str);
    QIODevice * create_tcp_socket   (const QString & str);
    QIODevice * create_udp_socket   (const QString & str);
    QIODevice * create_serial       (const QString & str);
    QIODevice * create_ftdi         (const QString & str);
    bool        connect_to_host      (int wait = 300);
static bool     connect_to_host      (QIODevice * dev ,int wait = 300);
    bool        disconnect_from_host ();
    QIODevice * m_iodevice        = Q_NULLPTR;

    static    void init_socket(QAbstractSocket * socket, const QString & params );

    static QMap<QString,conn_types_t> types;
    static const char * const  con_names[];
    static void init_types();

};



inline qint64     QMultiIODev::bytesAvail  ()
{
  return m_iodevice ? m_iodevice->bytesAvailable() : 0;
}

inline QByteArray QMultiIODev::readAll     ()
{
  return m_iodevice ? m_iodevice->readAll() : QByteArray();
}

inline qint64     QMultiIODev::write       (const QByteArray & data)
{
 return write(data.constData(), qint64(data.size()));
}

inline bool QMultiIODev::waitForReadyRead   (int msecs)
{
 return m_iodevice ? m_iodevice->waitForReadyRead(msecs) : false;
}

inline bool QMultiIODev::waitForBytesWritten(int msecs)
{
  return m_iodevice ? m_iodevice->waitForBytesWritten(msecs) : false;
}

inline QString        QMultiIODev::host_name()
{
  return host_name(m_iodevice);
}

#endif

