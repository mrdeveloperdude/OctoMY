#ifndef REMOTE_HPP
#define REMOTE_HPP

#include <QObject>

#include <QGeoPositionInfoSource>

#include <QCompassFilter>

class QSensor;
class QAccelerometer;
class QCompass;
class QCompassReading;
class QAccelerometerReading;

class Remote : public QObject,  public QCompassFilter{
		Q_OBJECT
	private:

		QGeoPositionInfoSource *source;
		QAccelerometer *accelerometer;
		QCompass*compass;

	private:

		qreal lastCompassFilterValue;
		qreal lastCompassSmoothValue;


	public:
		explicit Remote(QObject *parent = 0);


		void hookSignals(QObject *o);
		void unHookSignals(QObject *o);

	protected:
		bool filter(QCompassReading *reading);

	private:

		QByteArray getFirstSensorForType(QByteArray type);

	private slots:

		void onAccelerometerReadingChanged() ;
		void onCompassReadingChanged() ;

	signals:

		void compassUpdated(QCompassReading *);
		void accelerometerUpdated(QAccelerometerReading *);



};

#endif // REMOTE_HPP
