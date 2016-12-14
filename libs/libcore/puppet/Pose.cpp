#include "Pose.hpp"

#include "comms/messages/MessageType.hpp"



#include <QDataStream>
#include <QDebug>

//We support up to 32 servos per agent for now
const quint32 Pose::MAX_SIZE=32;

Pose::Pose(quint32 size)
	: mValues(qMin(MAX_SIZE,size))
{
	if((quint32)mValues.size()!=size) {
		qWarning()<<"ERROR: wanted Pose size "<< size << " was beyond the limit "<<MAX_SIZE;
	}
}


quint64 Pose::size()
{
	return mValues.size();
}


QDataStream &Pose::receive(QDataStream &ds)
{
	QVector<qreal> vec;
	ds >> vec;
	mValues=vec;
	return ds;
}

QDataStream &Pose::send(QDataStream &ds) const
{
	ds << mValues;
	return ds;
}



QDataStream &operator>>(QDataStream &ds, Pose &p)
{
	p.receive(ds);
	return ds;
}




QDataStream &operator<<(QDataStream &ds, const Pose &p)
{
	p.send(ds);
	return ds;
}




QString Pose::toString() const
{
	QString out="Pose( ";
	for(qreal val:mValues) {
		out+=QString::number(val)+", ";
	}
	out+=" )";
	return out;
}
