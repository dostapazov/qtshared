#ifndef FTDICONST_H
#define FTDICONST_H

//
// Device status
//
enum {
    FT_OK,
    FT_INVALID_HANDLE,
    FT_DEVICE_NOT_FOUND,
    FT_DEVICE_NOT_OPENED,
    FT_IO_ERROR,
    FT_INSUFFICIENT_RESOURCES,
    FT_INVALID_PARAMETER,
    FT_INVALID_BAUD_RATE,

    FT_DEVICE_NOT_OPENED_FOR_ERASE,
    FT_DEVICE_NOT_OPENED_FOR_WRITE,
    FT_FAILED_TO_WRITE_DEVICE,
    FT_EEPROM_READ_FAILED,
    FT_EEPROM_WRITE_FAILED,
    FT_EEPROM_ERASE_FAILED,
    FT_EEPROM_NOT_PRESENT,
    FT_EEPROM_NOT_PROGRAMMED,
    FT_INVALID_ARGS,
    FT_NOT_SUPPORTED,
    FT_OTHER_ERROR,
    FT_DEVICE_LIST_NOT_READY,
};


#define FT_SUCCESS(status) ((status) == FT_OK)

//
// FT_OpenEx Flags
//

#define FT_OPEN_BY_SERIAL_NUMBER	1
#define FT_OPEN_BY_DESCRIPTION		2
#define FT_OPEN_BY_LOCATION			4

//
// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
//

#define FT_LIST_NUMBER_ONLY			0x80000000
#define FT_LIST_BY_INDEX			0x40000000
#define FT_LIST_ALL					0x20000000

#define FT_LIST_MASK (FT_LIST_NUMBER_ONLY|FT_LIST_BY_INDEX|FT_LIST_ALL)

//
// Baud Rates
//

#define FT_BAUD_300			300
#define FT_BAUD_600			600
#define FT_BAUD_1200		1200
#define FT_BAUD_2400		2400
#define FT_BAUD_4800		4800
#define FT_BAUD_9600		9600
#define FT_BAUD_14400		14400
#define FT_BAUD_19200		19200
#define FT_BAUD_38400		38400
#define FT_BAUD_57600		57600
#define FT_BAUD_115200		115200
#define FT_BAUD_230400		230400
#define FT_BAUD_460800		460800
#define FT_BAUD_921600		921600

//
// Word Lengths
//

#define FT_BITS_8			8
#define FT_BITS_7			7

//
// Stop Bits
//

#define FT_STOP_BITS_1		 0
#define FT_STOP_BITS_2		 2

//
// Parity
//

#define FT_PARITY_NONE		 0
#define FT_PARITY_ODD		 1
#define FT_PARITY_EVEN		 2
#define FT_PARITY_MARK		 3
#define FT_PARITY_SPACE		 4

//
// Flow Control
//

#define FT_FLOW_NONE		0x0000
#define FT_FLOW_RTS_CTS		0x0100
#define FT_FLOW_DTR_DSR		0x0200
#define FT_FLOW_XON_XOFF	0x0400

//
// Purge rx and tx buffers
//
#define FT_PURGE_RX			1
#define FT_PURGE_TX			2

//
// Events
//

#define FT_EVENT_RXCHAR			1
#define FT_EVENT_MODEM_STATUS	2
#define FT_EVENT_LINE_STATUS	4

//
// Timeouts
//

#define FT_DEFAULT_RX_TIMEOUT	300
#define FT_DEFAULT_TX_TIMEOUT	300

//
// Bit Modes
//

#define FT_BITMODE_RESET					0x00
#define FT_BITMODE_ASYNC_BITBANG			0x01
#define FT_BITMODE_MPSSE					0x02
#define FT_BITMODE_SYNC_BITBANG				0x04
#define FT_BITMODE_MCU_HOST					0x08
#define FT_BITMODE_FAST_SERIAL				0x10
#define FT_BITMODE_CBUS_BITBANG				0x20
#define FT_BITMODE_SYNC_FIFO				0x40

//
// FT232R CBUS Options EEPROM values
//

