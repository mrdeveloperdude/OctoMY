#include "CameraList.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/string/String.hpp"

#include <QList>
#include <QMediaRecorder>
#include <QCamera>

#include <QtMultimedia/QMediaDevices>
#include <QMediaMetaData>


Q_DECLARE_METATYPE(QCameraDevice)


CameraList::CameraList(QObject *parent)
	: QObject(parent)
{
	OC_METHODGATE();
	setUpDeviceTimer(5000);
}


void CameraList::setUpDeviceTimer(const quint64 ms)
{
	OC_METHODGATE();
	onDevChangeTimer();
	//Start timer to poll for camera device changes
	if(!connect(&mDevChangeTimer, SIGNAL(timeout()), this, SLOT(onDevChangeTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}
	mDevChangeTimer.setTimerType(Qt::VeryCoarseTimer);//No need for precision
	mDevChangeTimer.start(static_cast<int>(ms));//Poll for camera changes ever X ms
}


QString CameraList::toSpecStanzas(QString space)
{
	OC_METHODGATE();
	QString out="";
	for(auto it=mDeviceList.begin(),e=mDeviceList.end(); it!=e; ++it) {
		auto const dev=*it;
		out+=space+"camera {\n";
		out+=space+"\ttype=\""+dev.description()+"\"\n";
		//out+=space+"\tid=\""+dev.deviceName()+"\"\n";
		out+=space+"\t// Position="+(QCameraDevice::Position::BackFace==dev.position()?"BackFace":QCameraDevice::Position::FrontFace==dev.position()?"FrontFace":"Unspecified")+"\n";
		//out+=space+"\t// Orientation="+QString::number(dev.orientation())+" degrees\n";
		out+=space+"}\n\n";
	}
	return out;
}

QString CameraList::deviceListToHash(QList<QCameraDevice> devices)
{
	OC_METHODGATE();
	QString summary="";
	for(auto it=devices.begin(),e=devices.end(); it!=e; ++it) {
		auto const dev=*it;
		//qDebug()<<"CAM: "<<cameraInfo.description() << cameraInfo.deviceName();
		summary+=dev.description();
	}
	return utility::string::toHash(summary);
}

void CameraList::onDevChangeTimer()
{
	OC_METHODGATE();
// TODO: Rely on built in event instead of creating our own on a timer
	
	mDeviceList=QMediaDevices::videoInputs();
	QString deviceListHashNew=deviceListToHash(mDeviceList);
	if(deviceListHashNew!=mDeviceListHash) {
		mDeviceListHash=deviceListHashNew;
		emit cameraDevicesChanged();
	}
}


