#include "SensorsCourier.hpp"


#include <QDebug>
#include <QSensor>
#include <QAccelerometer>
#include <QCompass>
#include <QGyroscope>

#include <QGeoPositionInfo>
#include <QCompassReading>
#include <QAccelerometerReading>
#include <QGyroscopeReading>

//Let the CommChannel know what we want
CourierMandate SensorsCourier::mandate(){
	return man;
}

//Act on sending opportunity.
//Return number of bytes that we wrote (0> means we took advantage of the opportunity)
quint64 SensorsCourier::sendingOportunity(QDataStream &ds){
	(void)ds;
	if(man.active){
		ds<<msg;
		man.active=false;
		return msg.size();
	}
	return 0;
}



void SensorsCourier::onPositionUpdated(const QGeoPositionInfo &info){
	msg.gps=info.coordinate();
	man.active=true;
}


void SensorsCourier::onCompassUpdated(QCompassReading *r){
	if(0!=r){
		msg.compassAzimuth=r->azimuth();
		msg.compassAccuracy=r->calibrationLevel();
		man.active=true;
	}
}

void SensorsCourier::onAccelerometerUpdated(QAccelerometerReading *r){
	if(0!=r){
		msg.accellerometer=QVector3D(r->x(), r->y(), r->z());
		man.active=true;
	}
	//ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void SensorsCourier::onGyroscopeUpdated(QGyroscopeReading *r){
	if(0!=r){
		msg.gyroscope=QVector3D(r->x(), r->y(), r->z());
		man.active=true;
	}
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}
