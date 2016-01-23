#ifndef STATUSMESSAGE_HPP
#define STATUSMESSAGE_HPP

#include <QDataStream>
#include <QVector3D>
#include <QGeoCoordinate>
#include <QCompassReading>

class StatusMessage{
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

	public:
		StatusMessage(
				const bool ok=false
				, const qint64 timestamp=0
				, const QGeoCoordinate gps=QGeoCoordinate()
				, const QVector3D slam=QVector3D()
				, const QVector3D gyroscope=QVector3D()
				, const QVector3D accellerometer=QVector3D()
				, const qreal compassAzimuth=0
				, const qreal compassAccuracy=0
				, const qint32 temperature=0
				);

		StatusMessage(QDataStream &);

};


QDataStream &operator<<(QDataStream &, const StatusMessage &);
QDataStream &operator>>(QDataStream &, StatusMessage &);

#endif // STATUSMESSAGE_HPP
