#include "StatusMessage.hpp"

StatusMessage::StatusMessage(
		const qint64 timestamp
		, const QGeoCoordinate  gps
		, const QVector3D slam
		, const QVector3D gyroscope
		, const QVector3D accellerometer
		, const qreal compassAzimuth
		, const qreal compassAccuracy
		, const qint32 temperature
		):
	timestamp(timestamp)
  , gps(gps)
  , slam(slam)
  , gyroscope(gyroscope)
  , accellerometer(accellerometer)
  , compassAzimuth(compassAzimuth)
  , compassAccuracy(compassAccuracy)
  , temperature(temperature)
{

}



QDataStream &operator>>(QDataStream &ds, StatusMessage &sm){
	ds >> sm.timestamp;
	ds >> sm.gps;
	ds >> sm.slam;
	ds >> sm.gyroscope;
	ds >> sm.accellerometer;
	ds >> sm.compassAzimuth;
	ds >> sm.compassAccuracy;
	ds >> sm.temperature;
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const StatusMessage &sm){
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

