#ifndef NODEASSOCIATE_HPP
#define NODEASSOCIATE_HPP

#include "security/Key.hpp"
#include "comms/discovery/DiscoveryRole.hpp"
#include "basic/NetworkAddress.hpp"

#include <QBluetoothAddress>
#include "comms/ClientSignature.hpp"

#include <QByteArray>
#include <QVariantMap>
#include <QDateTime>


/*

  NodeAssociate is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  NodeAssociateStore.

*/


class NodeAssociate{
	private:
		Key mKey;

		// A list of the trusts assigned to this associate
		QStringList mTrusts;
		// The most updated trusted type and role for this associate
		DiscoveryRole mRole;
		DiscoveryType mType;
		// When was the last trusted sighting of this associate?
		quint64 mLastSeenMS;

		NetworkAddress mPublicNetworkAddress;
		NetworkAddress mLocalNetworkAddress;
		QBluetoothAddress mBluetoothAddress;
		QByteArray mNFCAddress;

		QStringList mPins;

	public:
		explicit NodeAssociate();
		explicit NodeAssociate(const QVariantMap map, bool isPublic=true);
		virtual ~NodeAssociate();


	public:
		bool update(const QVariantMap map, bool trustedSource=false);

	public:
		QString id() const;
		Key key();
		DiscoveryType type() const;
		DiscoveryRole role() const;

		bool isValidForClient(bool onlyPublic=true);
		bool isValidForServer();


		ClientSignature toClientSignature();


		void clearPins();
		void addPin(QString pin);
		const QStringList &pins();


		void clearTrust();
		void addTrust(QString trust);
		QVariantMap toVariantMap();
		void fromVariantMap(const QVariantMap map);
		QString toString();

		bool operator==(const NodeAssociate &o) const;
		bool operator!=(const NodeAssociate &o) const;
		NetworkAddress &publicAddress();
		NetworkAddress &localAddress();
		QBluetoothAddress &bluetoothAddress();
		quint64 lastSeen() const;

};


const QDebug &operator<<(QDebug &d, NodeAssociate &ass);

#endif // NODEASSOCIATE_HPP
