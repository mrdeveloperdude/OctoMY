#ifndef SERIALSETTINGS_HPP
#define SERIALSETTINGS_HPP

#include <QString>
#include <QSerialPort>

struct SerialSettings {
	QString name="Default";
	qint32 baudRate=9600;
	QString stringBaudRate="9600";
	QSerialPort::DataBits dataBits=QSerialPort::Data8;
	QString stringDataBits="8";
	QSerialPort::Parity parity=QSerialPort::NoParity;
	QString stringParity="None";
	QSerialPort::StopBits stopBits=QSerialPort::OneStop;
	QString stringStopBits="1";
	QSerialPort::FlowControl flowControl=QSerialPort::NoFlowControl;
	QString stringFlowControl="None";
	bool localEchoEnabled=false;


	SerialSettings(QString name="Default", qint32 baudRate=9600, QString stringBaudRate="9600", QSerialPort::DataBits dataBits=QSerialPort::Data7, QString stringDataBits="7", QSerialPort::Parity parity=QSerialPort::NoParity, QString stringParity="None", QSerialPort::StopBits stopBits=QSerialPort::OneStop, QString stringStopBits="1", QSerialPort::FlowControl flowControl=QSerialPort::NoFlowControl, QString stringFlowControl="None", bool localEchoEnabled=false)
		: name(name)
		, baudRate(baudRate)
		, stringBaudRate(stringBaudRate)
		, dataBits(dataBits)
		, stringDataBits(stringDataBits)
		, parity(parity)
		, stringParity(stringParity)
		, stopBits(stopBits)
		, stringStopBits(stringStopBits)
		, flowControl(flowControl)
		, stringFlowControl(stringFlowControl)
		, localEchoEnabled(localEchoEnabled)
	{

	}
};


#endif // SERIALSETTINGS_HPP
