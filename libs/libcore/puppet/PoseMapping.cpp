#include "PoseMapping.hpp"

#include  "Pose.hpp"

#include <QDataStream>
#include <QDebug>

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
	if(swap) {
		quint32 oldFrom=0;
		for(quint32 oldTo:mMapping) {
			if(to==oldTo) {
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
		out+=QString::number(index)+"="+QString::number(val)+(!name.isEmpty()?("("+name+")"):"")+", ";
		index++;
	}
	out+=" )";
	return out;
}


void PoseMapping::resize(quint32 nusz)
{
	quint32 sz=mMapping.size();
	if(sz==nusz) {
		qDebug()<<"SAME SIZE SKIPPING: "<<sz;
		return;
	}
	if(nusz<sz) {
		qDebug()<<"SMALLER SIZE FIXING: "<<sz;
		for(quint32 i=0; i<sz; ++i) {
			if(mMapping[i]>=nusz) {
				mMapping[i]=nusz-1;
			}
		}
	}
	mMapping.resize(nusz);
	mNames.resize(nusz);
	if(nusz>sz) {
		qDebug()<<"BIGGER SIZE GENERATING: "<<nusz;
		for(quint32 i=sz; i<nusz; ++i) {
			mMapping[i]=i;
			mNames[i]="";
		}
	}

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

#include <QVariantMap>

QVariantList PoseMapping::toMap() const
{
	QVariantList list;
	quint32 from=0;
	for(quint32 to:mMapping) {
		QString name=mNames[from];
		QVariantMap entry;
		entry["name"]=name;
		entry["to"]=to;
		list<<entry;
		from++;
	}
	return list;
}
void PoseMapping::fromMap( QVariantList map)
{
	QVector<quint32> tos;
	QVector<QString> names;
	quint32 ct=0;
	for(QVariant ventry:map) {
		if (ventry.canConvert<QVariantMap>()) {
			QVariantMap entry=ventry.value<QVariantMap>();
			QString name="";
			quint32 to=0;
			if(entry.contains("name")) {
				name=entry["name"].toString();
			}
			if(entry.contains("to")) {
				to=entry["to"].toUInt();
			}
			tos<<to;
			names<<name;
			ct++;
		}
	}
	mMapping.resize(ct);
	mNames.resize(ct);
	quint32 from=0;
	for(quint32 to:tos) {
		QString name=names[from];
		mMapping[from]=qMin(to,ct-1);
		mNames[from]=name;
		from++;
	}
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
