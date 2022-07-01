/* Ostapenko D. V. 2018-12-01
 * NIKTES
 *
 * Работа с устройством ввода/вывода
 * Обмен производится в потоке.
 * При срабатывании watchdog таймера устройство закрывается а затем открывается
 * Запретить watchdog можно установив отрицательное значение watchdog_limit
 *
 */


#ifndef MIODEVWORKER_H
#define MIODEVWORKER_H

#include <qthread.h>
#include <qmutex.h>
#include <multi_iodev.hpp>




class QMultioDevWorker: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QMultioDevWorker)
public:
    explicit QMultioDevWorker(QObject* parent = Q_NULLPTR);
    virtual ~QMultioDevWorker() override;
    bool      is_connected();
    QString   connection_string();

    virtual
    bool      set_connection_string(const QString& conn_str);

    bool      check_watchdog ();
    int       watchdog_limit ()            { return m_watchdog_limit; }
    void      set_watchdog_limit(int value);
    bool      is_working();

    void start_work (const QString& str);
    void start_work ();
    void stop_work  ();
    void operator <<(const QByteArray& data);
    bool device_is_open();
signals  :
    void sig_receive     (QByteArray rcv_data);
    void sig_connect     (bool);
    void sig_device_error(QString error);
    void sig_dev_wite    (QByteArray data);

protected:

    virtual void device_write  (const void* data, int size);
    virtual bool do_device_open();
    virtual void do_device_close();
    virtual void handle_connect(bool connected);
    virtual void handle_recv   (const QByteArray& recv_data);
    virtual void handle_thread_finish ();
    virtual void handle_thread_start  ();
    virtual void handle_write         (qint64 wr_bytes);

private slots:
    void sl_ready_read  ();
    void sl_writed      (qint64 bytes);
    void sl_connected   ();
    void sl_disconnected();
    void sl_finished    ();
    void sl_started     ();
    void sl_dev_write   (QByteArray data);
protected:
    QMutex        m_worker_mut;
    QString       m_connection_sting ;
    bool          m_connected = false;
    QThread*      m_thread    = Q_NULLPTR;
    QMultiIODev* m_iodev     = Q_NULLPTR;
    QAtomicInt    m_watchdog_value;
    int           m_watchdog_limit = 3;

    void resetWatchDog();
};

inline void      QMultioDevWorker::operator << (const QByteArray& data)
{
    emit sig_dev_wite(data);
}

inline void      QMultioDevWorker::set_watchdog_limit(int value)
{
    m_watchdog_limit = value;
    resetWatchDog();
}


#endif // MIODEVWORKER_H
