#include "PoseMapping.hpp"

#include  "Pose.hpp"

#include <QDataStream>

PoseMapping::PoseMapping(quint32 size)
	: mMapping(size)
{
	for(quint32 i=0; i<size; ++i) {
		mMapping[i]=i;
	}
}


PoseMapping::~PoseMapping()
{

}


void PoseMapping::setMapping(quint32 from, quint32 to)
{
	const quint32 l=mMapping.size();
	if(l<=from) {
		return;
	}
	mMapping[from]=qMin(to,l);
}


quint32 PoseMapping::map(quint32 from) const
{
	const quint32 l=mMapping.size();
	if(l<=from) {
		return 0;
	}
	return mMapping[from];
}


qreal PoseMapping::value(Pose &pose, quint32 from) const
{
	const quint32 to=map(from);
	if(to>pose.size()){
		return 0.0;
	}
	return pose.value(to);
}


QString PoseMapping::toString() const
{
	QString out="PoseMapping( ";
	quint32 index=0;
	for(quint32 val:mMapping) {
		out+=QString::number(index)+"="+QString::number(val)+", ";
		index++;
	}
	out+=" )";
	return out;
}



QDataStream &PoseMapping::receive(QDataStream &ds)
{
	QVector<quint32> mapping;
	ds >> mapping;
	mMapping=mapping;
	return ds;
}


QDataStream &PoseMapping::send(QDataStream &ds) const
{
	ds << mMapping;
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
