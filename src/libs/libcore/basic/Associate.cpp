/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "basic/Associate.hpp"
#include "utility/Utility.hpp"



#include "node/Client.hpp"
#include "node/AgentClient.hpp"
#include "node/RemoteClient.hpp"
#include "node/HubClient.hpp"


#include <QRegularExpression>


Associate::Associate(const QVariantMap map, bool isPublic)
	: mKey( map["key"].toMap(), isPublic)
	, mName( map["name"].toString() )
	, mGender( map["gender"].toString() )
	, mRole( nodeRoleFromString( map["role"].toString() ) )
	, mType( nodeTypeFromString( map["type"].toString() ) )
	, mTrusts( map["trusts"].toStringList())
	, mLastSeenMS( utility::variantToMs(map["lastSeenMS"]) )
	, mLastInitiatedHandshakeMS( utility::variantToMs(map["lastInitiatedHandshakeMS"]) )
	, mLastAdherentHandshakeMS( utility::variantToMs(map["lastAdherentHandshakeMS"]) )
	, mBirthDate( utility::variantToMs(map["birthDate"]) )
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
	if(map["key"].toMap()["publicKey"].toString()==mKey.pubKey() && map["key"].toMap()["privateKey"].toString()==mKey.key()) {
		passesSecurityCheck=true;
		//TODO: Intensify this security check and make tests that can verify it well
	}
	if(trustedSource || passesSecurityCheck) {

		//mKey=Key( map["key"].toMap(), true);
		mName=( map["name"].toString() );
		mGender=( map["gender"].toString() );
		mRole=( nodeRoleFromString( map["role"].toString() ) );
		mType=( nodeTypeFromString( map["type"].toString() ) );
		mLastSeenMS=utility::variantToMs(map["lastSeenMS"]);
		mLastInitiatedHandshakeMS=utility::variantToMs( map["lastInitiatedHandshakeMS"] );
		mLastAdherentHandshakeMS=utility::variantToMs( map["lastAdherentHandshakeMS"]);
		mBirthDate=utility::variantToMs( map["birthDate"]);
		mAddressList=AddressList( map["addressList"].toList() );
		//NOTE: We don't just use fromVariantMap here for security reasons. See "trustedSource" parameter
		//fromVariantMap(map);
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////

QString Associate::id() //NOTE no const please
{
	OC_METHODGATE();
	return mKey.id();
}

QString Associate::name() const
{
	OC_METHODGATE();
	return mName;
}

//Unlike name() this always returns something. For agent it i sname, for remote it is remote:id etc.
QString Associate::identifier()
{
	OC_METHODGATE();
	switch(mType) {
	case(TYPE_AGENT):
		return mName;
	case(TYPE_REMOTE):
		return QStringLiteral("REMOTE-")+id().left(8);
	case(TYPE_HUB):
		return QStringLiteral("HUB-")+id().left(8);
	case(TYPE_ZOO):
		return QStringLiteral("ZOO-")+id().left(8);
	}
	return QStringLiteral("UNKNOWN-")+id().left(8);
}



QString Associate::gender() const
{
	OC_METHODGATE();
	return mGender;
}


Key Associate::key()
{
	OC_METHODGATE();
	return mKey;
}

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




bool Associate::isValidForClient(bool onlyPublic)
{
	OC_METHODGATE();
	const bool keyValid=mKey.isValid(onlyPublic);
	const bool listValid=mAddressList.isValid(false);
	const bool out=( keyValid &&  listValid );
	if(!out) {
		qDebug()<<"keyValid(onlyPublic="<<onlyPublic<< ")="<<keyValid<<", listValid()="<<listValid;
	}
	return out;
}



bool Associate::isValidForLocalIdentity(bool onlyPublic)
{
	OC_METHODGATE();
	const bool keyValid=mKey.isValid(onlyPublic);
	const bool listValid=true;
	const bool out=( keyValid &&  listValid );
	if(!out) {
		qDebug()<<"keyValid(onlyPublic="<<onlyPublic<< ")="<<keyValid<<", listValid()="<<listValid;
	}
	return out;
}


bool Associate::isValidForServer()
{
	OC_METHODGATE();
	const bool pinsEmpty=mPins.isEmpty();
	const bool validForClient=isValidForClient(true);
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

QBluetoothAddress Associate::bluetoothAddress() const
{
	OC_METHODGATE();
	return mBluetoothAddress;
}


void Associate::setLastSeen(quint64 when)
{
	OC_METHODGATE();
	if(0 == when) {
		when=QDateTime::currentMSecsSinceEpoch();
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


////////////////////////////////////////////////////////////////////////////////


void Associate::clearTrust()
{
	OC_METHODGATE();
	mTrusts.clear();
}

void Associate::addTrust(QString trust)
{
	OC_METHODGATE();
	mTrusts.push_back(trust);
}


void Associate::removeTrust(QString trust)
{
	OC_METHODGATE();
	if(mTrusts.contains(trust)) {
		mTrusts.removeAll(trust);
	}
}

bool Associate::hasTrust(QString trust)
{
	return mTrusts.contains(trust);
}

const QStringList &Associate::trusts()
{
	OC_METHODGATE();
	return mTrusts;
}




////////////////////////////////////////////////////////////////////////////////

const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


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



////////////////////////////////////////////////////////////////////////////////

PortableID Associate::toPortableID()
{
	OC_METHODGATE();
	PortableID pid;
	pid.setName(mName);
	pid.setGender(mGender);
	pid.setID(mKey.id());
	pid.setType(mType);
	pid.setBirthDate(mBirthDate);
	return pid;
}


QVariantMap Associate::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["addressList"]=mAddressList.toVariantList();
	map["lastSeenMS"]=utility::msToVariant(mLastSeenMS);
	map["lastInitiatedHandshakeMS"]=utility::msToVariant(mLastInitiatedHandshakeMS);

	map["lastAdherentHandshakeMS"]=utility::msToVariant(mLastAdherentHandshakeMS);
	map["birthDate"]=utility::msToVariant(mBirthDate);
	map["key"]=mKey.toVariantMap(true);
	map["role"]=nodeRoleToString(mRole);
	map["type"]=nodeTypeToString(mType);
	map["name"]=mName;
	map["gender"]=mGender;
	//map["pins"]=mPins;//DONT STORE PINS THEY ARE EPHEMERAL
	map["trusts"]=mTrusts;
	return map;
}



void Associate::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	mKey=Key( map["key"].toMap(), true);
	mName=( map["name"].toString() );
	mGender=( map["gender"].toString() );
	mBirthDate= utility::variantToMs( map["birthDate"]);
	mRole=( nodeRoleFromString( map["role"].toString() ) );
	mType=( nodeTypeFromString( map["type"].toString() ) );
	mLastSeenMS=utility::variantToMs( map["lastSeenMS"]);
	mLastInitiatedHandshakeMS=utility::variantToMs( map["lastInitiatedHandshakeMS"]);
	mLastAdherentHandshakeMS=utility::variantToMs( map["lastAdherentHandshakeMS"]);
	mAddressList=AddressList( map["addressList"].toList() );
	mTrusts=map["trusts"].toStringList();
}



QString Associate::toString()
{
	OC_METHODGATE();
	return mKey.toString()
		   +", name: "+mName
		   +", gender: "+mGender
		   +", addressList:"+mAddressList.toString()
		   +", lastSeenMS:"+utility::formattedDateFromMS(mLastSeenMS)
		   +", lastInitiatedHandshakeMS:"+utility::formattedDateFromMS(mLastInitiatedHandshakeMS)
		   +", lastAdherentHandshakeMS:"+utility::formattedDateFromMS(mLastAdherentHandshakeMS)
		   +", birthDate:"+utility::formattedDateFromMS(mBirthDate)
		   +", role:"+nodeRoleToString(mRole)
		   +", type:"+nodeTypeToString(mType)
		   +", pins:"+mPins.join(";");
	+", trusts:"+mTrusts.join(";");
}


QSharedPointer<Client> Associate::toClient(QSharedPointer<Node> node)
{
	switch(mType) {
	case(TYPE_AGENT):
		return QSharedPointer<AgentClient>(OC_NEW AgentClient(node, sharedFromThis()));
	case(TYPE_REMOTE):
		return QSharedPointer<RemoteClient>(OC_NEW RemoteClient(node, sharedFromThis()));
	case(TYPE_HUB):
		return QSharedPointer<HubClient>(OC_NEW HubClient(node, sharedFromThis()));
	}
	return nullptr;

}

////////////////////////////////////////////////////////////////////////////////

bool Associate::operator==(Associate &o)
{
	OC_METHODGATE();
	return mKey.id() == o.mKey.id();
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
