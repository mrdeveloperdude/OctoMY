#include "StatusMessage.hpp"

StatusMessage::StatusMessage()
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
