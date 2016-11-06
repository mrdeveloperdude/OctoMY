#ifndef SENSORSMESSAGE_HPP
#define SENSORSMESSAGE_HPP

#include <QDataStream>
#include <QVector3D>
#include <QVector2D>
#include <QGeoCoordinate>
#include <QCompassReading>
#include <QColor>

class SensorsMessage{
	private:
		qint64 sz=-1;
	public:
		bool ok;
		qint64 timestamp;
		QGeoCoordinate gps;
		QVector3D slam;
		QVector3D gyroscope;
		QVector3D accellerometer;
		qreal compassAzimuth;
		qreal compassAccuracy;
		qint32 temperature;
		QVector2D touch;
		QColor color;

	public:
		SensorsMessage(
				const bool ok=false
				, const qint64 timestamp=0
				, const QGeoCoordinate gps=QGeoCoordinate()
				, const QVector3D slam=QVector3D()
				, const QVector3D gyroscope=QVector3D()
				, const QVector3D accellerometer=QVector3D()
				, const qreal compassAzimuth=0
				, const qreal compassAccuracy=0
				, const qint32 temperature=0
				, const QVector2D touch=QVector2D(0,0)
				, const QColor color=Qt::black
				);

		SensorsMessage(QDataStream &);

		qint64 size();

};


QDataStream &operator<<(QDataStream &, const SensorsMessage &);
QDataStream &operator>>(QDataStream &, SensorsMessage &);

#endif // SENSORSMESSAGE_HPP
