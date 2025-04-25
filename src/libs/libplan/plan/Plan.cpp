#include "Plan.hpp"

#include "uptime/MethodGate.hpp"
#include "utility/random/Random.hpp"
#include "utility/string/String.hpp"
#include "utility/time/HumanTime.hpp"

#include <QRegularExpression>

Plan::Plan(const QVariantMap map)
	: mID ( map["id"].toString())
	, mName( map["name"].toString() )
	, mContent( map["content"].toString() )
	, mCreatedMS( utility::time::variantToMs(map["createdMS"]) )
	, mLastUpdatedMS( utility::time::variantToMs(map["lastUpdatedMS"]) )
{
	OC_METHODGATE();
	//qDebug()<<"CREATE Plan(map, isPublic)"<<map<<isPublic;
}


Plan::Plan()
	: mID{utility::random::randomString(10)}
	, mName("Unnamed")
	, mCreatedMS(0)
	, mLastUpdatedMS(0)
{
	OC_METHODGATE();
	updateTimestamps();
	qDebug() << "CREATE Plan()" << mID << mName << mCreatedMS;
}


Plan::~Plan()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING node Plan with ID "<<id();
}


void Plan::updateTimestamps(){
	OC_METHODGATE();
	mLastUpdatedMS = utility::time::currentMsecsSinceEpoch<quint64>();
	if(0 == mCreatedMS){
		mCreatedMS = mLastUpdatedMS;
	}
}


bool Plan::update(const QVariantMap map, bool trustedSource)
{
	OC_METHODGATE();
	// TODO: Implement
	//updateTimestamps();
	fromVariantMap(map);
	return true;
}


QString Plan::id() //NOTE no const please
{
	OC_METHODGATE();
	return mID;
}

quint64 Plan::createdMS() const
{
	OC_METHODGATE();
	return mCreatedMS;
}

quint64 Plan::lastUpdatedMS() const
{
	OC_METHODGATE();
	return mLastUpdatedMS;
}


void Plan::setContent(const QString &content){
	OC_METHODGATE();
	mContent = content;
	updateTimestamps();
}


QString Plan::content() const
{
	OC_METHODGATE();
	return mContent;
}


void Plan::setName(const QString &name){
	OC_METHODGATE();
	mName = name;
	updateTimestamps();
}


QString Plan::name() const
{
	OC_METHODGATE();
	return mName;
}


QString Plan::identifier() const
{
	OC_METHODGATE();
	if(!mName.isEmpty()) {
		return mName;
	}
	const QString left(mID.left(8));
	return QStringLiteral("PLAN-")+left;
}


QPixmap Plan::previewPixmap(){
	OC_METHODGATE();
	return QPixmap(":/icons/plan/mandate.svg");
};



QVariantMap Plan::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["id"]=mID;
	map["name"]=mName;
	map["content"]=mContent;
	return map;
}


void Plan::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	//mKey=Key( map["key"].toMap(), true);
	mID=( map["id"].toString() );
	mName=( map["name"].toString() );
	mContent=( map["content"].toString() );
	mCreatedMS=( utility::time::variantToMs(map["createdMS"]) );
	mLastUpdatedMS=( utility::time::variantToMs(map["lastUpdatedMS"]) );
}


QString Plan::toString()
{
	OC_METHODGATE();
	return QString()
		+ "  name: " + mName
		+ ", id: " + mID
		+ ", content: " + mContent;
		+ ", lastUpdated: " + utility::string::formattedDateFromMS(mLastUpdatedMS);
		+ ", created: " + utility::string::formattedDateFromMS(mCreatedMS);
}


QMap<QString, QString> Plan::toMap()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	map["name"] =          mName;
	map["id"] =            mID;
	map["content"] =       mContent;
	map["createdMS"] =     utility::string::formattedDateFromMS(mCreatedMS);
	map["lastUpdatedMS"] = utility::string::formattedDateFromMS(mLastUpdatedMS);
	return map;
}



bool Plan::operator==(Plan &o)
{
	OC_METHODGATE();
	return id() == o.id();
}


bool Plan::operator!=(Plan &o)
{
	OC_METHODGATE();
	return (! (o == *this));
}


const QDebug &operator<<(QDebug &d, Plan &plan)
{
	OC_FUNCTIONGATE();
	d.nospace() << "Plan("<<plan.toString()<<")";
	return d.maybeSpace();
}
