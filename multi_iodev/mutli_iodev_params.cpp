#include "mutli_iodev_params.h"
#include <qresource.h>
#include <qscreen.h>
#include <qhostaddress.h>

#ifndef  Q_OS_ANDROID
    #include <qserialportinfo.h>
#endif


constexpr const char* miodev_conn_type = "miodev_conn_type";
constexpr const char* con_type_enabled = "con_type_disabled";

mutli_iodev_params::mutli_iodev_params(QWidget* parent) :
    QWidget(parent)

{
    //QResource::registerResource("multi_iodev_params.rcc");
    setupUi(this);
    prepare_ui();


    tbTcp->setProperty(miodev_conn_type, QMultiIODev::tcp_ip);
    tbUdp->setProperty(miodev_conn_type, QMultiIODev::udp);

#ifdef Q_OS_ANDROID
    tbSerial->setProperty(con_type_enabled, false);
    tbFtdi->setProperty  (con_type_enabled, false);
#else
    tbSerial->setProperty(miodev_conn_type, QMultiIODev::serial);
    tbFtdi->setProperty(miodev_conn_type, QMultiIODev::ftdi);
#endif

    enable_undo (false);
    enable_apply(false);

//    connect(host_addr , SIGNAL(textChanged(const QString &)) , this, SLOT(check_params_valid()));
//    connect(host_port , SIGNAL(valueChanged(int))            , this, SLOT(check_params_valid()));
//    connect(sbStopBits, SIGNAL(valueChanged(int))            , this, SLOT(check_params_valid()));

    connect(host_addr, QOverload<const QString&>::of(&QLineEdit::textChanged), this, &mutli_iodev_params::check_params_valid);
    connect(host_port, QOverload<int>::of(&QSpinBox::valueChanged), this, &mutli_iodev_params::check_params_valid);
    connect(sbStopBits, QOverload<int>::of(&QSpinBox::valueChanged), this, &mutli_iodev_params::check_params_valid);


    for (auto&& cb : gb_serial_device->findChildren<QComboBox*>())
        connect(cb, &QComboBox::editTextChanged, this, &mutli_iodev_params::check_params_valid);

    for (auto&& rb : gb_serial_device->findChildren<QRadioButton*>())
        connect(rb, &QRadioButton::clicked, this, &mutli_iodev_params::check_params_valid  );


    for (auto tb : type_frame->findChildren<QAbstractButton*>())
    {
        if (tb == tbApply || tb == tbUndo)
        {
            connect(tb, &QAbstractButton::clicked, this, &mutli_iodev_params::conn_param_apply_undo );
        }
        else
        {
            connect(tb, &QAbstractButton::toggled, this, &mutli_iodev_params::conn_type_changed );
        }
    }

    tbTcp->setChecked(true);
    emit tbTcp->clicked();

#ifndef Q_OS_ANDROID
    connect(tbRefresh, &QAbstractButton::clicked, this, &mutli_iodev_params::scan_refresh);
#endif
}


mutli_iodev_params::~mutli_iodev_params()
{
}


void mutli_iodev_params::conn_param_apply_undo ()
{
    if (sender() == tbApply)
        emit param_apply();
    else
    {
        emit param_undo();
    }
    tbApply->setEnabled(false);
    tbUndo ->setEnabled(false);
}


void    mutli_iodev_params::enable_apply(bool enable)
{
    tool_button_enable(tbApply, enable);
}

void    mutli_iodev_params::enable_undo (bool enable)
{
    tool_button_enable(tbUndo, enable);
}



void  mutli_iodev_params::prepare_ui()
{
//    auto screen =  QApplication::primaryScreen();
//    qreal ratio = screen->logicalDotsPerInch() / 96.0;
//    QSizeF szf = tbTcp->iconSize();
//    szf  = szf.scaled(szf.width() * ratio, szf.height() * ratio, Qt::AspectRatioMode::KeepAspectRatio);
//    for (auto tb : type_frame->findChildren<QAbstractButton*>())
//        tb->setIconSize(szf.toSize());
}

