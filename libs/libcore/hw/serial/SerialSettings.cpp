#include "SerialSettings.hpp"




SerialSettings::SerialSettings(QString name, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits, QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl, bool localEchoEnabled)
	: name(name)
	, baudRate(baudRate)
	, dataBits(dataBits)
	, parity(parity)
	, stopBits(stopBits)
	, flowControl(flowControl)
	, localEchoEnabled(localEchoEnabled)
{

}


QVariantMap SerialSettings::toMap()
{
	QVariantMap map;
	map["name"]=name;
	map["baudRate"]=baudRate;
	map["dataBits"]=dataBits;
	map["parity"]=parity;
	map["stopBits"]=stopBits;
	map["flowControl"]=flowControl;
	map["localEchoEnabled"]=localEchoEnabled;
	return map;
}


void SerialSettings::setIntBaudRate(int ib)
{
	baudRate=static_cast<QSerialPort::BaudRate>(ib);
}

int SerialSettings::intBaudRate() const
{
	return static_cast<int>(baudRate);
}

QString SerialSettings::stringBaudRate() const
{
	return baudRateToString(baudRate);
}

QString SerialSettings::stringDataBits()const
{
	return dataBitsToString(dataBits);
	/*

			ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);

			ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

			ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
			ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
			ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);


			mCurrentSettings.stringDataBits = ui->dataBitsBox->currentText();

			mCurrentSettings.stringParity = ui->parityBox->currentText();

			mCurrentSettings.stringStopBits = ui->stopBitsBox->currentText();

			mCurrentSettings.stringFlowControl = ui->flowControlBox->currentText();
			*/

}
QString SerialSettings::stringParity()const
{
	return parityToString(parity);
}
QString SerialSettings::stringStopBits()const
{
	return stopBitsToString(stopBits);
}
QString SerialSettings::stringFlowControl()const
{
	return flowControlToString(flowControl);
}


/////////////////////////////////////////////////////////////////


QString serialSettingsToString (const SerialSettings &set)
{
	QString out;
	out+="SerialSettings{name="+set.name;
	out+=", baudRate="+set.baudRate;
	out+=", dataBits="+set.dataBits;
	out+=", parity="+set.parity;
	out+=", stopBits="+set.stopBits;
	out+=", flowControl="+set.flowControl;
	out+="}";
	return out;
}



QString baudRateToString( const QSerialPort::BaudRate baudRate)
{
	return QString::number(baudRate);
}



QSerialPort::BaudRate baudRateFromString(QString string)
{
	bool ok=false;
	int i=string.toInt(&ok);
	// TODO: Look at "ok"
	return static_cast<QSerialPort::BaudRate>(i);
}



QString dataBitsToString( const QSerialPort::DataBits dataBits)
{
	return QString::number(dataBits);
}

QSerialPort::DataBits dataBitsFromString(QString string)
{
	bool ok=false;
	int i=string.toInt(&ok);
	// TODO: Look at "ok"
	return static_cast<QSerialPort::DataBits>(i);
}


QString parityToString( const QSerialPort::Parity parity)
{
#define parityToStringCASE(A, B) case(QSerialPort::A): return B
	switch(parity) {
		parityToStringCASE(NoParity,"None");
		parityToStringCASE(EvenParity,"Even");
		parityToStringCASE(OddParity,"Odd");
		parityToStringCASE(SpaceParity,"Space");
		parityToStringCASE(MarkParity,"Mark");
	default:
		return "Unknown";
	}
#undef parityToStringCASE
}

QSerialPort::Parity parityFromString(QString string)
{
#define parityFromStringCASE(A, B) if(B == string){ return QSerialPort::A;}
	parityFromStringCASE(NoParity,"None");
	parityFromStringCASE(EvenParity,"Even");
	parityFromStringCASE(OddParity,"Odd");
	parityFromStringCASE(SpaceParity,"Space");
	parityFromStringCASE(MarkParity,"Mark");
	return QSerialPort::UnknownParity;

#undef parityFromStringCASE
}



QString stopBitsToString( const QSerialPort::StopBits stopBits)
{
#define stopBitsToStringCASE(A, B) case(QSerialPort::A): return B
	switch(stopBits) {
		stopBitsToStringCASE(OneStop,"1");
		stopBitsToStringCASE(TwoStop,"2");
	default:
		return "Unknown";
	}
#undef stopBitsToStringCASE
}

QSerialPort::StopBits stopBitsFromString(QString string)
{
#define stopBitsFromStringCASE(A, B) if(B == string){ return QSerialPort::A;}
	stopBitsFromStringCASE(OneStop,"1");
	//stopBitsFromStringCASE(OneAndHalfStop,"1.5");
	stopBitsFromStringCASE(TwoStop,"2");
	return QSerialPort::UnknownStopBits;
#undef stopBitsFromStringCASE
}


QString flowControlToString( const QSerialPort::FlowControl flowControl)
{
#define flowControlToStringCASE(A, B) case(QSerialPort::A): return B
	switch(flowControl) {
		flowControlToStringCASE(NoFlowControl,"None");
		flowControlToStringCASE(HardwareControl,"RTS/CTS");
		flowControlToStringCASE(SoftwareControl,"XON/XOFF");
	default:
		return "Unknown";
	}
#undef flowControlToStringCASE

}

QSerialPort::FlowControl flowControlFromString(QString string)
{
#define flowControlFromStringCASE(A, B) if(B == string){ return QSerialPort::A;}
	flowControlFromStringCASE(NoFlowControl,"None");
	flowControlFromStringCASE(HardwareControl,"RTS/CTS");
	flowControlFromStringCASE(SoftwareControl,"XON/XOFF");
	return QSerialPort::UnknownFlowControl;
#undef flowControlFromStringCASE
}
