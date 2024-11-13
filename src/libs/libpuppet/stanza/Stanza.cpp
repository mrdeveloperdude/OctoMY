#include "Stanza.hpp"

#include "lobe/Lobe.hpp"
#include "lobe/LobeFactory.hpp"
#include "pose/PoseMapping.hpp"
#include "uptime/MethodGate.hpp"

Stanza::Stanza()
	: mMapping(QSharedPointer<PoseMapping>::create())
{
	OC_METHODGATE();
}

Stanza::Stanza(const QVariantMap map)
	: mMapping(QSharedPointer<PoseMapping>::create())
{
	OC_METHODGATE();
	fromVariantMap(map);
}


QString Stanza::id() const{
	OC_METHODGATE();
	return mID;
}


void Stanza::setID(const QString &id){
	OC_METHODGATE();
	mID = id;
}


QSharedPointer<Lobe> Stanza::lobe(){
	OC_METHODGATE();
	return mLobe;
}


QSharedPointer<PoseMapping> Stanza::mapping(){
	OC_METHODGATE();
	return mMapping;
}


void Stanza::setLobe(QSharedPointer<Lobe> lobe){
	OC_METHODGATE();
	mLobe = lobe;
}


void Stanza::setMapping(QSharedPointer<PoseMapping> mapping){
	OC_METHODGATE();
	mMapping=mapping;	
}

QVariantMap Stanza::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["id"] = mID;
	if(!mLobe.isNull()){
		map["lobe"] = mLobe->lobeTitle();
	}
	if(!mMapping.isNull()){
		map["mapping"] = mMapping->toList();
	}
	return map;
}


void Stanza::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	mID = map["id"].toString();
	LobeFactory lf;
	mLobe = lf.createLobe(map["lobe"].toString());
	mMapping->fromList(map["mapping"].toList());
}
/*

QString Stanza::toString()
{
	OC_METHODGATE();
	return QString() //mKey.toString()
		+"  id: "+id()
		+", pins:"+mPins.join(";")
		+", trusts:"+QStringList(mTrusts.begin(), mTrusts.end()).join(";");
}


QMap<QString, QString> Stanza::toMap()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	map["id"]=id();
	map["pins"]=mPins.join(";");
	map["trusts"]=QStringList(mTrusts.begin(), mTrusts.end()).join(";");
	return map;
}
*/
