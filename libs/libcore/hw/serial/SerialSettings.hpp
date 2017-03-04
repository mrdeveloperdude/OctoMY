#ifndef SERIALSETTINGS_HPP
#define SERIALSETTINGS_HPP

#include <QString>
#include <QSerialPort>
#include <QVariantMap>


QString baudRateToString( const QSerialPort::BaudRate baudRate);
QSerialPort::BaudRate baudRateFromString(QString string);

QString dataBitsToString( const QSerialPort::DataBits dataBits);
QSerialPort::DataBits dataBitsFromString(QString string);

QString parityToString( const QSerialPort::Parity parity);
QSerialPort::Parity parityFromString(QString string);

QString stopBitsToString( const QSerialPort::StopBits);
QSerialPort::StopBits stopBitsFromString(QString string);

QString flowControlToString( const QSerialPort::FlowControl);
QSerialPort::FlowControl flowControlFromString(QString string);




struct SerialSettings {
	QString name="Default";
	QSerialPort::BaudRate baudRate=QSerialPort::Baud9600;
	QSerialPort::DataBits dataBits=QSerialPort::Data8;
	QSerialPort::Parity parity=QSerialPort::NoParity;
	QSerialPort::StopBits stopBits=QSerialPort::OneStop;
	QSerialPort::FlowControl flowControl=QSerialPort::NoFlowControl;
	bool localEchoEnabled=false;

public:
	SerialSettings(QString name="Default", QSerialPort::BaudRate baudRate=QSerialPort::Baud9600, QSerialPort::DataBits dataBits=QSerialPort::Data8, QSerialPort::Parity parity=QSerialPort::NoParity, QSerialPort::StopBits stopBits=QSerialPort::OneStop, QSerialPort::FlowControl flowControl=QSerialPort::NoFlowControl, bool localEchoEnabled=false);

public:
	QVariantMap toMap();

	void setIntBaudRate(int ib);
	int intBaudRate() const;
	QString stringBaudRate() const;
	QString stringDataBits()const;
	QString stringParity()const;
	QString stringStopBits()const;
	QString stringFlowControl()const;


};



QString serialSettingsToString (const SerialSettings &set);

#endif // SERIALSETTINGS_HPP