void mutli_iodev_params::showEvent(QShowEvent* event)
{
#ifndef Q_OS_ANDROID
    if (!cbBaud->count())
        init_serial_bauds();
#endif

    QWidget::showEvent(event);
    for (auto b : type_frame->findChildren<QAbstractButton*>())
    {
        QVariant prop = b->property(con_type_enabled);
        b->setVisible(!prop.isValid() || prop.toBool());
    }
}

/**
 * @brief mutli_iodev_params::connectionString
 * @return QString contains connection parameters
 * con_type=con_[addr|device]:different parameters
 * for example :
 * for network connection con_addr = host addr : port number
 * tcp=192.168.1.2:5000
 * for serial port name: speed,data_size, stop_bits,parity
 * serial=COM7:9600,8,1,0
 */
QString mutli_iodev_params::connectionString()
{
    QString ret ;
    int curr_conn_idx = type_connection->checkedButton()->property(miodev_conn_type).toInt();
    switch (curr_conn_idx)
    {
        case QMultiIODev::conn_types_t::tcp_ip:
        case QMultiIODev::conn_types_t::udp:
            ret = tr("%1=%2:%3").arg(QMultiIODev::conn_type(curr_conn_idx))
                  .arg(host_addr->text().trimmed())
                  .arg(host_port->value());
            break;
#ifndef Q_OS_ANDROID
        case QMultiIODev::conn_types_t::serial:
        case QMultiIODev::conn_types_t::ftdi  :
            ret = tr("%1=%2:%3").arg(QMultiIODev::conn_type(curr_conn_idx))
                  .arg(dev_name->currentText().trimmed())
                  .arg(serialParams(curr_conn_idx));
            break;

        default:
            break;

#endif
    }
    return  ret;
}

void    mutli_iodev_params::setConnectionString(const  QString& conn_str)
{
    QStringList sl = conn_str.split(QLatin1Char('='));
    QAbstractButton* tb = Q_NULLPTR;
    if (sl.count() > 1)
    {
        auto conn_type = QMultiIODev::conn_type(sl[0].trimmed());

        switch (conn_type)
        {
            case QMultiIODev::tcp_ip :
            case QMultiIODev::udp    :
                tb = conn_type == QMultiIODev::udp ? tbUdp : tbTcp;
                tb->setChecked(true);
                set_network_params(sl[1]);
                break;
#ifndef Q_OS_ANDROID
            case QMultiIODev::serial :
                tbSerial->setChecked(true);;
                set_serial_params(sl[1]);
                break;
            case QMultiIODev::ftdi :
                tbFtdi->setChecked(true);
                set_ftdi_params(sl[1]);

                break;
#endif
            default :
                break;
        }
    }
    else
    {
        tb = this->type_connection->checkedButton();
        if (tb)
            tb->setChecked(false);
    }
    tbApply->setEnabled(false);
    tbUndo ->setEnabled(false);
}

void mutli_iodev_params::set_network_params(const QString& param_str)
{
    QStringList sl = param_str.split(QLatin1Char(':'));
    QString     addr;
    QString     port;
    if (sl.count() > 0)
        addr = sl[0].trimmed();
    if (sl.count() > 1)
        port = sl[1].trimmed();
    if (addr.isEmpty())
        addr = "192.168.1.237";
    if (port.isEmpty())
        port = "5000";
    //qApp->processEvents();
    this->host_addr->setText (addr);
    this->host_port->setValue(port.toInt());
}



