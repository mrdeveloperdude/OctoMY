#ifndef SENSORINPUT_HPP
#define SENSORINPUT_HPP

#include <QObject>

#include <QGeoPositionInfoSource>
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <QCompassFilter>
#pragma GCC diagnostic warning "-Woverloaded-virtual"

class QSensor;
class QAccelerometer;
class QCompass;
class QCompassReading;
class QAccelerometerReading;
class QGyroscope;
class QGyroscopeReading;
class QSensor;

/*!
 * \brief The SensorInput class wraps all sensor I/O that is available
 * through Qt5 API in one manager class.
 * TODO: There are other sensors to incorporate:

	"QOrientationSensor"
	"QLightSensor"
	"QMagnetometer"
	"QAmbientLightSensor"
	"QRotationSensor"
	"QGyroscope"
	"QTiltSensor"
	"QProximitySensor"
*/

class SensorInput : public QObject,  public QCompassFilter
{
	Q_OBJECT
private:

	QList<QSensor*> mAvailableSensors;
	QGeoPositionInfoSource *mGeoPositionSource;
	QAccelerometer *mAccelerometer;
	QCompass *mCompass;
	QGyroscope *mGyroscope;

private:

	qreal lastCompassFilterValue;
	qreal lastCompassSmoothValue;


public:
	explicit SensorInput(QObject *parent = nullptr);


	void hookSignals(QObject &object, bool hook=true);

	QString toSpecStanzas(QString space="");

public:

	bool filter(QCompassReading *reading) Q_DECL_OVERRIDE;


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
