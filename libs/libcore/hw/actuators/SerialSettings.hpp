#ifndef SERIALSETTINGS_HPP
#define SERIALSETTINGS_HPP

#include <QString>
#include <QSerialPort>

struct SerialSettings {
	QString name;
	qint32 baudRate;
	QString stringBaudRate;
	QSerialPort::DataBits dataBits;
	QString stringDataBits;
	QSerialPort::Parity parity;
	QString stringParity;
	QSerialPort::StopBits stopBits;
	QString stringStopBits;
	QSerialPort::FlowControl flowControl;
	QString stringFlowControl;
	bool localEchoEnabled;
};


#endif // SERIALSETTINGS_HPP

