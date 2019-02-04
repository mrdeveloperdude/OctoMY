/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "Pose.hpp"

#include "comms/messages/MessageType.hpp"

#include "uptime/MethodGate.hpp"


#include <QDataStream>
#include <QDebug>

// We support up to 32 servos per agent for now
const quint32 Pose::MAX_SIZE=32;

Pose::Pose(quint32 size)
	: mValues(static_cast<int>(std::min(MAX_SIZE, size)))
{
	OC_METHODGATE();
	if(static_cast<quint32>(mValues.size())!=size) {
		qWarning()<<"ERROR: wanted Pose size "<< size << " was beyond the limit "<<MAX_SIZE;
	}
}


quint64 Pose::size() const
{
	OC_METHODGATE();
	return static_cast<quint64>(mValues.size());
}

qreal Pose::value(quint32 index) const
{
	OC_METHODGATE();
	if( index>= static_cast<quint32>(mValues.size())) {
		return 0.0;
	}
	return mValues[static_cast<int>(index)];
}

void Pose::setValue(quint32 index, qreal value)
{
	OC_METHODGATE();
	if( index>= static_cast<quint32>(mValues.size())) {
		return;
	}
	mValues[static_cast<int>(index)]=value;
}



void Pose::setValues(QVector<qreal> values)
{
	OC_METHODGATE();
	const int sz=qMin(values.size(), mValues.size());
	for(int i=0; i<sz; ++i) {
		mValues[i]=values[i];
	}
}
void Pose::setValues(qreal *values, int n)
{
	OC_METHODGATE();
	const int sz=qMin(n, mValues.size());
	for(int i=0; i<sz; ++i) {
		mValues[i]=values[i];
	}
}

void Pose::mix(const Pose &other,qreal alpha)
{
	OC_METHODGATE();
	const int sz=qMin(mValues.size(), other.mValues.size());
	const qreal ialpha=1.0-alpha;
	for(int i=0; i<sz; ++i) {
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


QString ardumyActuatorValueToString (const Pose &v)
{
	OC_FUNCTIONGATE();
	return v.toString();
}
