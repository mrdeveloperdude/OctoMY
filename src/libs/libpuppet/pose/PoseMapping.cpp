#include "PoseMapping.hpp"

#include  "Pose.hpp"

#include "uptime/MethodGate.hpp"

#include <QDataStream>
#include <QDebug>
#include <QVariantMap>

PoseMapping::PoseMapping(quint32 size)
	: mMapping(static_cast<int>(size))
	, mNames(static_cast<int>(size))
{
	OC_METHODGATE();
	for(quint32 i=0; i<size; ++i) {
		mMapping[i]=i;
		mNames[i]="";
	}
}


PoseMapping::~PoseMapping()
{
	OC_METHODGATE();

}


void PoseMapping::setMapping(quint32 from, quint32 to, bool swap)
{
	OC_METHODGATE();
	const quint32 l=mMapping.size();
	if(l<=from) {
		return;
	}
	if(swap) {
		quint32 oldFrom = 0;
		for(quint32 oldTo:mMapping) {
			if(to == oldTo) {
				break;
			}
			oldFrom++;
		}
		mMapping[oldFrom] = mMapping[from];
	}
	mMapping[from] = qMin(to,l);
}


void PoseMapping::setName(quint32 from, QString name)
{
	OC_METHODGATE();
	const quint32 l=mNames.size();
	if(l<=from) {
		return;
	}
	mNames[from]=name;
}


QString PoseMapping::name(quint32 from) const
{
	OC_METHODGATE();
	const quint32 l=mNames.size();
	if(l<=from) {
		return "";
	}
	return mNames[from];
}


quint32 PoseMapping::size() const
{
	OC_METHODGATE();
	return mMapping.size();
}


quint32 PoseMapping::map(quint32 from) const
{
	OC_METHODGATE();
	const quint32 l=mMapping.size();
	if(l<=from) {
		return 0;
	}
	return mMapping[from];
}


qreal PoseMapping::value(Pose &pose, quint32 from) const
{
	OC_METHODGATE();
	const quint32 to=map(from);
	if(to>pose.size()) {
		return 0.0;
	}
	return pose.value(to);
}


QString PoseMapping::toString() const
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	quint32 sz=static_cast<quint32>(mMapping.size());
	if(sz==nusz) {
		if(mDebug){
			qDebug() << "SAME SIZE SKIPPING: " << sz;
		}
		return;
	}
	if(nusz<sz) {
		if(mDebug){
			qDebug() << "SMALLER SIZE FIXING: " << sz;
		}
		for(quint32 i=0; i<sz; ++i) {
			const int ii=static_cast<int>(i);
			if(mMapping[ii]>=nusz) {
				mMapping[ii]=nusz-1;
			}
		}
	}
	const int nuszi=static_cast<int>(nusz);
	mMapping.resize(nuszi);
	mNames.resize(nuszi);
	if(nusz>sz) {
		if(mDebug){
			qDebug() << "BIGGER SIZE GENERATING: " << nusz;
		}
		for(quint32 i=sz; i<nusz; ++i) {
			const int ii=static_cast<int>(i);
			mMapping[ii]=i;
			mNames[ii]="";
		}
	}

}


QDataStream &PoseMapping::receive(QDataStream &ds)
{
	OC_METHODGATE();
	QVector<quint32> mapping;
	ds >> mapping;
	mMapping=mapping;
	return ds;
}


QDataStream &PoseMapping::send(QDataStream &ds) const
{
	OC_METHODGATE();
	ds << mMapping;
	return ds;
}


QVariantList PoseMapping::toMap() const
{
	OC_METHODGATE();
	QVariantList list;
	quint32 from=0;
	for(quint32 to:mMapping) {
		QString name = mNames[from];
		QVariantMap entry;
		entry["name"] = name;
		entry["to"] = to;
		list << entry;
		from++;
	}
	return list;
}


void PoseMapping::fromMap( QVariantList map)
{
	OC_METHODGATE();
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


void PoseMapping::set(const PoseMapping &other)
{
	OC_METHODGATE();
	mMapping=other.mMapping;
	mNames=other.mNames;
}


QDataStream &operator>>(QDataStream &ds, PoseMapping &p)
{
	OC_FUNCTIONGATE();
	p.receive(ds);
	return ds;
}


QDataStream &operator<<(QDataStream &ds, const PoseMapping &p)
{
	OC_FUNCTIONGATE();
	p.send(ds);
	return ds;
}
