#include "Pose.hpp"

#include "comms/messages/MessageType.hpp"



#include <QDataStream>
#include <QDebug>


QDataStream &operator>>(QDataStream &ds, Pose &p)
{
	/*
	qint32 mt=INVALID;
	ds >> mt;
	p.ok=(DIRECT_POSE==mt);
	if(p.ok){

	}
	else{
		qWarning()<<"ERROR: not a pose:"<<mt;
	}
	*/
	ds >> p.pos1;
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const Pose &p)
{
	ds << p.pos1;
	return ds;
}




QString Pose::toString() const
{
//	qDebug()<<"pos1="<<pos1<<"ok="<<ok;
	return QString("Pose(ok=")+(ok?"true":"false")+", pos1="+QString::number(pos1)+")";
}

