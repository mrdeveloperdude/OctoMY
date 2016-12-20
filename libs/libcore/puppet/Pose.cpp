#include "Pose.hpp"

#include "comms/messages/MessageType.hpp"

#include "../libutil/utility/Standard.hpp"

#include <QDataStream>
#include <QDebug>

//We support up to 32 servos per agent for now
const quint32 Pose::MAX_SIZE=32;

Pose::Pose(quint32 size)
	: mValues(qMin(MAX_SIZE,size))
{
	OC_METHODGATE();
	if((quint32)mValues.size()!=size) {
		qWarning()<<"ERROR: wanted Pose size "<< size << " was beyond the limit "<<MAX_SIZE;
	}
}


quint64 Pose::size() const
{
	OC_METHODGATE();
	return mValues.size();
}

qreal Pose::value(quint32 index) const
{
	OC_METHODGATE();
	if( index>=mValues.size()) {
		return 0.0;
	}
	return mValues[index];
}

void Pose::setValue(quint32 index, qreal value)
{
	OC_METHODGATE();
	if( index>=mValues.size()) {
		return;
	}
	mValues[index]=value;
}

void Pose::mix(const Pose &other,qreal alpha)
{
	OC_METHODGATE();
	const quint64 sz=qMin(mValues.size(), other.mValues.size());
	const qreal ialpha=1.0-alpha;
	for(quint64  i=0; i<sz; ++i) {
		mValues[i]=mValues[i]*ialpha+other.mValues[i]*alpha;
	}
}

QDataStream &Pose::receive(QDataStream &ds)
{
	OC_METHODGATE();
	QVector<qreal> vec;
	ds >> vec;
	mValues=vec;
	return ds;
}

QDataStream &Pose::send(QDataStream &ds) const
{
	OC_METHODGATE();
	ds << mValues;
	return ds;
}



QDataStream &operator>>(QDataStream &ds, Pose &p)
{
	OC_FUNCTIONGATE();
	p.receive(ds);
	return ds;
}




QDataStream &operator<<(QDataStream &ds, const Pose &p)
{
	OC_FUNCTIONGATE();
	p.send(ds);
	return ds;
}




QString Pose::toString() const
{
	OC_METHODGATE();
	QString out="Pose( ";
	for(qreal val:mValues) {
		out+=QString::number(val)+", ";
	}
	out+=" )";
	return out;
}
