#include "SerialList.hpp"


#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/string/String.hpp"

#include <QList>
#include <QSerialPortInfo>


Q_DECLARE_METATYPE(QSerialPortInfo)


SerialList::SerialList(QObject *parent)
	: QObject(parent)
{
	setUpDeviceTimer(2000);
	OC_METHODGATE();
}


void SerialList::setUpDeviceTimer(const quint64 ms)
{
	OC_METHODGATE();
	onDevChangeTimer();
	//Start timer to poll for serial device changes
	if(!connect(&mDeviceChangeTimer, SIGNAL(timeout()), this, SLOT(onDevChangeTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}
	//No need for precision
	mDeviceChangeTimer.setTimerType(Qt::VeryCoarseTimer);
	//Poll for serial changes ever X ms
	mDeviceChangeTimer.start(static_cast<int>(ms));
}



QString SerialList::toSpecStanzas(QString space)
{
	OC_METHODGATE();
	QString out="";
	for(QList<QSerialPortInfo>::iterator it=mDeviceList.begin(),e=mDeviceList.end(); it!=e; ++it) {
		const QSerialPortInfo dev=*it;
		out+=space+"serial {\n";
		out+=space+"\ttype=\""+dev.description()+"\"\n";
		out+=space+"\tid=\""+dev.serialNumber()+"\"\n";
		out+=space+"\t// Manufacturer="+dev.manufacturer()+" \n";
		out+=space+"\t// Vendor="+QString::number(dev.vendorIdentifier(), 16)+" \n";
		out+=space+"\t// Location="+dev.systemLocation()+" \n";
		out+=space+"}\n\n";
	}
	return out;
}


int SerialList::count()
{
	OC_METHODGATE();
	return mDeviceList.count();
}

QSerialPortInfo SerialList::device(int index)
{
	OC_METHODGATE();
	return mDeviceList[index];
}

QString SerialList::deviceListToHash(QList<QSerialPortInfo> devices)
{
	OC_METHODGATE();
	QString summary="";
	for(QList<QSerialPortInfo>::iterator it=devices.begin(),e=devices.end(); it!=e; ++it) {
		const QSerialPortInfo dev=*it;
		summary+=dev.description()+dev.serialNumber();
	}
	return utility::string::toHash(summary);
}

void SerialList::onDevChangeTimer()
{
	OC_METHODGATE();
	mDeviceList=QSerialPortInfo::availablePorts();
	//qDebug()<<"bump: "<<mDeviceList.count();
	QString deviceListHashNew=deviceListToHash(mDeviceList);
	if(deviceListHashNew!=mDeviceListHash) {
		mDeviceListHash=deviceListHashNew;
		emit serialDevicesChanged();
	}
}

