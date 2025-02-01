/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "Associate.hpp"

#include "client/Client.hpp"
#include "client/agent/AgentClient.hpp"
#include "client/remote/RemoteClient.hpp"
#include "client/hub/HubClient.hpp"

#include "utility/time/HumanTime.hpp"
#include "utility/string/String.hpp"

#include "uptime/MethodGate.hpp"

#include <QRegularExpression>


const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters

Associate::Associate(const QVariantMap map/*, bool isPublic*/)
	: mID ( map["id"].toString())
	, mName( map["name"].toString() )
	, mGender( map["gender"].toString() )
	, mRole( nodeRoleFromString( map["role"].toString() ) )
	, mType( nodeTypeFromString( map["type"].toString() ) )
	, mTrusts( map["trusts"].toStringList())
	, mLastSeenMS( utility::time::variantToMs(map["lastSeenMS"]) )
	, mLastInitiatedHandshakeMS( utility::time::variantToMs(map["lastInitiatedHandshakeMS"]) )
	, mLastAdherentHandshakeMS( utility::time::variantToMs(map["lastAdherentHandshakeMS"]) )
	, mBirthDate( utility::time::variantToMs(map["birthDate"]) )
	, mAddressList( map["addressList"].toList() )
	  //	, mPins( map["pins"].toStringList())// DONT STORE PINS THEY ARE EPHEMERAL

{
	OC_METHODGATE();
	//qDebug()<<"CREATE NodeAssociate(map, isPublic)"<<map<<isPublic;
}


Associate::Associate()
	: mRole(ROLE_UNKNOWN)
	, mType(TYPE_UNKNOWN)
	, mLastSeenMS(0)
	, mLastInitiatedHandshakeMS(0)
	, mLastAdherentHandshakeMS(0)
	, mBirthDate(0)
{
	OC_METHODGATE();
	//qDebug()<<"CREATE Associate()";
}


Associate::~Associate()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING node associate with ID "<<id();
}

bool Associate::update(const QVariantMap map, bool trustedSource)
{
	OC_METHODGATE();
	bool passesSecurityCheck=false;
	/*
	if(map["key"].toMap()["publicKey"].toString()==mKey.pubKey() && map["key"].toMap()["privateKey"].toString()==mKey.key()) {
		passesSecurityCheck=true;
		// TODO: Intensify this security check and make tests that can verify it well
	}
	*/

	// TODO: Look at how we can implement this test now that key is no longer part of associate
	if(map["id"].toString() == mID) {
		passesSecurityCheck=true;
	}

	if(trustedSource || passesSecurityCheck) {

		//mKey=Key( map["key"].toMap(), true);
		mName=( map["name"].toString() );
		mGender=( map["gender"].toString() );
		mRole=( nodeRoleFromString( map["role"].toString() ) );
		mType=( nodeTypeFromString( map["type"].toString() ) );
		mLastSeenMS=utility::time::variantToMs(map["lastSeenMS"]);
		mLastInitiatedHandshakeMS=utility::time::variantToMs( map["lastInitiatedHandshakeMS"] );
		mLastAdherentHandshakeMS=utility::time::variantToMs( map["lastAdherentHandshakeMS"]);
		mBirthDate=utility::time::variantToMs( map["birthDate"]);
		mAddressList=AddressList( map["addressList"].toList() );
		//NOTE: We don't just use fromVariantMap here for security reasons. See "trustedSource" parameter
		//fromVariantMap(map);
		return true;
	}
	return false;
}


QString Associate::id() //NOTE no const please
{
	OC_METHODGATE();
	return mID;
}


QString Associate::name() const
{
	OC_METHODGATE();
	return mName;
}


QString Associate::identifier()
{
	OC_METHODGATE();
	if(!mName.isEmpty()) {
		return mName;
	}
	const QString left(id().left(8));
	switch(mType) {
	case(TYPE_AGENT):
		return QStringLiteral("REMOTE-")+left;
	case(TYPE_REMOTE):
		return QStringLiteral("REMOTE-")+left;
	case(TYPE_HUB):
		return QStringLiteral("HUB-")+left;
	case(TYPE_ZOO):
		return QStringLiteral("ZOO-")+left;
	default:
		return QStringLiteral("UNKNOWN-")+left;
	}

}


