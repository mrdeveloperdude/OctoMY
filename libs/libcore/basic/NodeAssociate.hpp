#ifndef NODEASSOCIATE_HPP
#define NODEASSOCIATE_HPP

#include "security/Key.hpp"
#include "comms/discovery/DiscoveryRole.hpp"
#include "basic/NetworkAddress.hpp"

#include <QBluetoothAddress>
#include <QByteArray>
#include <QVariantMap>
#include <QDateTime>


/*

  NodeAssociate is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  DiscoveryClientStore.

*/



class NodeAssociate{
	public:
		Key mKey;
	public:
		// A list of the trusts assigned to this participant
		QStringList mTrusts;
		// The most updated trusted type and role for this participant
		DiscoveryRole mRole;
		DiscoveryType mType;
		// When was the last trusted sighting of this associate?
		quint64 mLastSeenMS;

	public:
		NetworkAddress mPublicNetworkAddress;
		NetworkAddress mLocalNetworkAddress;
		QBluetoothAddress mBluetoothAddress;
		QByteArray mNFCAddress;

	public:
		NodeAssociate(QVariantMap map, bool isPublic)
			: mKey( map["key"].toMap(), isPublic)
			, mRole( DiscoveryRoleFromString( map["role"].toString() ) )
			, mType( DiscoveryTypeFromString( map["type"].toString() ) )
			, mLastSeenMS( QDateTime::currentMSecsSinceEpoch() )
			, mPublicNetworkAddress( map["publicAddress"].toMap() )
			, mLocalNetworkAddress( map["localAddress"].toMap() )
		{
			qDebug()<<"NodeAssociate(map, isPublic)"<<map<<isPublic;
		}

		NodeAssociate()
			: mRole(ROLE_UNKNOWN)
			, mType(TYPE_UNKNOWN)
			, mLastSeenMS(0)
		{
			qDebug()<<"NodeAssociate()";
		}


	public:

		void update(QVariantMap map, bool trustedSource){
			//TODO: implement and use to ensure the security of this class
		}


	public:

		inline QString id() const
		{
			return mKey.id();
		}

		DiscoveryType type() const
		{
			return mType;
		}


		void clearTrust()
		{
			mTrusts.clear();
		}

		void addTrust(QString trust)
		{
			mTrusts.push_back(trust);
		}

		QVariantMap toVariantMap()
		{
			QVariantMap map;
			map["publicAddress"]=mPublicNetworkAddress.toVariantMap();
			map["localAddress"]==mLocalNetworkAddress.toVariantMap();
			map["lastSeenMS"]=mLastSeenMS;
			map["key"]=mKey.toVariantMap(true);
			map["role"]=DiscoveryRoleToString(mRole);
			map["type"]=DiscoveryTypeToString(mType);
			return map;
		}

		QString toString()
		{

			return mKey.toString()
					+", publicAddress:"+mPublicNetworkAddress.toString()
					+", localAddress:"+mLocalNetworkAddress.toString()
					+", lastSeenMS:"+QString::number(mLastSeenMS)
					+", role:"+DiscoveryRoleToString(mRole)
					+", type:"+DiscoveryTypeToString(mType);

		}


		bool isValid()
		{
			return (
						mKey.isValid(true)
						&& (
							mPublicNetworkAddress.isValid()
							|| mLocalNetworkAddress.isValid()
							)
						);
		}


		bool operator==(const NodeAssociate &o) const{
			//TODO: Contemplate what effect adding test for key and bt address would have on this method and why they are currently missing
			return o.mPublicNetworkAddress == mPublicNetworkAddress
					&& o.mLocalNetworkAddress == mLocalNetworkAddress;
			//Disregard lastTime on purpose
		}



		inline NetworkAddress &publicAddress()
		{
			return mPublicNetworkAddress;
		}


		inline NetworkAddress &localAddress()
		{
			return mLocalNetworkAddress;
		}

		inline QBluetoothAddress &bluetoothAddress()
		{
			return mBluetoothAddress;
		}


};


#endif // NODEASSOCIATE_HPP
