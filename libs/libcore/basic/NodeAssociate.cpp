#include "basic/NodeAssociate.hpp"

#include <QRegularExpression>

NodeAssociate::NodeAssociate(const QVariantMap map, bool isPublic)
	: mKey( map["key"].toMap(), isPublic)
	, mName( map["name"].toString() )
	, mGender( map["gender"].toString() )
	, mRole( DiscoveryRoleFromString( map["role"].toString() ) )
	, mType( DiscoveryTypeFromString( map["type"].toString() ) )
	, mLastSeenMS( QDateTime::currentMSecsSinceEpoch() )
	, mBirthDate( map["birthDate"].toULongLong())
	, mPublicNetworkAddress( map["publicAddress"].toMap() )
	, mLocalNetworkAddress( map["localAddress"].toMap() )
	//	, mPins( map["pins"].toStringList())// DONT STORE PINS THEY ARE EPHEMERAL
	, mTrusts( map["trusts"].toStringList())
{
	OC_METHODGATE();
	//qDebug()<<"CREATE NodeAssociate(map, isPublic)"<<map<<isPublic;
}

NodeAssociate::NodeAssociate()
	: mRole(ROLE_UNKNOWN)
	, mType(TYPE_UNKNOWN)
	, mLastSeenMS(0)
{
	OC_METHODGATE();
	//qDebug()<<"CREATE NodeAssociate()";
}



NodeAssociate::~NodeAssociate(){
	//qDebug()<<"DELETING node associate with ID "<<id();
}

bool NodeAssociate::update(const QVariantMap map, bool trustedSource){
	OC_METHODGATE();
	bool passesSecurityCheck=false;
	if(map["key"].toMap()["publicKey"].toString()==mKey.pubKey() && map["key"].toMap()["privateKey"].toString()==mKey.key()){
		passesSecurityCheck=true;
		//TODO: Intensify this security check and make tests that can verify it well
	}
	if(trustedSource || passesSecurityCheck){

		//mKey=Key( map["key"].toMap(), true);
		mName=( map["name"].toString() );
		mGender=( map["gender"].toString() );
		mRole=( DiscoveryRoleFromString( map["role"].toString() ) );
		mType=( DiscoveryTypeFromString( map["type"].toString() ) );
		mLastSeenMS=( QDateTime::currentMSecsSinceEpoch() );
		mPublicNetworkAddress=NetworkAddress( map["publicAddress"].toMap() );
		mLocalNetworkAddress=NetworkAddress( map["localAddress"].toMap() );

		//fromVariantMap(map);
		return true;
	}
	return false;
}




QString NodeAssociate::id() const
{
	OC_METHODGATE();
	return mKey.id();
}

QString NodeAssociate::name() const
{
	OC_METHODGATE();
	return mName;
}



QString NodeAssociate::gender() const
{
	OC_METHODGATE();
	return mGender;
}


Key NodeAssociate::key()
{
	OC_METHODGATE();
	return mKey;
}

DiscoveryType NodeAssociate::type() const
{
	OC_METHODGATE();
	return mType;
}

DiscoveryRole NodeAssociate::role() const
{
	OC_METHODGATE();
	return mRole;
}


void NodeAssociate::clearTrust()
{
	OC_METHODGATE();
	mTrusts.clear();
}

void NodeAssociate::addTrust(QString trust)
{
	OC_METHODGATE();
	mTrusts.push_back(trust);
}


void NodeAssociate::removeTrust(QString trust)
{
	OC_METHODGATE();
	if(mTrusts.contains(trust)){
		mTrusts.removeAll(trust);
	}
}



const QRegularExpression rePin("^[0-9A-H]{5}$"); // trimmed 5-digit string with 0-9 and A-H as valid characters


void NodeAssociate::clearPins()
{
	OC_METHODGATE();
	mPins.clear();
}


void NodeAssociate::addPin(QString pin)
{
	OC_METHODGATE();
	if(rePin.match(pin).hasMatch()){
		//qDebug()<<"ACCEPTED PIN:" <<pin;
		mPins<<pin;
	}
	else {
		//qDebug()<<"Pin "<<pin<<" did not match validation:";
	}
}


bool NodeAssociate::hasTrust(QString trust){
	return mTrusts.contains(trust);
}

bool NodeAssociate::hasPin(QString pin){
	return mPins.contains(pin);
}

