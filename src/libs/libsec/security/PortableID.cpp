#include "PortableID.hpp"
#include "utility/Utility.hpp"


#include <QStringList>
#include <QDateTime>



const QString PortableID::SEP(".");
const QString PortableID::SEP_RE(QRegularExpression::escape(SEP));
const QRegularExpression PortableID::sepRE("("+SEP_RE+")");
const QString PortableID::dateFMT("yyyy-MM-dd_hh:mm:ss:zzz");


PortableID::PortableID()
	: mBirthDate(0)
	, mType(DiscoveryType::TYPE_UNKNOWN)
{

}


PortableID::PortableID(QVariantMap &data)
	: mName(data["name"].toString())
	, mGender(data["gender"].toString())
	, mID(data["key"].toMap()["id"].toString())
	, mBirthDate(QDateTime::fromString(data["createDate"].toString(), dateFMT).toMSecsSinceEpoch())
	, mType(DiscoveryTypeFromString(data["type"].toString()))
{

}

PortableID::~PortableID()
{

}



void PortableID::setName(QString name)
{
	mName=name;
}

void PortableID::setGender(QString gender)
{
	mGender=gender;
}

void PortableID::setID(QString id)
{
	mID=id;
}


void PortableID::setBirthDate(quint64 birthDate)
{
	mBirthDate=birthDate;
}


void PortableID::setType(DiscoveryType type)
{
	mType=type;
}




QString PortableID::name() const
{
	return mName;
}
QString PortableID::gender() const
{
	return mGender;
}
QString PortableID::id() const
{
	return mID;
}


quint64 PortableID::birthDate() const
{
	return mBirthDate;
}


bool PortableID::fromPortableString(QString s)
{
	QStringList parts = s.split(sepRE);
	if(parts.size()!=5) {
		return false;
	}
	mName=parts.at(0).trimmed();
	mGender=parts.at(1).trimmed();
	mID=parts.at(2).trimmed();
	mBirthDate=QDateTime::fromString(parts.at(3).trimmed(), dateFMT).toMSecsSinceEpoch();
	mType=DiscoveryTypeFromString(parts.at(4).trimmed());
	//qDebug()<<"from "<<s<<" gave birth="<<mBirthDate;
	return true;
}

QString PortableID::toPortableString()
{
	QString dateString=QDateTime::fromMSecsSinceEpoch(mBirthDate).toString(dateFMT);
	//qDebug()<<"birth="<<mBirthDate<<" to str="<<dateString;
	return mName + SEP + mGender + SEP + mID + SEP + dateString + SEP + DiscoveryTypeToString(mType);
}



DiscoveryType PortableID::type() const
{
	return mType;
}
