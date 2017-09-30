/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef ASSOCIATE_HPP
#define ASSOCIATE_HPP


#include "discovery/DiscoveryRole.hpp"
#include "comms/NetworkAddress.hpp"
#include "comms/CommsSignature.hpp"

#include "security/Key.hpp"

#include "security/PortableID.hpp"


#include <QBluetoothAddress>

#include <QByteArray>
#include <QVariantMap>
#include <QDateTime>


class PortableID;

/*

  NodeAssociate is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  NodeAssociateStore.

*/


class Associate
{
private:
	Key mKey;

	QString mName;
	QString mGender;
	// The most updated trusted type and role for this associate
	DiscoveryRole mRole;
	DiscoveryType mType;
	// A list of the trusts assigned to this associate
	QStringList mTrusts;

	// When was the last trusted sighting of this associate?
	quint64 mLastSeenMS;
	// When was the last time this associate initiated a handshake?
	quint64 mLastInitiatedHandshakeMS;
	// When was the last time this associate received a handshake initiated on the other end?
	quint64 mLastAdherentHandshakeMS;

	quint64 mBirthDate;

	NetworkAddress mPublicNetworkAddress;
	NetworkAddress mLocalNetworkAddress;
	QBluetoothAddress mBluetoothAddress;
	QByteArray mNFCAddress;

	QStringList mPins;

public:
	explicit Associate();
	explicit Associate(const QVariantMap map, bool isPublic=true);
	virtual ~Associate();


public:
	bool update(const QVariantMap map, bool trustedSource=false);

public:
	QString id();//NOTE no const please;
	QString name() const;
	QString gender() const;
	Key key();
	DiscoveryType type() const;
	DiscoveryRole role() const;

	bool isValidForClient(bool onlyPublic=true);
	bool isValidForServer();

	NetworkAddress publicAddress() const;
	NetworkAddress localAddress() const;
	// TODO: Merge into NetworkAddress?
	QBluetoothAddress bluetoothAddress()const;


	void setLastSeen(quint64 when=0);
	quint64 lastSeen() const;
	quint64 lastInitiatedHandshake() const;
	quint64 lastAdherentHandshake() const;



public: // Pins management
	void clearPins();
	void addPin(QString pin);
	bool hasPin(QString pin);
	const QStringList &pins();


public: // Trusts management

	void clearTrust();
	void addTrust(QString trust);
	void removeTrust(QString trust);
	bool hasTrust(QString trust);
	const QStringList &trusts();

public:// to/from

	PortableID toPortableID();
	QVariantMap toVariantMap();
	void fromVariantMap(const QVariantMap map);
	QString toString();


public: // Operators
	bool operator==(const Associate &o) const;
	bool operator!=(const Associate &o) const;

};


const QDebug &operator<<(QDebug &d, Associate &ass);

#endif // ASSOCIATE_HPP