const QStringList &NodeAssociate::pins(){
	OC_METHODGATE();
	return mPins;
}


const QStringList &NodeAssociate::trusts(){
	OC_METHODGATE();
	return mTrusts;
}



QVariantMap NodeAssociate::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["publicAddress"]=mPublicNetworkAddress.toVariantMap();
	map["localAddress"]==mLocalNetworkAddress.toVariantMap();
	map["lastSeenMS"]=mLastSeenMS;
	map["birthDate"]=mBirthDate;
	map["key"]=mKey.toVariantMap(true);
	map["role"]=DiscoveryRoleToString(mRole);
	map["type"]=DiscoveryTypeToString(mType);
	map["name"]=mName;
	map["gender"]=mGender;
	//map["pins"]=mPins;//DONT STORE PINS THEY ARE EPHEMERAL
	map["trusts"]=mTrusts;
	return map;
}



void NodeAssociate::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	mKey=Key( map["key"].toMap(), true);
	mName=( map["name"].toString() );
	mGender=( map["gender"].toString() );
	mBirthDate= ( map["birthDate"].toULongLong());
	mRole=( DiscoveryRoleFromString( map["role"].toString() ) );
	mType=( DiscoveryTypeFromString( map["type"].toString() ) );
	mLastSeenMS=( QDateTime::currentMSecsSinceEpoch() );
	mPublicNetworkAddress=NetworkAddress( map["publicAddress"].toMap() );
	mLocalNetworkAddress=NetworkAddress( map["localAddress"].toMap() );
	//mPins=map["pins"].toStringList(); //DONT STORE PINS THEY ARE EPHEMERAL
	mTrusts=map["trusts"].toStringList();
}

#include "utility/Utility.hpp"

QString NodeAssociate::toString()
{
	OC_METHODGATE();
	return mKey.toString()
			+"name: "+mName
			+", gender: "+mGender
			+", publicAddress:"+mPublicNetworkAddress.toString()
			+", localAddress:"+mLocalNetworkAddress.toString()
			+", birthDate:"+utility::formattedDateFromMS( mBirthDate)
			+", lastSeenMS:"+utility::formattedDateFromMS( mLastSeenMS)
			+", role:"+DiscoveryRoleToString(mRole)
			+", type:"+DiscoveryTypeToString(mType)
			+", pins:"+mPins.join(";");
	+", trusts:"+mTrusts.join(";");
}


bool NodeAssociate::isValidForClient(bool onlyPublic)
{
	OC_METHODGATE();
	return (
				mKey.isValid(onlyPublic)
				&& (
					mPublicNetworkAddress.isValid()
					|| mLocalNetworkAddress.isValid()
					)
				);
}


bool NodeAssociate::isValidForServer()
{
	OC_METHODGATE();
	return (!mPins.isEmpty()) && isValidForClient(true) ;
}


ClientSignature NodeAssociate::toClientSignature()
{
	OC_METHODGATE();
	return ClientSignature(id(), publicAddress());
}




PortableID NodeAssociate::toPortableID()
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


bool NodeAssociate::operator==(const NodeAssociate &o) const{
	OC_METHODGATE();
	//TODO: Contemplate what effect adding test for key and bt address would have on this method and why they are currently missing
	return o.mPublicNetworkAddress == mPublicNetworkAddress
			&& o.mLocalNetworkAddress == mLocalNetworkAddress;
	//Disregard lastTime on purpose
}


bool NodeAssociate::operator!=(const NodeAssociate &o) const{
	OC_METHODGATE();
	return (! (o == *this));
}



NetworkAddress &NodeAssociate::publicAddress()
{
	OC_METHODGATE();
	return mPublicNetworkAddress;
}


NetworkAddress &NodeAssociate::localAddress()
{
	OC_METHODGATE();
	return mLocalNetworkAddress;
}

QBluetoothAddress &NodeAssociate::bluetoothAddress()
{
	OC_METHODGATE();
	return mBluetoothAddress;
}




quint64 NodeAssociate::lastSeen() const
{
	return mLastSeenMS;
}


const QDebug &operator<<(QDebug &d, NodeAssociate &ass)
{
	OC_FUNCTIONGATE();
	d.nospace() << "NodeAssociate("<<ass.toString()<<")";
	return d.maybeSpace();
}

