#include "StatusMessage.hpp"

StatusMessage::StatusMessage(
		const qint64 timestamp
		, const QVector3D gps
		, const QVector3D slam
		, const QVector3D gyroscope
		, const QVector3D accellerometer
		, const qint32 compass
		, const qint32 temperature
		):
	timestamp(timestamp)
  , gps(gps)
  , slam(slam)
  , gyroscope(gyroscope)
  , accellerometer(accellerometer)
  ,compass(compass)
  ,temperature(temperature)
{

}



QDataStream& StatusMessage::operator<<( QDataStream&ds){
	ds >> timestamp;
	ds >> gps;
	ds >> slam;
	ds >> gyroscope;
	ds >> accellerometer;
	ds >> compass;
	ds >> temperature;
	return ds;
}

QDataStream& StatusMessage::operator>> (QDataStream &ds){
	ds << timestamp;
	ds << gps;
	ds << slam;
	ds << gyroscope;
	ds << accellerometer;
	ds << compass;
	ds << temperature;
	return ds;
}
