#include "PoseMapping.hpp"

#include  "Pose.hpp"

#include <QDataStream>

PoseMapping::PoseMapping(quint32 size)
	: mMapping(size)
	, mNames(size)
{
	for(quint32 i=0; i<size; ++i) {
		mMapping[i]=i;
		mNames[i]="";
	}
}


PoseMapping::~PoseMapping()
{

}


void PoseMapping::setMapping(quint32 from, quint32 to, bool swap)
{
	const quint32 l=mMapping.size();
	if(l<=from) {
		return;
	}
	if(swap){
		quint32 oldFrom=0;
		for(quint32 oldTo:mMapping){
			if(to==oldTo){
				break;
			}
			oldFrom++;
		}
		mMapping[oldFrom]=mMapping[from];
	}
	mMapping[from]=qMin(to,l);
}

void PoseMapping::setName(quint32 from, QString name)
{
	const quint32 l=mNames.size();
	if(l<=from) {
		return;
	}
	mNames[from]=name;
}

QString PoseMapping::name(quint32 from) const
{
	const quint32 l=mNames.size();
	if(l<=from) {
		return "";
	}
	return mNames[from];
}
quint32 PoseMapping::size() const
{
	return mMapping.size();
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
	if(to>pose.size()) {
		return 0.0;
	}
	return pose.value(to);
}


QString PoseMapping::toString() const
{
	QString out="PoseMapping( ";
	quint32 index=0;
	for(quint32 val:mMapping) {
		QString name=mNames[index];
		out+=QString::number(index)+"="+QString::number(val)+(name.isEmpty()?("("+name+")"):"")+", ";
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



QDataStream &operator>>(QDataStream &ds, PoseMapping &p)
{
	p.receive(ds);
	return ds;
}


QDataStream &operator<<(QDataStream &ds, const PoseMapping &p)
{
	p.send(ds);
	return ds;
}
