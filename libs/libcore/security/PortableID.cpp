#include "PortableID.hpp"

#include <QStringList>
#include <QDateTime>



PortableID::PortableID()
	: mBirthDate(0)
	, mType(DiscoveryType::TYPE_UNKNOWN)
{

}

#include "utility/Utility.hpp"
PortableID::PortableID(QVariantMap &data)
	: mName(data["name"].toString())
	, mGender(data["gender"].toString())
	, mID(data["key"].toMap()["id"].toString())
	, mBirthDate(QDateTime::fromString(data["createDate"].toString()).toMSecsSinceEpoch())
	, mType(DiscoveryTypeFromString(data["type"].toString()))
{

}

PortableID::~PortableID()
{

}



void PortableID::setName(QString name){
	mName=name;
}

void PortableID::setGender(QString gender){
	mGender=gender;
}

void PortableID::setID(QString id){
	mID=id;
}


void PortableID::setBirthDate(quint64 birthDate){
	mBirthDate=birthDate;
}


void PortableID::setType(DiscoveryType type){
	mType=type;
}




QString PortableID::name() const{
	return mName;
}
QString PortableID::gender() const{
	return mGender;
}
QString PortableID::id() const{
	return mID;
}


quint64 PortableID::birthDate() const{
	return mBirthDate;
}


bool PortableID::fromPortableString(QString s){
	QStringList parts = s.split(QRegExp ("(\\.)"));
	if(parts.size()!=4){
		return false;
	}
	mName=parts.at(0).trimmed();
	mGender=parts.at(1).trimmed();
	mID=parts.at(2).trimmed();
	mBirthDate=parts.at(3).trimmed().toInt();
	return true;
}

QString PortableID::toPortableString(){
	return mName + "." +mGender + "." +mID+ "." +QString::number(mBirthDate);
}



DiscoveryType PortableID::type() const{
	return mType;
}
