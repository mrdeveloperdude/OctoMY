#include "Remote.hpp"
#include "Standard.hpp"
#include <QDebug>
#include <QSensor>
#include <QAccelerometer>
#include <QCompass>

Remote::Remote(QObject *parent):
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
}


void Remote::onAccelerometerReadingChanged() {
	if(0!=accelerometer){
		QAccelerometerReading *r=accelerometer->reading();
		if(0!=r){
			emit accelerometerUpdated(r);
		}
	}
}

void Remote::onCompassReadingChanged() {
	if(0!=compass){
		QCompassReading *r=compass->reading();
		if(0!=r){
			emit compassUpdated(r);
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

//TODO: select "best" sensor for type based on a scoring model. For now we assume the device has exactlty one sensor of each type and we simply return the first one we find that works
QByteArray Remote::getFirstSensorForType(QByteArray type){
	QSensor* sensor=0;
	foreach (const QByteArray &identifier, QSensor::sensorsForType(type)) {
		if(0!=sensor){
			delete sensor;
			sensor=0;
		}
		qDebug() << "Found sensor ID" << identifier;
		// Don't put in sensors we can't connect to
		sensor = new QSensor(type, this);
		if(0!=sensor){
			sensor->setIdentifier(identifier);
			if (!sensor->connectToBackend()) {
				qDebug() << "Skipping invalid sensor: " << identifier;
				continue;
			}
			return identifier;
		}
	}
	return "";
}

void Remote::hookSignals(QObject *o){
	if(0!=o){
		if(0!=source){
			if(!connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),o,SLOT(onPositionUpdated(QGeoPositionInfo)),WWCONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
		}

		if(!connect(this, SIGNAL(compassUpdated(QCompassReading*)),o,SLOT(onCompassUpdated(QCompassReading *)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)),o,SLOT(onAccelerometerUpdated(QAccelerometerReading *)),WWCONTYPE)){
			qWarning()<<"ERROR: Could not connect";
		}

	}
	else{
		qWarning()<<"Missing object";
	}
}



void Remote::unHookSignals(QObject *o){
	if(0!=o){
		if(0!=source){
			if(!disconnect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),o,SLOT(onPositionUpdated(QGeoPositionInfo)))){
				qWarning()<<"ERROR: Could not disconnect";
			}
		}
		if(!disconnect(this, SIGNAL(compassUpdated(QCompassReading*)),o,SLOT(onCompassUpdated(QCompassReading *)))){
			qWarning()<<"ERROR: Could not disconnect";
		}
		if(!disconnect(this, SIGNAL(accelerometerUpdated(QAccelerometerReading*)),o,SLOT(onAccelerometerUpdated(QAccelerometerReading *)))){
			qWarning()<<"ERROR: Could not disconnect";
		}

	}
	else{
		qWarning()<<"Missing object";
	}

}







///////////////////// Filters


bool Remote::filter(QCompassReading *reading){
	lastCompassSmoothValue=lastCompassSmoothValue*0.95+reading->azimuth()*0.05;
	const qreal minDelta=1.0;
	if(lastCompassSmoothValue-minDelta > lastCompassFilterValue || lastCompassSmoothValue+minDelta < lastCompassFilterValue ){
		lastCompassFilterValue=lastCompassSmoothValue;
		return true;
	}
	return false;
}