QString Associate::gender() const
{
	OC_METHODGATE();
	return mGender;
}

/*
Key Associate::key()
{
	OC_METHODGATE();
	return mKey;
}
*/

NodeType Associate::type() const
{
	OC_METHODGATE();
	return mType;
}


NodeRole Associate::role() const
{
	OC_METHODGATE();
	return mRole;
}

bool Associate::isValidForClient()
{
	OC_METHODGATE();
	//const bool keyValid=mKey.isValid(onlyPublic);
	// TODO: Look at this if the check is good enough
	const bool listValid=mAddressList.isValid(false);
	const bool out=( /*keyValid && */ listValid );
	if(!out) {
		qDebug()<<"keyValid()=" /*<<keyValid<<*/ ", listValid()="<<listValid;
	}
	return out;
}


bool Associate::isValidForLocalIdentity()
{
	OC_METHODGATE();
	//const bool keyValid=mKey.isValid(onlyPublic);
	// TODO: Look at this if the check is good enough
	const bool listValid=true;
	const bool out=( /* keyValid &&  */ listValid );
	if(!out) {
		qDebug()<<"keyValid()=" /*<<keyValid<<*/ ", listValid()="<<listValid;
	}
	return out;
}


bool Associate::isValidForServer()
{
	OC_METHODGATE();
	const bool pinsEmpty=mPins.isEmpty();
	const bool validForClient=isValidForClient();
	const bool out= (!pinsEmpty) && validForClient;
	if(!out) {
		qDebug()<<"pinsEmpty()="<<pinsEmpty<<", validForClient()="<<validForClient;
	}
	return out;
}


AddressList &Associate::addressList()
{
	OC_METHODGATE();
	return mAddressList;
}


void Associate::setLastSeen(quint64 when)
{
	OC_METHODGATE();
	if(0 == when) {
		when=utility::time::currentMsecsSinceEpoch<quint64>();
	}
	mLastSeenMS=when;
}


quint64 Associate::lastSeen() const
{
	OC_METHODGATE();
	return mLastSeenMS;
}


quint64 Associate::lastInitiatedHandshake() const
{
	OC_METHODGATE();
	return mLastInitiatedHandshakeMS;
}


quint64 Associate::lastAdherentHandshake() const
{
	OC_METHODGATE();
	return mLastAdherentHandshakeMS;
}


TrustList &Associate::trusts()
{
	OC_METHODGATE();
	return mTrusts;
}


void Associate::clearPins()
{
	OC_METHODGATE();
	mPins.clear();
}


void Associate::addPin(QString pin)
{
	OC_METHODGATE();
	if(rePin.match(pin).hasMatch()) {
		//qDebug()<<"ACCEPTED PIN:" <<pin;
		mPins<<pin;
	} else {
		//qDebug()<<"Pin "<<pin<<" did not match validation:";
	}
}


bool Associate::hasPin(QString pin)
{
	return mPins.contains(pin);
}

const QStringList &Associate::pins()
{
	OC_METHODGATE();
	return mPins;
}


PortableID Associate::toPortableID()
{
	OC_METHODGATE();
	PortableID pid;
	pid.setName(mName);
	pid.setGender(mGender);
	pid.setID(id());
	pid.setType(mType);
	pid.setBirthDate(mBirthDate);
	return pid;
}


QVariantMap Associate::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["addressList"]=mAddressList.toVariantList();
	map["lastSeenMS"]=utility::time::msToVariant(mLastSeenMS);
	map["lastInitiatedHandshakeMS"]=utility::time::msToVariant(mLastInitiatedHandshakeMS);

	map["lastAdherentHandshakeMS"]=utility::time::msToVariant(mLastAdherentHandshakeMS);
	map["birthDate"]=utility::time::msToVariant(mBirthDate);
	//map["key"]=mKey.toVariantMap(true);
	map["id"]=mID;
	map["role"]=nodeRoleToString(mRole);
	map["type"]=nodeTypeToString(mType);
	map["name"]=mName;
	map["gender"]=mGender;
	//map["pins"]=mPins;//DONT STORE PINS THEY ARE EPHEMERAL
	map["trusts"]=QStringList(mTrusts.begin(), mTrusts.end());
	return map;
}


