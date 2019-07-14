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

#include "TrustList.hpp"

#include <QBluetoothAddress>

#include <QByteArray>
#include <QVariantMap>
#include <QDateTime>


class PortableID;
class Client;
class Node;



/**
 * @brief The Associate class is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  the AddressBook class.

  It is also used to store our own identity via the LocalIdentityStore

 */

class Associate: public QEnableSharedFromThis<Associate>
{
private:
	// The public key of this associate
//	Key mKey;
	QString mID;
	// The name of this associate
	QString mName;
	// The gender of this associate
	QString mGender;
	// The role of this associate
	NodeRole mRole;
	// The type of this associate
	NodeType mType;
	// A list of the trusts assigned to this associate
	TrustList mTrusts;
	// The last trusted sighting of this associate
	quint64 mLastSeenMS;
	// The last time this associate initiated a handshake
	quint64 mLastInitiatedHandshakeMS;
	// The last time this associate received a handshake initiated on the other end
	quint64 mLastAdherentHandshakeMS;
	// The birthdate of this associate
	quint64 mBirthDate;
	// The network addresses this associate has been known to be reachable by
	AddressList mAddressList;
	// The bluetooth address this associate has been known to use
	QBluetoothAddress mBluetoothAddress;
	// The NFC address this associate has been known to use
	QByteArray mNFCAddress;
	// The pins this associate knows (for multi factor auth)
	// NOTE: pins are not stored, they should be ephemeral
	QStringList mPins;

public:
	explicit Associate();
	explicit Associate(const QVariantMap map/*, bool isPublic=true*/);
	virtual ~Associate();

public:
	bool update(const QVariantMap map, bool trustedSource=false);

public:
	// Return the ID string
	QString id(); //NOTE no const please;
	// Provide the name
	QString name() const;
	// Provide a unique human readible identifier string
	// Unlike name() this always returns something. For agent it is name, for remote it is name if existing or id if not etc.
	QString identifier();
	// Provide gender
	QString gender() const;
	// Provide the private key
	//Key key();
	// Provide the node type
	NodeType type() const;
	// Provide the node role
	NodeRole role() const;

	// Determine is this associate is usable for storing details for a client
	bool isValidForClient();
	// Determine if this associate is usable for storing details for our local identity
	bool isValidForLocalIdentity();
	// Determine if this associate is is usable for storing details for the zoo server
	bool isValidForServer();

	// Provide the list of network addresses used by this associate
	AddressList &addressList();

	// Provide the bluetooth address used by this associate
	// TODO: Merge into NetworkAddress?
	QBluetoothAddress bluetoothAddress() const;

	// Update the last seen timestamp. If when is 0 then current timestamp will be used
	void setLastSeen(quint64 when=0);
	// Provide the last seen timestamp
	quint64 lastSeen() const;
	// Provide the last initiated handshake timestamp
	quint64 lastInitiatedHandshake() const;
	// Provide the last adherent handshake timestamp
	quint64 lastAdherentHandshake() const;

public: // Pins management
	void clearPins();
	void addPin(QString pin);
	bool hasPin(QString pin);
	const QStringList &pins();

	// Trusts management
public:
	TrustList &trusts();

public: // Converting selectors to/from different formats
	PortableID toPortableID();
	QVariantMap toVariantMap();
	void fromVariantMap(const QVariantMap map);
	QString toString();
	QMap<QString, QString> toMap();
	QSharedPointer<Client> toClient(QSharedPointer<Node> node);

public: // Operators
	bool operator==(Associate &o);
	bool operator!=(Associate &o);

};


const QDebug &operator<<(QDebug &d, Associate &ass);

#endif
// ASSOCIATE_HPP
