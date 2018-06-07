#ifndef SENSORINPUT_HPP
#define SENSORINPUT_HPP

#include <QObject>

#include <QGeoPositionInfoSource>

#include <QCompassFilter>

class QSensor;
class QAccelerometer;
class QCompass;
class QCompassReading;
class QAccelerometerReading;
class QGyroscope;
class QGyroscopeReading;
class QSensor;

class SensorInput : public QObject,  public QCompassFilter
{
	Q_OBJECT
private:

	QList<QSensor*> m_availableSensors;
	QGeoPositionInfoSource *source;
	QAccelerometer *accelerometer;
	QCompass*compass;
	QGyroscope *gyroscope;

private:

	qreal lastCompassFilterValue;
	qreal lastCompassSmoothValue;


public:
	explicit SensorInput(QObject *parent = nullptr);


	void hookSignals(QObject &);
	void unHookSignals(QObject &);


	QString toSpecStanzas(QString space="");

public:
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	bool filter(QCompassReading *reading) Q_DECL_OVERRIDE;
#pragma GCC diagnostic warning "-Woverloaded-virtual"

private:

	void load();

private slots:

	void onAccelerometerReadingChanged() ;
	void onCompassReadingChanged() ;
	void onGyroscopeReadingChanged() ;

signals:

	void compassUpdated(QCompassReading *);
	void accelerometerUpdated(QAccelerometerReading *);
	void gyroscopeUpdated(QGyroscopeReading *);



};


#endif // SENSORINPUT_HPP
