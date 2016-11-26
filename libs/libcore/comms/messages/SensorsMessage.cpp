#include "SensorsMessage.hpp"

#include <QDebug>

SensorsMessage::SensorsMessage(
	const bool ok
	, const qint64 timestamp
	, const QGeoCoordinate  gps
	, const QVector3D slam
	, const QVector3D gyroscope
	, const QVector3D accellerometer
	, const qreal compassAzimuth
	, const qreal compassAccuracy
	, const qint32 temperature
	, const QVector2D touch
	, const QColor color
)
	: ok(ok)
	, timestamp(timestamp)
	, gps(gps)
	, slam(slam)
	, gyroscope(gyroscope)
	, accellerometer(accellerometer)
	, compassAzimuth(compassAzimuth)
	, compassAccuracy(compassAccuracy)
	, temperature(temperature)
	, touch(touch)
	, color(color)
{

}

SensorsMessage::SensorsMessage(QDataStream &ds)
{
	ds >> *this;
}


qint64 SensorsMessage::size()
{
	if(sz<0) {
		QByteArray ba;
		{
			QDataStream ds(&ba,QIODevice::WriteOnly);
			ds<<this;
		}
		sz=ba.size();
		//qDebug()<<"Counted bytesize of StatusMessage to be "<<sz;
	}
	return sz;

}

#include "MessageType.hpp"

QDataStream &operator>>(QDataStream &ds, SensorsMessage &sm)
{
	qint32 mt=INVALID;
	ds >> mt;
	sm.ok=(STATUS==mt);
	if(sm.ok) {
		ds >> sm.timestamp;
		ds >> sm.gps;
		ds >> sm.slam;
		ds >> sm.gyroscope;
		ds >> sm.accellerometer;
		ds >> sm.compassAzimuth;
		ds >> sm.compassAccuracy;
		ds >> sm.temperature;
		ds >> sm.touch;
	}
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const SensorsMessage &sm)
{
	ds << STATUS;
	ds << sm.timestamp;
	ds << sm.gps;
	ds << sm.slam;
	ds << sm.gyroscope;
	ds << sm.accellerometer;
	ds << sm.compassAzimuth;
	ds << sm.compassAccuracy;
	ds << sm.temperature;
	ds << sm.touch;
	return ds;
}

