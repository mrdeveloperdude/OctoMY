#include "NodeAssociate.hpp"



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

DiscoveryType NodeAssociate::type() const
{
	OC_METHODGATE();
	return mType;
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
			+", type:"+DiscoveryTypeToString(mType);

}


bool NodeAssociate::isValid(bool onlyPublic)
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


