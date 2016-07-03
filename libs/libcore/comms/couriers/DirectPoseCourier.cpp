#include "DirectPoseCourier.hpp"

#include <QDateTime>

DirectPoseCourier::DirectPoseCourier(QObject *parent)
	: Courier("Direct Pose", (Courier::FIRST_USER_ID+1), parent)
	, lastRX(0)
	, lastTX(0)
{

}


void DirectPoseCourier::setPose(Pose &newPose){
	pose=newPose;
	lastRX=QDateTime::currentMSecsSinceEpoch();
}

CourierMandate DirectPoseCourier::mandate() {
	return CourierMandate(Pose::MAX_SIZE,1,100,lastRX>=lastTX);
}

quint16 DirectPoseCourier::sendingOpportunity(QDataStream &ds){
	const quint64 sz=pose.size();
	if(mandate().payloadSize>=sz){
		ds<<pose;
		lastTX=QDateTime::currentMSecsSinceEpoch();
		qDebug()<<"DIRECT POSE COURIER: sent "<<sz<<" bytes ("<<pose.toString()<<")";
		return sz;
	}
	return 0;
}
