#include "SensorInput.hpp"
#include "basic/Standard.hpp"
#include <QDebug>
#include <QSensor>
#include <QAccelerometer>
#include <QCompass>
#include <QGyroscope>

SensorInput::SensorInput(QObject *parent):
	QObject(parent)
  , source(QGeoPositionInfoSource::createDefaultSource(this))
  , accelerometer(0)
  , compass(0)
  , lastCompassFilterValue(0)
  , lastCompassSmoothValue(0)
{
	if (0 != source) {
		source->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
		source->startUpdates();
	}

	QAccelerometer *a=new QAccelerometer(this);
	if(0!=a){
		if(!a->connectToBackend()){
			delete a;
			a=0;
		}
		else{
			accelerometer=a;
			//accelerometer->addFilter(this);
			accelerometer->setSkipDuplicates(true);
			accelerometer->start();
			if(!connect(accelerometer, SIGNAL(readingChanged()),this,SLOT(onAccelerometerReadingChanged()),WWCONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}

	QCompass *c=new QCompass(this);
	if(0!=c){
		if(!c->connectToBackend()){
			delete c;
			c=0;
		}
		else{
			compass=c;
			compass->addFilter(this);
			compass->setSkipDuplicates(true);
			compass->start();
			if(!connect(compass, SIGNAL(readingChanged()),this,SLOT(onCompassReadingChanged()),WWCONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}

	QGyroscope*g=new QGyroscope(this);
	if(0!=g){
		if(!g->connectToBackend()){
			delete g;
			g=0;
		}
		else{
			gyroscope=g;
			//gyroscpe->addFilter(this);
			gyroscope->setSkipDuplicates(true);
			gyroscope->start();
			if(!connect(gyroscope, SIGNAL(readingChanged()),this,SLOT(onGyroscopeReadingChanged()),WWCONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}
}


void SensorInput::onAccelerometerReadingChanged() {
	if(0!=accelerometer){
		QAccelerometerReading *r=accelerometer->reading();
		if(0!=r){
			emit accelerometerUpdated(r);
		}
	}
}

void SensorInput::onCompassReadingChanged() {
	if(0!=compass){
		QCompassReading *r=compass->reading();
		if(0!=r){
			emit compassUpdated(r);
		}
	}
}


void SensorInput::onGyroscopeReadingChanged() {
	if(0!=gyroscope){
		QGyroscopeReading *r=gyroscope->reading();
		if(0!=r){
			emit gyroscopeUpdated(r);
		}
	}
}

/*
	"QOrientationSensor"
	"QLightSensor"
	"QMagnetometer"
	"QAmbientLightSensor"
	"QRotationSensor"
	"QGyroscope"
	"QTiltSensor"
	"QProximitySensor"
*/


void SensorInput::hookSignals(QObject &o){

	if(0!=source){
		if(!connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),&o,SLOT(onPositionUpdated(QGeoPositionInfo)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
	}

	if(!connect(this, SIGNAL(compassUpdated(QCompassReading*)),&o,SLOT(onCompassUpdated(QCompassReading *)),WWCONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)),&o,SLOT(onAccelerometerUpdated(QAccelerometerReading *)),WWCONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(this, SIGNAL(gyroscopeUpdated(QGyroscopeReading*)),&o,SLOT(onGyroscopeUpdated(QGyroscopeReading *)),WWCONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
}



void SensorInput::unHookSignals(QObject &o){
	if(0!=source){
		if(!disconnect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),&o,SLOT(onPositionUpdated(QGeoPositionInfo)))){
			qWarning()<<"ERROR: Could not disconnect";
		}
	}
	if(!disconnect(this, SIGNAL(compassUpdated(QCompassReading*)),&o,SLOT(onCompassUpdated(QCompassReading *)))){
		qWarning()<<"ERROR: Could not disconnect";
	}
	if(!disconnect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)),&o,SLOT(onAccelerometerUpdated(QAccelerometerReading *)))){
		qWarning()<<"ERROR: Could not disconnect";
	}
	if(!disconnect(this, SIGNAL(gyroscopeUpdated(QGyroscopeReading*)),&o,SLOT(onGyroscopeUpdated(QGyroscopeReading *)))){
		qWarning()<<"ERROR: Could not disconnect";
	}
}







///////////////////// Filters


bool SensorInput::filter(QCompassReading *reading){
	lastCompassSmoothValue=lastCompassSmoothValue*0.95+reading->azimuth()*0.05;
	const qreal minDelta=1.0;
	if(lastCompassSmoothValue-minDelta > lastCompassFilterValue || lastCompassSmoothValue+minDelta < lastCompassFilterValue ){
		lastCompassFilterValue=lastCompassSmoothValue;
		return true;
	}
	return false;
}