#define FT_232R_CBUS_TXDEN					0x00	//	Tx Data Enable
#define FT_232R_CBUS_PWRON					0x01	//	Power On
#define FT_232R_CBUS_RXLED					0x02	//	Rx LED
#define FT_232R_CBUS_TXLED					0x03	//	Tx LED
#define FT_232R_CBUS_TXRXLED				0x04	//	Tx and Rx LED
#define FT_232R_CBUS_SLEEP					0x05	//	Sleep
#define FT_232R_CBUS_CLK48					0x06	//	48MHz clock
#define FT_232R_CBUS_CLK24					0x07	//	24MHz clock
#define FT_232R_CBUS_CLK12					0x08	//	12MHz clock
#define FT_232R_CBUS_CLK6					0x09	//	6MHz clock
#define FT_232R_CBUS_IOMODE					0x0A	//	IO Mode for CBUS bit-bang
#define FT_232R_CBUS_BITBANG_WR				0x0B	//	Bit-bang write strobe
#define FT_232R_CBUS_BITBANG_RD				0x0C	//	Bit-bang read strobe

//
// FT232H CBUS Options EEPROM values
//

#define FT_232H_CBUS_TRISTATE				0x00	//	Tristate
#define FT_232H_CBUS_TXLED					0x01	//	Tx LED
#define FT_232H_CBUS_RXLED					0x02	//	Rx LED
#define FT_232H_CBUS_TXRXLED				0x03	//	Tx and Rx LED
#define FT_232H_CBUS_PWREN					0x04	//	Power Enable
#define FT_232H_CBUS_SLEEP					0x05	//	Sleep
#define FT_232H_CBUS_DRIVE_0				0x06	//	Drive pin to logic 0
#define FT_232H_CBUS_DRIVE_1				0x07	//	Drive pin to logic 1
#define FT_232H_CBUS_IOMODE					0x08	//	IO Mode for CBUS bit-bang
#define FT_232H_CBUS_TXDEN					0x09	//	Tx Data Enable
#define FT_232H_CBUS_CLK30					0x0A	//	30MHz clock
#define FT_232H_CBUS_CLK15					0x0B	//	15MHz clock
#define FT_232H_CBUS_CLK7_5					0x0C	//	7.5MHz clock

//
// FT X Series CBUS Options EEPROM values
//

#define FT_X_SERIES_CBUS_TRISTATE			0x00	//	Tristate
#define FT_X_SERIES_CBUS_RXLED				0x01	//	Tx LED
#define FT_X_SERIES_CBUS_TXLED				0x02	//	Rx LED
#define FT_X_SERIES_CBUS_TXRXLED			0x03	//	Tx and Rx LED
#define FT_X_SERIES_CBUS_PWREN				0x04	//	Power Enable
#define FT_X_SERIES_CBUS_SLEEP				0x05	//	Sleep
#define FT_X_SERIES_CBUS_DRIVE_0			0x06	//	Drive pin to logic 0
#define FT_X_SERIES_CBUS_DRIVE_1			0x07	//	Drive pin to logic 1
#define FT_X_SERIES_CBUS_IOMODE				0x08	//	IO Mode for CBUS bit-bang
#define FT_X_SERIES_CBUS_TXDEN				0x09	//	Tx Data Enable
#define FT_X_SERIES_CBUS_CLK24				0x0A	//	24MHz clock
#define FT_X_SERIES_CBUS_CLK12				0x0B	//	12MHz clock
#define FT_X_SERIES_CBUS_CLK6				0x0C	//	6MHz clock
#define FT_X_SERIES_CBUS_BCD_CHARGER		0x0D	//	Battery charger detected
#define FT_X_SERIES_CBUS_BCD_CHARGER_N		0x0E	//	Battery charger detected inverted
#define FT_X_SERIES_CBUS_I2C_TXE			0x0F	//	I2C Tx empty
#define FT_X_SERIES_CBUS_I2C_RXF			0x10	//	I2C Rx full
#define FT_X_SERIES_CBUS_VBUS_SENSE			0x11	//	Detect VBUS
#define FT_X_SERIES_CBUS_BITBANG_WR			0x12	//	Bit-bang write strobe
#define FT_X_SERIES_CBUS_BITBANG_RD			0x13	//	Bit-bang read strobe
#define FT_X_SERIES_CBUS_TIMESTAMP			0x14	//	Toggle output when a USB SOF token is received
#define FT_X_SERIES_CBUS_KEEP_AWAKE			0x15	//



#endif // FTDICONST_H
