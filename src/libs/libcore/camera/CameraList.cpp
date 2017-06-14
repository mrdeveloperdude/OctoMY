#include "CameraList.hpp"

#include "utility/Utility.hpp"

#include <QList>
#include <QMediaService>
#include <QMediaRecorder>
#include <QCamera>

#include <QMediaMetaData>


Q_DECLARE_METATYPE(QCameraInfo)


CameraList::CameraList(QObject *parent)
	: QObject(parent)
{
	setUpDeviceTimer(5000);
}


void CameraList::setUpDeviceTimer(const quint64 ms)
{
	onDevChangeTimer();
	//Start timer to poll for camera device changes
	if(!connect(&devChangeTimer,SIGNAL(timeout()),this,SLOT(onDevChangeTimer()))){
		qWarning()<<"ERROR: could not connect";
	}
	devChangeTimer.setTimerType(Qt::VeryCoarseTimer);//No need for precision
	devChangeTimer.start(ms);//Poll for camera changes ever X ms
}


QString CameraList::toSpecStanzas(QString space){
	QString out="";
	for(QList<QCameraInfo>::iterator it=deviceList.begin(),e=deviceList.end();it!=e;++it){
		const QCameraInfo dev=*it;
		out+=space+"camera {\n";
		out+=space+"\ttype=\""+dev.description()+"\"\n";
		out+=space+"\tid=\""+dev.deviceName()+"\"\n";
		out+=space+"\t// Position="+(QCamera::BackFace==dev.position()?"BackFace":QCamera::FrontFace==dev.position()?"FrontFace":"Unspecified")+"\n";
		out+=space+"\t// Orientation="+QString::number(dev.orientation())+" degrees\n";
		out+=space+"}\n\n";
	}
	return out;
}

QString CameraList::deviceListToHash(QList<QCameraInfo> devices){
	QString summary="";
	for(QList<QCameraInfo>::iterator it=devices.begin(),e=devices.end();it!=e;++it){
		const QCameraInfo dev=*it;
		//qDebug()<<"CAM: "<<cameraInfo.description() << cameraInfo.deviceName();
		summary+=dev.description()+dev.deviceName();
	}
	return utility::toHash(summary);
}

void CameraList::onDevChangeTimer()
{
	deviceList=QCameraInfo::availableCameras();
	QString deviceListHashNew=deviceListToHash(deviceList);
	if(deviceListHashNew!=deviceListHash){
		deviceListHash=deviceListHashNew;
		emit cameraDevicesChanged();
	}
}


