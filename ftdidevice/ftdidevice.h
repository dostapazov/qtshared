#ifndef FTDIDEVICE_H
#define FTDIDEVICE_H

#include <QIODevice>
#include <QTimer>
#include "ftdilib.h"


class QFtdiNotifier;

class QFtdiDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit QFtdiDevice(QObject *parent = Q_NULLPTR);
    void set_params(QString ftdi_name, quint32 baudRate = FT_BAUD_921600, quint8 bits = FT_BITS_8
                   , quint8 stopBits = FT_STOP_BITS_1, quint8 parity = FT_PARITY_NONE
                   , quint16 flow = FT_FLOW_NONE, quint8 xon_char = 0, quint8 xoff_char = 0
                  );

    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual qint64 pos() const override;
    qint64  size() const override;
    bool    seek(qint64 pos) override;
    bool    atEnd() const override;
    bool    reset() override;
    int     poller_timeout    ()         {return m_poller_timeout;}
    void    set_poller_timeout(int value){ m_poller_timeout = value;}
    QString ftdiName();


    qint64 bytesAvailable() const override;
    qint64 bytesToWrite  () const override;
    FTDI_HANDLE_TYPE getHandle  () ;
    virtual bool isSequential() const  override;
    static QStringList scan();
    static quint64 get_default_speed()         {return  m_default_speed;}
    static void set_default_speed(quint64 spd) { m_default_speed = spd; }

protected:
            qint64 do_read_data (char *data, quint32 maxlen);
    virtual qint64 readData (char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual void   timerEvent(QTimerEvent * event) override;
    virtual void   check_for_read();

private:
mutable QByteArray   m_rx_data;
    int              m_poller_timer_id = 0;
    int              m_poller_timeout  = 10;

    FTDI_HANDLE_TYPE handle;
    QString          m_ftdi_name;
    quint32          m_baud       = FT_BAUD_921600;
    quint8           m_data_bits  = FT_BITS_8;
    quint8           m_stop_bits  = FT_STOP_BITS_1;
    quint8           m_parity     = FT_PARITY_NONE;
    quint16          m_flow       = FT_FLOW_NONE;
    quint8           m_xon_char   = 0;
    quint8           m_xoff_char  = 0;
    static           quint64        m_default_speed ;
};


inline QString QFtdiDevice::ftdiName()
{
   return m_ftdi_name;
}

inline FTDI_HANDLE_TYPE QFtdiDevice::getHandle(){return handle;}
inline  bool QFtdiDevice::isSequential() const  {return true;}

inline qint64 QFtdiDevice::pos() const
{    return 0;}

inline qint64 QFtdiDevice::size() const
{    return 0;}

inline bool QFtdiDevice::seek(qint64 pos)
{    Q_UNUSED(pos);    return false;}


inline bool QFtdiDevice::atEnd() const
{    return false;}

inline bool QFtdiDevice::reset()
{    return false;}

inline qint64 QFtdiDevice::bytesToWrite() const
{    return 0;}

#endif // FTDIDEVICE_H
