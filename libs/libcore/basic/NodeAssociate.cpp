#include "basic/NodeAssociate.hpp"

#include <QRegularExpression>

NodeAssociate::NodeAssociate(const QVariantMap map, bool isPublic)
	: mKey( map["key"].toMap(), isPublic)
	, mRole( DiscoveryRoleFromString( map["role"].toString() ) )
	, mType( DiscoveryTypeFromString( map["type"].toString() ) )
	, mLastSeenMS( QDateTime::currentMSecsSinceEpoch() )
	, mPublicNetworkAddress( map["publicAddress"].toMap() )
	, mLocalNetworkAddress( map["localAddress"].toMap() )
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
	qDebug()<<"DELETING node associate with ID "<<id();
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

const QStringList &NodeAssociate::pins(){
	OC_METHODGATE();
	return mPins;
}


QVariantMap NodeAssociate::toVariantMap()
{
	OC_METHODGATE();
	QVariantMap map;
	map["publicAddress"]=mPublicNetworkAddress.toVariantMap();
	map["localAddress"]==mLocalNetworkAddress.toVariantMap();
	map["lastSeenMS"]=mLastSeenMS;
	map["key"]=mKey.toVariantMap(true);
	map["role"]=DiscoveryRoleToString(mRole);
	map["type"]=DiscoveryTypeToString(mType);
	map["pins"]=mPins;
	return map;
}



void NodeAssociate::fromVariantMap(const QVariantMap map)
{
	OC_METHODGATE();
	mKey=Key( map["key"].toMap(), true);
	mRole=( DiscoveryRoleFromString( map["role"].toString() ) );
	mType=( DiscoveryTypeFromString( map["type"].toString() ) );
	mLastSeenMS=( QDateTime::currentMSecsSinceEpoch() );
	mPublicNetworkAddress=NetworkAddress( map["publicAddress"].toMap() );
	mLocalNetworkAddress=NetworkAddress( map["localAddress"].toMap() );
}


QString NodeAssociate::toString()
{
	OC_METHODGATE();
	return mKey.toString()
			+", publicAddress:"+mPublicNetworkAddress.toString()
			+", localAddress:"+mLocalNetworkAddress.toString()
			+", lastSeenMS:"+QString::number(mLastSeenMS)
			+", role:"+DiscoveryRoleToString(mRole)
			+", type:"+DiscoveryTypeToString(mType)
			+", pins:"+mPins.join(";");
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

