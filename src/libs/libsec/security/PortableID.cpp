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
	OC_METHODGATE();
}


PortableID::PortableID(QVariantMap &data)
	: mName(data["name"].toString())
	, mGender(data["gender"].toString())
	, mID(data["key"].toMap()["id"].toString())
	, mBirthDate(QDateTime::fromString(data["createDate"].toString(), dateFMT).toMSecsSinceEpoch())
	, mType(DiscoveryTypeFromString(data["type"].toString()))
{
	OC_METHODGATE();
}

PortableID::~PortableID()
{
	OC_METHODGATE();
}



void PortableID::setName(QString name)
{
	OC_METHODGATE();
	mName=name;
}

void PortableID::setGender(QString gender)
{
	OC_METHODGATE();
	mGender=gender;
}

void PortableID::setID(QString id)
{
	OC_METHODGATE();
	mID=id;
}


void PortableID::setBirthDate(quint64 birthDate)
{
	OC_METHODGATE();
	mBirthDate=birthDate;
}


void PortableID::setType(DiscoveryType type)
{
	OC_METHODGATE();
	mType=type;
}




QString PortableID::name() const
{
	OC_METHODGATE();
	return mName;
}

QString PortableID::identifier() const
{
	OC_METHODGATE();
	switch(mType) {
	case(TYPE_AGENT):
		return mName;
	case(TYPE_REMOTE):
		return QStringLiteral("REMOTE-")+mID.left(8);
	case(TYPE_HUB):
		return QStringLiteral("HUB-")+mID.left(8);
	case(TYPE_ZOO):
		return QStringLiteral("ZOO-")+mID.left(8);
	}
	return QStringLiteral("UNKNONW-")+mID.left(8);
}

QString PortableID::gender() const
{
	OC_METHODGATE();
	return mGender;
}

QString PortableID::id() const
{
	OC_METHODGATE();
	return mID;
}


quint64 PortableID::birthDate() const
{
	OC_METHODGATE();
	return mBirthDate;
}


bool PortableID::fromPortableString(QString s)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	QString dateString=QDateTime::fromMSecsSinceEpoch(mBirthDate).toString(dateFMT);
	//qDebug()<<"birth="<<mBirthDate<<" to str="<<dateString;
	return mName + SEP + mGender + SEP + mID + SEP + dateString + SEP + DiscoveryTypeToString(mType);
}



DiscoveryType PortableID::type() const
{
	OC_METHODGATE();
	return mType;
}
