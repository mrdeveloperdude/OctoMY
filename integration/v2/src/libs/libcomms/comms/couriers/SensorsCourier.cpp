#include "SensorsCourier.hpp"

#include "utility/network/SerialSize.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>
#include <QSensor>
#include <QAccelerometer>
#include <QCompass>
#include <QGyroscope>

#include <QGeoPositionInfo>
#include <QCompassReading>
#include <QAccelerometerReading>
#include <QGyroscopeReading>


SensorsCourier::SensorsCourier(QSharedPointer<CommsChannel> comms, QObject *parent)
	: Courier("Sensors", Courier::FIRST_USER_ID+3, comms, parent)
	, mMandate(400)

	  //const quint16 payloadSize , const quint8 priority=10, const quint64 interval=1000, const bool receiveActive=true, const bool sendActive=true);
{
	OC_METHODGATE();
	SerialSize size;
	mMandate.payloadSize=static_cast<quint16>(size(mMessage));
}

//Let the CommChannel know what we want
CourierMandate SensorsCourier::mandate() const
{
	OC_METHODGATE();
	return mMandate;
}

//Act on sending opportunity.
//Return number of bytes that we wrote (0> means we took advantage of the opportunity)
quint16 SensorsCourier::sendingOpportunity(QDataStream &ds)
{
	OC_METHODGATE();
	Q_UNUSED(ds);
	if(mMandate.sendActive) {
		qDebug()<<"Spending sending opportunity for sensors data";
		const quint16 bytes=static_cast<quint16>(mMessage.bytes());
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
	OC_METHODGATE();
	Q_UNUSED(ds);
	Q_UNUSED(availableBytes);
	// TODO: Implement
	return 0;
}


// https://stackoverflow.com/questions/52030285/what-is-correct-way-to-convert-qgyroscopereading-to-qvector3d
template <typename T>
static QVector3D readingToVector(T *r)
{
	OC_FUNCTIONGATE();
	if(nullptr==r){
		return QVector3D();
	}
	return QVector3D(static_cast<float>(r->x()), static_cast<float>(r->y()), static_cast<float>(r->z()));
}

//////////////////////////////////////////////////////////////////////////////////

void SensorsCourier::onPositionUpdated(const QGeoPositionInfo &info)
{
	OC_METHODGATE();
	mMessage.gps=info.coordinate();
	mMandate.sendActive=true;
}


void SensorsCourier::onCompassUpdated(QCompassReading *r)
{
	OC_METHODGATE();
	if(nullptr!=r) {
		mMessage.compassAzimuth=r->azimuth();
		mMessage.compassAccuracy=r->calibrationLevel();
		mMandate.sendActive=true;
	}
}

void SensorsCourier::onAccelerometerUpdated(QAccelerometerReading *r)
{
	OC_METHODGATE();
	if(nullptr!=r) {
		mMessage.accellerometer=readingToVector(r);
		mMandate.sendActive=true;
	}
	//ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void SensorsCourier::onGyroscopeUpdated(QGyroscopeReading *r)
{
	OC_METHODGATE();
	if(nullptr!=r) {
		mMessage.gyroscope=readingToVector(r);
		mMandate.sendActive=true;
	}
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}


void SensorsCourier::onColorUpdated(QColor c)
{
	OC_METHODGATE();
	qDebug()<<"COLOR UPDATATE RECEIVED :"<<c;
	mMessage.color=c;
	mMandate.sendActive=true;
}
