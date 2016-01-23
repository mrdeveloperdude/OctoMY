#include "StatusMessage.hpp"

StatusMessage::StatusMessage(
		const bool ok
		, const qint64 timestamp
		, const QGeoCoordinate  gps
		, const QVector3D slam
		, const QVector3D gyroscope
		, const QVector3D accellerometer
		, const qreal compassAzimuth
		, const qreal compassAccuracy
		, const qint32 temperature
		):
	ok(ok)
  , timestamp(timestamp)
  , gps(gps)
  , slam(slam)
  , gyroscope(gyroscope)
  , accellerometer(accellerometer)
  , compassAzimuth(compassAzimuth)
  , compassAccuracy(compassAccuracy)
  , temperature(temperature)
{

}

StatusMessage::StatusMessage(QDataStream &ds){
	ds >> *this;
}


#include "MessageType.hpp"

QDataStream &operator>>(QDataStream &ds, StatusMessage &sm){
	qint32 mt=INVALID;
	ds >> mt;
	sm.ok=(STATUS==mt);
	if(sm.ok){
		ds >> sm.timestamp;
		ds >> sm.gps;
		ds >> sm.slam;
		ds >> sm.gyroscope;
		ds >> sm.accellerometer;
		ds >> sm.compassAzimuth;
		ds >> sm.compassAccuracy;
		ds >> sm.temperature;
	}
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const StatusMessage &sm){
	ds << STATUS;
	ds << sm.timestamp;
	ds << sm.gps;
	ds << sm.slam;
	ds << sm.gyroscope;
	ds << sm.accellerometer;
	ds << sm.compassAzimuth;
	ds << sm.compassAccuracy;
	ds << sm.temperature;
	return ds;
}

