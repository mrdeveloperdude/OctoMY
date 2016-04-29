#include "SerialList.hpp"


#include "utility/Utility.hpp"

#include <QList>
#include <QMediaService>
#include <QMediaRecorder>
#include <QCamera>

#include <QSerialPortInfo>


Q_DECLARE_METATYPE(QSerialPortInfo)


SerialList::SerialList(QObject *parent)
	: QObject(parent)
{
	setUpDeviceTimer(5000);
}


void SerialList::setUpDeviceTimer(const quint64 ms)
{
	onDevChangeTimer();
	//Start timer to poll for serial device changes
	if(!connect(&deviceChangeTimer,SIGNAL(timeout()),this,SLOT(onDevChangeTimer()))){
		qWarning()<<"ERROR: could not connect";
	}
	deviceChangeTimer.setTimerType(Qt::VeryCoarseTimer);//No need for precision
	deviceChangeTimer.start(ms);//Poll for serial changes ever X ms
}



QString SerialList::toSpecStanzas(QString space){
	QString out="";
	for(QList<QSerialPortInfo>::iterator it=deviceList.begin(),e=deviceList.end();it!=e;++it){
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


QString SerialList::deviceListToHash(QList<QSerialPortInfo> devices){
	QString summary="";
	for(QList<QSerialPortInfo>::iterator it=devices.begin(),e=devices.end();it!=e;++it){
		const QSerialPortInfo dev=*it;
		//qDebug()<<"CAM: "<<cameraInfo.description() << cameraInfo.deviceName();
		summary+=dev.description()+dev.serialNumber();
	}
	return utility::toHash(summary);
}

void SerialList::onDevChangeTimer()
{
	deviceList=QSerialPortInfo::availablePorts();
	QString deviceListHashNew=deviceListToHash(deviceList);
	if(deviceListHashNew!=deviceListHash){
		deviceListHash=deviceListHashNew;
		emit serialDevicesChanged();
	}
}