void Associate::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	//mKey=Key( map["key"].toMap(), true);
	mID=( map["id"].toString() );
	mName=( map["name"].toString() );
	mGender=( map["gender"].toString() );
	mBirthDate= utility::time::variantToMs( map["birthDate"]);
	mRole=( nodeRoleFromString( map["role"].toString() ) );
	mType=( nodeTypeFromString( map["type"].toString() ) );
	mLastSeenMS=utility::time::variantToMs( map["lastSeenMS"]);
	mLastInitiatedHandshakeMS=utility::time::variantToMs( map["lastInitiatedHandshakeMS"]);
	mLastAdherentHandshakeMS=utility::time::variantToMs( map["lastAdherentHandshakeMS"]);
	mAddressList=AddressList( map["addressList"].toList() );
	mTrusts=map["trusts"].toStringList();
}


QString Associate::toString()
{
	OC_METHODGATE();
	return QString() //mKey.toString()
		+"  name: "+mName
		+", id: "+id()
		+", gender: "+mGender
		+", addressList:"+mAddressList.toString()
		+", lastSeenMS:"+utility::string::formattedDateFromMS(mLastSeenMS)
		+", lastInitiatedHandshakeMS:"+utility::string::formattedDateFromMS(mLastInitiatedHandshakeMS)
		+", lastAdherentHandshakeMS:"+utility::string::formattedDateFromMS(mLastAdherentHandshakeMS)
		+", birthDate:"+utility::string::formattedDateFromMS(mBirthDate)
		+", role:"+nodeRoleToString(mRole)
		+", type:"+nodeTypeToString(mType)
		+", pins:"+mPins.join(";")
		+", trusts:"+QStringList(mTrusts.begin(), mTrusts.end()).join(";");
}


QMap<QString, QString> Associate::toMap()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	map["name"]=mName;
	map["id"]=id();
	map["gender"]=mGender;
	map["addressList"]=mAddressList.toString();
	map["lastSeenMS"]=utility::string::formattedDateFromMS(mLastSeenMS);
	map["lastInitiatedHandshakeMS"]=utility::string::formattedDateFromMS(mLastInitiatedHandshakeMS);
	map["lastAdherentHandshakeMS"]=utility::string::formattedDateFromMS(mLastAdherentHandshakeMS);
	map["birthDate"]=utility::string::formattedDateFromMS(mBirthDate);
	map["role"]=nodeRoleToString(mRole);
	map["type"]=nodeTypeToString(mType);
	map["pins"]=mPins.join(";");
	map["trusts"]=QStringList(mTrusts.begin(), mTrusts.end()).join(";");
	return map;
}


QSharedPointer<Client> Associate::toClient(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	QSharedPointer<Client> ret;
	switch(mType) {
	case(TYPE_AGENT):
		ret = QSharedPointer<AgentClient>::create();
		break;
	case(TYPE_REMOTE):
		ret=QSharedPointer<RemoteClient>::create();
		break;
	case(TYPE_HUB):
		ret=QSharedPointer<HubClient>::create();
		break;
	default:
		qWarning()<<"ERROR: unknown node type";
	}
	if(!ret.isNull()) {
		ret->configure(node, sharedFromThis());
	}
	return ret;
}


bool Associate::operator==(Associate &o)
{
	OC_METHODGATE();
	return id() == o.id();
}


bool Associate::operator!=(Associate &o)
{
	OC_METHODGATE();
	return (! (o == *this));
}


const QDebug &operator<<(QDebug &d, Associate &ass)
{
	OC_FUNCTIONGATE();
	d.nospace() << "NodeAssociate("<<ass.toString()<<")";
	return d.maybeSpace();
}
