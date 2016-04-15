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
	foreach (const QCameraInfo &ci, cams) {
		out+=space+"camera {\n";
		out+=space+"\ttype=\""+ci.description()+"\"\n";
		out+=space+"\tid=\""+ci.deviceName()+"\"\n";
		out+=space+"\t// Position="+(QCamera::BackFace==ci.position()?"BackFace":QCamera::FrontFace==ci.position()?"FrontFace":"Unspecified")+"\n";
		out+=space+"\t// Orientation="+QString::number(ci.orientation())+" degrees\n";

		out+=space+"}\n\n";
	}
	return out;
}

QString CameraList::cameraListToHash(QList<QCameraInfo> cams){
	QString summary="";
	foreach (const QCameraInfo &ci, cams) {
		//qDebug()<<"CAM: "<<cameraInfo.description() << cameraInfo.deviceName();
		summary+=ci.description()+ci.deviceName();
	}
	return utility::toHash(summary);
}

void CameraList::onDevChangeTimer()
{
	cams=QCameraInfo::availableCameras();
	QString camListHashNew=cameraListToHash(cams);
	if(camListHashNew!=camListHash){
		camListHash=camListHashNew;
		emit cameraDevicesChanged();
	}
}


