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
CourierMandate SensorsCourier::mandate() const
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
		const quint32 bytes=mMessage.bytes();
		if(bytes>mMandate.payloadSize) {
			qWarning()<<" + return 0 (message size "<<bytes<<" bigger than payload mandate size "<<mMandate.payloadSize<<" )";
			return 0;
		}
		ds<<mMessage;
		mMandate.sendActive=false;
		return bytes;
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}

//Act on data received
//Return number of bytes actually read.
quint16 SensorsCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	(void)ds;
	(void)availableBytes;
}


//////////////////////////////////////////////////////////////////////////////////

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