bool  mutli_iodev_params::check_params_valid()
{
    bool ret = false;
    if (gb_net_params->isVisible())
    {
        ret = !host_addr->text().trimmed().isEmpty() && host_port->value();
    }
#ifndef Q_OS_ANDROID
    if (gb_serial_device->isVisible())
    {
        ret = !dev_name->currentText().trimmed().isEmpty() ;
        if (ret)
        {
            bool ok = false;
            quint32 baud = cbBaud->currentText().trimmed().toUInt(&ok);
            ret &= (ok && baud);
        }
    }
#endif
    tbApply->setEnabled(ret);
    tbUndo ->setEnabled(ret);

    emit params_valid(ret);
    return ret;
}



void mutli_iodev_params::conn_type_changed(bool checked)
{
    if (!checked)
        return;
    QObject* src = sender();
    int index = src ? src->property(miodev_conn_type).toInt() : QMultiIODev::unknown;
    bool net_conn    = false;
    bool serial_conn = false;
    gb_flow_ftdi->setVisible(false);
    gb_flow_serial->setVisible(false);
    gb_net_params->setVisible(false);
    gb_serial_device->setVisible(false);

    switch (index)
    {
        case QMultiIODev::conn_types_t::tcp_ip:
        case QMultiIODev::conn_types_t::udp   :
            net_conn = true;
            break;
#ifndef Q_OS_ANDROID
        case QMultiIODev::conn_types_t::serial:
            scan_serial_port();
            if (dev_name->count())
                dev_name->setCurrentIndex(0);
            gb_flow_serial->setVisible(true);
            serial_conn = true;
            break;
        case QMultiIODev::conn_types_t::ftdi  :
            serial_conn = true;
            scan_ftdi();
            if (dev_name->count())
                dev_name->setCurrentIndex(0);
            gb_flow_ftdi->setVisible(true);

            break;
#endif
        default:
            break;
    }

    gb_net_params->setVisible(net_conn);
    gb_serial_device->setVisible(serial_conn);
    check_params_valid();
}

#ifndef Q_OS_ANDROID

void mutli_iodev_params::scan_ftdi()
{
    dev_name->clear();
    dev_name->addItems(QFtdiDevice::scan());
}


void mutli_iodev_params::scan_serial_port()
{
    dev_name->clear();
    for ( auto p : QSerialPortInfo::availablePorts())
        dev_name->addItem(p.portName());
}

QVector<uint32_t> mutli_iodev_params::bauds_list;
void  mutli_iodev_params::set_bauds_list(const uint32_t* bauds_ptr, size_t count )
{
    if (bauds_ptr && count)
    {
        bauds_list.clear();
        bauds_list.reserve(int(count));
        const uint32_t* bauds_ptr_end = bauds_ptr + count;
        while (bauds_ptr < bauds_ptr_end)
            bauds_list.append(*bauds_ptr++);
    }
}


void mutli_iodev_params::init_serial_bauds()
{

    if (!bauds_list.size())
    {
        quint32 stdRate[] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
        set_bauds_list(stdRate, sizeof(stdRate) / sizeof(stdRate[0]));
    }

    cbBaud->clear();
    for (auto baud : bauds_list)
        cbBaud->addItem(QString::number(baud), QVariant(baud));

}


QString mutli_iodev_params::serialParams(int conn_idx)
{
    QString ret;
    ret = tr("%1,%2,%3,%4,%5")
          .arg(cbBaud->currentText().trimmed())
          .arg(cbDataSize->currentIndex() ? QSerialPort::Data7 : QSerialPort::Data8)
          .arg(get_stop_bits(conn_idx))
          .arg(get_parity(conn_idx))
          .arg(get_flow  (conn_idx)) ;
    return ret;
}

int  mutli_iodev_params::get_stop_bits(int conn_idx)
{
    if (conn_idx == QMultiIODev::serial)
    {
        return this->sbStopBits->value() == 2 ? QSerialPort::TwoStop : QSerialPort::OneStop;
    }

    if (conn_idx == QMultiIODev::ftdi)
    {
        return sbStopBits->value() == 2 ? int(FT_STOP_BITS_2) : int(FT_STOP_BITS_1);
    }
    return 0;
}

