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

class SensorInput : public QObject,  public QCompassFilter{
		Q_OBJECT
	private:

		QGeoPositionInfoSource *source;
		QAccelerometer *accelerometer;
		QCompass*compass;
		QGyroscope *gyroscope;

	private:

		qreal lastCompassFilterValue;
		qreal lastCompassSmoothValue;


	public:
		explicit SensorInput(QObject *parent = 0);


		void hookSignals(QObject &);
		void unHookSignals(QObject &);

	public:

		bool filter(QCompassReading *reading) override;


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
