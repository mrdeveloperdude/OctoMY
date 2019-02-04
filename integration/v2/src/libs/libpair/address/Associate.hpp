/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef ASSOCIATE_HPP
#define ASSOCIATE_HPP


#include "node/NodeRole.hpp"
#include "comms/NetworkAddress.hpp"
#include "comms/CommsSignature.hpp"

#include "security/Key.hpp"

#include "security/PortableID.hpp"

#include "address/AddressList.hpp"

#include <QBluetoothAddress>

#include <QByteArray>
#include <QVariantMap>
#include <QDateTime>


class PortableID;
class Client;
class Node;

/*

  Associate is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  AddressBook.

*/


class Associate: public QEnableSharedFromThis<Associate>
{
private:
	Key mKey;

	QString mName;
	QString mGender;
	// The most updated trusted type and role for this associate
	NodeRole mRole;
	NodeType mType;
	// A list of the trusts assigned to this associate
	QStringList mTrusts;

	// When was the last trusted sighting of this associate?
	quint64 mLastSeenMS;
	// When was the last time this associate initiated a handshake?
	quint64 mLastInitiatedHandshakeMS;
	// When was the last time this associate received a handshake initiated on the other end?
	quint64 mLastAdherentHandshakeMS;

	quint64 mBirthDate;

	//NetworkAddress mPublicNetworkAddress;
	//NetworkAddress mLocalNetworkAddress;

	AddressList mAddressList;
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
	QString identifier();
	QString gender() const;
	Key key();
	NodeType type() const;
	NodeRole role() const;

	bool isValidForClient(bool onlyPublic=true);
	bool isValidForLocalIdentity(bool onlyPublic=true);
	bool isValidForServer();

	AddressList &addressList();
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
	QSharedPointer<Client> toClient(QSharedPointer<Node> node);

public: // Operators
	bool operator==(Associate &o);
	bool operator!=(Associate &o);

};


const QDebug &operator<<(QDebug &d, Associate &ass);

#endif // ASSOCIATE_HPP