int  mutli_iodev_params::get_parity(int conn_idx)
{
    if (conn_idx == QMultiIODev::serial)
    {
        if (rb_parity_odd->isChecked())
            return  QSerialPort::Parity::OddParity;

        if (rb_parity_even->isChecked())
            return QSerialPort::Parity::EvenParity;
    }

    if (conn_idx == QMultiIODev::ftdi)
    {
        if (rb_parity_odd->isChecked())
            return  FT_PARITY_ODD;

        if (rb_parity_even->isChecked())
            return FT_PARITY_EVEN;
    }

    return QSerialPort::Parity::NoParity;
}


int  mutli_iodev_params::get_flow  (int conn_idx)
{
    if (conn_idx == QMultiIODev::conn_types_t::serial)
    {
        if (rb_flow_serial_hardware->isChecked())
            return QSerialPort::FlowControl::HardwareControl;
        if (rb_flow_serial_software->isChecked())
            return QSerialPort::FlowControl::SoftwareControl;
    }

    if (conn_idx == QMultiIODev::conn_types_t::ftdi)
    {
        if (rb_flow_rts_cts->isChecked())
            return FT_FLOW_RTS_CTS ;
        if (rb_flow_dtr_dsr->isChecked())
            return FT_FLOW_DTR_DSR;
        if (rb_flow_xon_xof->isChecked())
            return FT_FLOW_XON_XOFF;

    }
    return QSerialPort::FlowControl::NoFlowControl;
}


void mutli_iodev_params::set_serial_bauds  (int baud_value)
{
    if (!cbBaud->count())
        init_serial_bauds();
    int idx = cbBaud->findData(baud_value);
    if (idx >= 0)
    {
        cbBaud->setCurrentIndex(idx);
    }
    else
        cbBaud->setEditText(QString::number(baud_value));
}

void mutli_iodev_params::set_serial_dev_name(const QString& name)
{
    int idx = dev_name->findText(name);
    if (idx < 0)
        dev_name->setEditText(name);
    else
        dev_name->setCurrentIndex(idx);

}

void mutli_iodev_params::set_serial_params (const QString& param_str)
{
    QStringList sl = param_str.split(QLatin1Char(':'));
    QString  name  ;
    QString  params;
    if (sl.count() > 0)
        name   = sl[0].trimmed();
    if (sl.count() > 1)
        params = sl[1].trimmed();
    if (!dev_name->count())
        scan_serial_port();
    set_serial_dev_name(name);

    sl = params.split(QLatin1Char(','));

    for (int i = 0 ; i < sl.count(); i++)
    {
        bool ok  = false;
        int  val =  sl[i].toInt(&ok);
        if (!ok)
            continue;
        switch (i)
        {
            case 0:
                set_serial_bauds(val);
                break;
            case 1:
                cbDataSize->setCurrentIndex(( val == QSerialPort::Data7 ) ? 1 : 0 ) ;
                break;
            case 2:
                sbStopBits->setValue( (val == QSerialPort::TwoStop) ? 2 : 1);
                break;
            case 3:
                switch (val)
                {
                    case QSerialPort::EvenParity :
                        this->rb_parity_even->setChecked(true);
                        break;
                    case QSerialPort::OddParity  :
                        this->rb_parity_odd ->setChecked(true);
                        break;
                    default :
                        rb_parity_none->setChecked(true);
                        break;
                }
                break;
            case 4:
                switch (val)
                {
                    case QSerialPort::SoftwareControl :
                        rb_flow_serial_software->setChecked(true);
                        break;
                    case QSerialPort::HardwareControl :
                        rb_flow_serial_hardware->setChecked(true);
                        break;
                    default :
                        rb_flow_serial_none->setChecked(true);
                        break;
                }
                break;

        }
    }
}

