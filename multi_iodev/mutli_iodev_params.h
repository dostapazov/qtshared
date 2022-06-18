#ifndef MUTLI_IODEV_PARAMS_H
#define MUTLI_IODEV_PARAMS_H

#include <QWidget>
#include <multi_iodev.hpp>
#include "ui_mutli_iodev_params.h"

class mutli_iodev_params : public QWidget,protected Ui::mutli_iodev_params
{
    Q_OBJECT

public:
    explicit mutli_iodev_params(QWidget *parent = nullptr);
    virtual ~mutli_iodev_params() override;
    QString connectionString();
    void    setConnectionString (const  QString & conn_str);
    void    enable_valid_signal (bool enable);
    void    interface_enable    (QMultiIODev::conn_types_t iface_type, bool enable);
    bool    interface_is_enabled(QMultiIODev::conn_types_t iface_type);
    void    enable_apply(bool enable);
    void    enable_undo (bool enable);

signals:
    void    params_valid(bool);
    void    param_apply ();
    void    param_undo  ();

public slots:
    bool    check_params_valid();

private slots:
    void conn_type_changed(bool checked);
    void conn_param_apply_undo();
#ifndef Q_OS_ANDROID
    void scan_refresh();
#endif

protected:
    virtual void showEvent(QShowEvent * event) override;
    void set_network_params(const QString & param_str);
    QAbstractButton * interface_button(QMultiIODev::conn_types_t iface_type);
#ifndef Q_OS_ANDROID
    QString serialParams    (int conn_idx);
    void init_serial_bauds  ();
    int  get_stop_bits      (int conn_idx);
    int  get_parity         (int conn_idx);
    int  get_flow           (int conn_idx);
    void set_serial_params  (const QString & param_str);
    void set_ftdi_params    (const QString & param_str);
    void set_serial_bauds   (int baud_value);
    void set_serial_dev_name(const QString & param_str);
    void scan_serial_port   ();
    void scan_ftdi          ();

    static QVector<uint32_t> bauds_list;
    static void  set_bauds_list(const uint32_t * bauds_ptr, size_t count );
 #endif
    void  tool_button_enable  (QAbstractButton * btn, bool enable);
    void  prepare_ui();
};

#endif // MUTLI_IODEV_PARAMS_H
