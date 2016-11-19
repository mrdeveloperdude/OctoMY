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


SensorsCourier::SensorsCourier(QObject *parent)
	: Courier("Sensors", Courier::FIRST_USER_ID+3, parent)
	, mMandate(400)
{

}

//Let the CommChannel know what we want
CourierMandate SensorsCourier::mandate()
{
	return mMandate;
}

//Act on sending opportunity.
//Return number of bytes that we wrote (0> means we took advantage of the opportunity)
quint16 SensorsCourier::sendingOpportunity(QDataStream &ds)
{
	(void)ds;
	if(mMandate.sendActive) {
		qDebug()<<"Spending sending opportunity for sensors data";
		quint32 sz=mMessage.size();
		if(sz>mMandate.payloadSize) {
			qWarning()<<" + return 0 (message bigger than payload mandate size)";
			return 0;
		}
		ds<<mMessage;
		mMandate.sendActive=false;
		return mMessage.size();
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}



void SensorsCourier::onPositionUpdated(const QGeoPositionInfo &info)
{
	mMessage.gps=info.coordinate();
	mMandate.sendActive=true;
}


void SensorsCourier::onCompassUpdated(QCompassReading *r)
{
	if(0!=r) {
		mMessage.compassAzimuth=r->azimuth();
		mMessage.compassAccuracy=r->calibrationLevel();
		mMandate.sendActive=true;
	}
}

void SensorsCourier::onAccelerometerUpdated(QAccelerometerReading *r)
{
	if(0!=r) {
		mMessage.accellerometer=QVector3D(r->x(), r->y(), r->z());
		mMandate.sendActive=true;
	}
	//ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void SensorsCourier::onGyroscopeUpdated(QGyroscopeReading *r)
{
	if(0!=r) {
		mMessage.gyroscope=QVector3D(r->x(), r->y(), r->z());
		mMandate.sendActive=true;
	}
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}


void SensorsCourier::onColorUpdated(QColor c)
{
	qDebug()<<"COLOR UPDATATE RECEIVED :"<<c;
	mMessage.color=c;
	mMandate.sendActive=true;
}