void mutli_iodev_params::set_ftdi_params   (const QString& param_str)
{
    QStringList sl = param_str.split(QLatin1Char(':'));
    QString  name  ;
    QString  params;
    if (sl.count() > 0)
        name   = sl[0].trimmed();
    if (!dev_name->count())
        scan_ftdi();
    set_serial_dev_name(name);
    if (sl.count() > 1)
        params = sl[1].trimmed();

    sl = params.split(QLatin1Char(','));
    for (int i = 0 ; i < sl.count(); i++)
    {
        bool ok  = false;
        int  val =  sl[i].toInt(&ok);
        if (!ok)
            continue;
        switch (i)
        {
            case 0:
                set_serial_bauds(val);
                break;
            case 1:
                cbDataSize->setCurrentIndex( (val == int(FT_BITS_7)) ? 1 : 0 ) ;
                break;
            case 2:
                this->sbStopBits->setValue( (val == int(FT_STOP_BITS_2)) ? 2 : 1);
                break;
            case 3:
                switch (val)
                {
                    case FT_PARITY_EVEN :
                        this->rb_parity_even->setChecked(true);
                        break;
                    case FT_PARITY_ODD  :
                        this->rb_parity_odd ->setChecked(true);
                        break;
                    default :
                        rb_parity_none->setChecked(true);
                        break;
                }
                break;
            case 4:
                switch (val)
                {
                    case FT_FLOW_DTR_DSR  :
                        rb_flow_dtr_dsr->setChecked(true);
                        break;
                    case FT_FLOW_RTS_CTS  :
                        rb_flow_rts_cts->setChecked(true);
                        break;
                    case FT_FLOW_XON_XOFF :
                        rb_flow_xon_xof->setChecked(true);
                        break;
                    default :
                        rb_flow_none->setChecked(true);
                        break;
                }
                break;

        }
    }
}

void  mutli_iodev_params::enable_valid_signal(bool enable)
{
    if (enable)
    {
        //connect(host_addr, QOverload<const QString &>::of(&QLineEdit::textChanged), this, &mutli_iodev_params::check_params_valid);
        //connect(host_port, QOverload<int>::of(&QSpinBox::valueChanged), this, &mutli_iodev_params::check_params_valid);
    }
    else
    {
        this->disconnect(SLOT(check_params_valid()));
    }
}




#endif

bool    mutli_iodev_params::interface_is_enabled(QMultiIODev::conn_types_t iface_type)
{
    auto btn = interface_button(iface_type);
    return btn ? btn->isVisible() : false;
}

void    mutli_iodev_params::tool_button_enable  (QAbstractButton* btn, bool enable)
{
    if (btn)
    {
        btn->setEnabled(enable);
        btn->setVisible(enable);
        btn->setProperty(con_type_enabled, enable);
    }
}

void    mutli_iodev_params::interface_enable    (QMultiIODev::conn_types_t iface_type, bool enable)
{
    auto btn = interface_button(iface_type);
    if (btn )
    {
        if (!enable )
            btn->setChecked(false);
        tool_button_enable(btn, enable);
    }
}


QAbstractButton* mutli_iodev_params::interface_button(QMultiIODev::conn_types_t iface_type)
{
    QAbstractButton* btn = Q_NULLPTR;
    switch (iface_type)
    {
        case QMultiIODev::tcp_ip  :
            btn = tbTcp;
            break;
        case QMultiIODev::udp     :
            btn = tbUdp;
            break;
#ifndef Q_OS_ANDROID
        case QMultiIODev::serial  :
            btn = tbSerial;
            break;
        case QMultiIODev::ftdi    :
            btn = tbSerial;
            break;
#endif
        default :
            break;
    }
    return btn ;
}

#ifndef Q_OS_ANDROID
void mutli_iodev_params::scan_refresh()
{
    if (tbSerial->isChecked())
        scan_serial_port();
    if (tbFtdi->isChecked())
        scan_ftdi();
}
#endif
