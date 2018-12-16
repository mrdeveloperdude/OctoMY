#include "SensorInput.hpp"
#include "utility/Standard.hpp"
#include <QDebug>
#include <QSensor>
#include <QAccelerometer>
#include <QCompass>
#include <QGyroscope>

#pragma GCC diagnostic ignored "-Woverloaded-virtual"


SensorInput::SensorInput(QObject *parent):
	QObject(parent)
	, mGeoPositionSource(QGeoPositionInfoSource::createDefaultSource(this))
	, mAccelerometer(nullptr)
	, mCompass(nullptr)
	, lastCompassFilterValue(0)
	, lastCompassSmoothValue(0)
{

	//qDebug()<<"SENSOR INPUT STARTING";
	load();
	if (nullptr != mGeoPositionSource) {
		mGeoPositionSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
		mGeoPositionSource->startUpdates();
	}

	QAccelerometer *a=OC_NEW QAccelerometer(this);
	if(nullptr != a) {
		if(!a->connectToBackend()) {
			delete a;
			a=nullptr;
		} else {
			mAccelerometer=a;
			//accelerometer->addFilter(this);
			mAccelerometer->setSkipDuplicates(true);
			mAccelerometer->start();
			if(!connect(mAccelerometer, SIGNAL(readingChanged()),this,SLOT(onAccelerometerReadingChanged()),OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}

	QCompass *c=OC_NEW QCompass(this);
	if(nullptr != c) {
		if(!c->connectToBackend()) {
			delete c;
			c=nullptr;
		} else {
			mCompass=c;
			mCompass->addFilter(this);
			mCompass->setSkipDuplicates(true);
			mCompass->start();
			if(!connect(mCompass, SIGNAL(readingChanged()),this,SLOT(onCompassReadingChanged()),OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}

	QGyroscope*g=OC_NEW QGyroscope(this);
	if(nullptr != g) {
		if(!g->connectToBackend()) {
			delete g;
			g=nullptr;
		} else {
			mGyroscope=g;
			//gyroscpe->addFilter(this);
			mGyroscope->setSkipDuplicates(true);
			mGyroscope->start();
			if(!connect(mGyroscope, SIGNAL(readingChanged()),this,SLOT(onGyroscopeReadingChanged()),OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}
}


void SensorInput::onAccelerometerReadingChanged()
{
	if(nullptr!=mAccelerometer) {
		QAccelerometerReading *r=mAccelerometer->reading();
		if(nullptr!=r) {
			emit accelerometerUpdated(r);
		}
	}
}

void SensorInput::onCompassReadingChanged()
{
	if(nullptr!=mCompass) {
		QCompassReading *r=mCompass->reading();
		if(nullptr!=r) {
			emit compassUpdated(r);
		}
	}
}


void SensorInput::onGyroscopeReadingChanged()
{
	if(nullptr!=mGyroscope) {
		QGyroscopeReading *r=mGyroscope->reading();
		if(nullptr!=r) {
			emit gyroscopeUpdated(r);
		}
	}
}



void SensorInput::hookSignals(QObject &o, bool hook)
{
	if(hook) {
		if(nullptr!=mGeoPositionSource) {
			if(!connect(mGeoPositionSource, SIGNAL(positionUpdated(QGeoPositionInfo)), &o, SLOT(onPositionUpdated(QGeoPositionInfo)),OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect QGeoPositionInfo for "<<o.objectName();
			}
		}
		if(!connect(this, SIGNAL(compassUpdated(QCompassReading*)), &o, SLOT(onCompassUpdated(QCompassReading *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect QCompassReading for "<<o.objectName();
		}
		if(!connect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)), &o, SLOT(onAccelerometerUpdated(QAccelerometerReading *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect QAccelerometerReading for "<<o.objectName();
		}
		if(!connect(this, SIGNAL(gyroscopeUpdated(QGyroscopeReading*)), &o, SLOT(onGyroscopeUpdated(QGyroscopeReading *)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect QGyroscopeReading for "<<o.objectName();
		}
	} else {
		if(nullptr!=mGeoPositionSource) {
			if(!disconnect(mGeoPositionSource, SIGNAL(positionUpdated(QGeoPositionInfo)), &o, SLOT(onPositionUpdated(QGeoPositionInfo)))) {
				qWarning()<<"ERROR: Could not disconnect QGeoPositionInfo for "<<o.objectName();
			}
		}
		if(!disconnect(this, SIGNAL(compassUpdated(QCompassReading*)), &o, SLOT(onCompassUpdated(QCompassReading *)))) {
			qWarning()<<"ERROR: Could not disconnect QCompassReading for "<<o.objectName();
		}
		if(!disconnect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)), &o, SLOT(onAccelerometerUpdated(QAccelerometerReading *)))) {
			qWarning()<<"ERROR: Could not disconnect QAccelerometerReading for "<<o.objectName();
		}
		if(!disconnect(this, SIGNAL(gyroscopeUpdated(QGyroscopeReading*)), &o, SLOT(onGyroscopeUpdated(QGyroscopeReading *)))) {
			qWarning()<<"ERROR: Could not disconnect QGyroscopeReading for "<<o.objectName();
		}
	}
}


void SensorInput::load()
{
	mAvailableSensors.clear();
	QList<QByteArray> sensorTypes=QSensor::sensorTypes();
	for(QList<QByteArray> ::iterator it=sensorTypes.begin(),e=sensorTypes.end(); it!=e; ++it) {
		QByteArray &type=*it;
		//qDebug() << " + Found type" << type;
		QList<QByteArray> sensorsForTypes=QSensor::sensorsForType(type);
		for(QList<QByteArray> ::iterator it2=sensorsForTypes.begin(),e2=sensorsForTypes.end(); it2!=e2; ++it2) {
			QByteArray &identifier=*it2;
			QSensor* sensor = OC_NEW QSensor(type, this);
			sensor->setIdentifier(identifier);
			if (!sensor->connectToBackend()) {
				//qDebug() << "    * Skipping inactive sensor " << identifier;
				continue;
			}
			//qDebug() << "    * Adding identifier" << identifier;
			mAvailableSensors.append(sensor);
		}
	}
}

QString SensorInput::toSpecStanzas(QString space)
{
	QString out="";
	for(QList<QSensor*>::iterator i=mAvailableSensors.begin(), e=mAvailableSensors.end(); i!=e; ++i) {
		out+=space+"sensor {\n";
		out+=space+"\ttype="+(*i)->type()+"\n";
		out+=space+"\tid="+(*i)->identifier()+"\n";
		out+=space+"}\n\n";
	}
	return out;
}

///////////////////// Filters


bool SensorInput::filter(QCompassReading *reading)
{
	lastCompassSmoothValue=lastCompassSmoothValue*0.95+reading->azimuth()*0.05;
	const qreal minDelta=1.0;
	if(lastCompassSmoothValue-minDelta > lastCompassFilterValue || lastCompassSmoothValue+minDelta < lastCompassFilterValue ) {
		lastCompassFilterValue=lastCompassSmoothValue;
		return true;
	}
	return false;
}
