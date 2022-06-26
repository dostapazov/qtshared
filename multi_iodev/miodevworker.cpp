#include "miodevworker.h"
#include <qdebug.h>
#include <qdatetime.h>
#include <algorithm>
//#include <qcoreapplication.h>


QMultioDevWorker::QMultioDevWorker(QObject* parent)
    : QObject(parent), m_worker_mut(QMutex::Recursive)

{
    m_thread = new QThread(this);
    connect(m_thread, &QThread::finished, this, &QMultioDevWorker::sl_finished    /*,Qt::DirectConnection*/);
    connect(m_thread, &QThread::started, this, &QMultioDevWorker::sl_started     /*,Qt::DirectConnection*/);

    m_iodev = new QMultiIODev(this);

    connect(m_iodev, &QMultiIODev::connected, this, &QMultioDevWorker::sl_connected   );
    connect(m_iodev, &QMultiIODev::disconnected, this, &QMultioDevWorker::sl_disconnected);
    connect(m_iodev, &QMultiIODev::readyRead, this, &QMultioDevWorker::sl_ready_read  );
    connect(m_iodev, &QMultiIODev::writed, this, &QMultioDevWorker::sl_writed      );
    connect(this, &QMultioDevWorker::sig_dev_wite, this, &QMultioDevWorker::sl_dev_write );

    moveToThread(m_thread);
    m_iodev->moveToThread(m_thread);


}

QMultioDevWorker::~QMultioDevWorker()
{
    if (m_thread->isRunning())
    {
        m_thread->quit();
        m_thread->wait();
        m_thread->deleteLater();
    }
}

void QMultioDevWorker::sl_ready_read  ()
{
    QByteArray recv_data = m_iodev->readAll();
    if (recv_data.size())
    {
        emit sig_receive(recv_data);
        handle_recv(recv_data);
    }
}

void QMultioDevWorker::sl_connected   ()
{

    handle_connect  (true);
}

void QMultioDevWorker::sl_disconnected()
{
    handle_connect  (false);
}


void QMultioDevWorker::sl_finished()
{
    handle_thread_finish();
}

void QMultioDevWorker::sl_started()
{
    handle_thread_start();
}

void QMultioDevWorker::sl_writed      (qint64 bytes)
{
    handle_write(bytes);
}


void QMultioDevWorker::handle_thread_finish()
{
    do_device_close();
}



void QMultioDevWorker::handle_thread_start()
{
    do_device_open();
}

bool QMultioDevWorker::is_connected()
{
    QMutexLocker l(&m_worker_mut) ;
    return m_connected;
}



void QMultioDevWorker::handle_connect(bool connected)
{

#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
    m_watchdog_value.store(m_watchdog_limit);
#else
    m_watchdog_value.storeRelaxed(m_watchdog_limit);
#endif
    bool con_change ;
    {
        QMutexLocker l(&m_worker_mut) ;
        std::swap(m_connected, connected);
        con_change  = (m_connected != connected);
    }

    if (con_change)
    {
        emit sig_connect(m_connected);
    }

}

void QMultioDevWorker::handle_recv   (const QByteArray& recv_data)
{
    Q_UNUSED(recv_data);
    //qDebug()<<tr("handle recv %1").arg(recv_data.size())<<QThread::currentThreadId();
}

void QMultioDevWorker::handle_write         (qint64 wr_bytes)
{
    Q_UNUSED(wr_bytes)
// qDebug()<<tr("handle write %1").arg(wr_bytes)<<QThread::currentThreadId();
}


bool QMultioDevWorker::device_is_open ()
{
    return  m_iodev->is_open();
}

QString   QMultioDevWorker::connection_string()
{
    QMutexLocker l(&m_worker_mut);
    return m_connection_sting;
}

bool  QMultioDevWorker::set_connection_string(const QString& conn_str)
{

    if (this->connection_string() != conn_str)
    {
        bool worked = this->is_working();
        if (worked)
            stop_work();
        {
            QMutexLocker l(&m_worker_mut);
            m_connection_sting = conn_str;
        }
        if (worked)
            start_work();
        return true;
    }
    return false;
}

bool      QMultioDevWorker::do_device_open()
{
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
    m_watchdog_value.store(m_watchdog_limit);
#else
    m_watchdog_value.storeRelaxed(m_watchdog_limit);
#endif
    if (m_iodev->create_device(connection_string()))
    {
        m_iodev->device()->moveToThread(m_thread);
        if (!m_iodev->open_device())
            emit sig_device_error(m_iodev->errorString());
        else
        {
            emit sig_device_error(QString());
            return true;
        }
    }
    else
    {
        emit sig_device_error(QString(tr("Error create device")));
    }
    return  false;
}

bool      QMultioDevWorker::is_working()
{
    return m_thread && m_thread->isRunning();
}

void      QMultioDevWorker::start_work(const QString& str)
{
    if ( connection_string() != str)
    {
        stop_work();
        set_connection_string(str);
    }
    start_work();
}

void      QMultioDevWorker::start_work()
{
    if (!connection_string().isEmpty() && !is_working())
        m_thread->start();

}

void      QMultioDevWorker::stop_work()
{
    if (is_working())
    {
        m_thread->quit();
        m_thread->wait();
    }
}

void      QMultioDevWorker::do_device_close()
{
    QMutexLocker l(&m_worker_mut);
    if (device_is_open())
    {
        m_iodev->close();
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
        m_watchdog_value.store(m_watchdog_limit);
#else
        m_watchdog_value.storeRelaxed(m_watchdog_limit);
#endif
    }
}


void      QMultioDevWorker::device_write(const void* data, int size)
{
    if (device_is_open())
    {
        m_iodev->write(data, size);
        m_iodev->flush();
    }
}

void QMultioDevWorker::sl_dev_write   (QByteArray data)
{
    device_write(data.constData(), data.size());
}

bool QMultioDevWorker::check_watchdog()
{
    bool ret = !m_watchdog_value.deref();
    return ret;
}

