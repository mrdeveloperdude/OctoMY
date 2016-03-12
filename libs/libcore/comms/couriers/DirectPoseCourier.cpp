#include "DirectPoseCourier.hpp"

DirectPoseCourier::DirectPoseCourier(QObject *parent)
	: Courier(parent)
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

quint64 DirectPoseCourier::sendingOpportunity(QDataStream &ds, quint32 availableBytes){
	const quint64 sz=pose.size();
	if(availableBytes>=sz){
		ds<<pose;
		lastTX=QDateTime::currentMSecsSinceEpoch();
		qDebug()<<"DIRECT POSE COURIER: sent "<<sz<<" bytes ("<<pose.toString()<<")";
		return sz;
	}
	return 0;
}
