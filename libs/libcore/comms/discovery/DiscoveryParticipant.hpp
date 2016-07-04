#ifndef DISCOVERYPARTICIPANT_HPP
#define DISCOVERYPARTICIPANT_HPP

#include "DiscoveryRole.hpp"
#include "comms/ClientSignature.hpp"

#include <QString>
#include <QVariantMap>
#include <QBluetoothAddress>

/*

  DiscoveryParticipant is a means to store the address info and trust level
  for one of Node's friends in a manner suitable for persistence via
  DiscoveryClientStore.

*/

struct NetworkAddress{
		QHostAddress ip;
		quint16 port;
};


class NodeAssociate{
	public:
		// The full text of the RSA public key for this participant
		QString publicKey;
		// The deterministaclly generated ID for this participant (a.k.a. Soul ID)
		// NOTE: This is simply the SHA1 of the pubkey
		QString ID;
	public:
		// A list of the trusts assigned to this participant
		QStringList trusts;
		// The most updated trusted type and role for this participant
		DiscoveryRole role;
		DiscoveryType type;
		// When was the last trusted sighting of this associate?
		quint64 lastSeenMS;

	public:
		NetworkAddress publicNetworkAddress;
		NetworkAddress localNetworkAddress;
		QBluetoothAddress bluetoothAddress;
		QByteArray NFCAddress;
};

class DiscoveryParticipant{

	public:
		// The full text of the RSA public key for this participant
		QString publicKey;
		// The deterministaclly generated ID for this participant (a.k.a. Soul ID)
		// NOTE: This is simply the SHA1 of the pubkey
		QString ID;
		// An ephemeral list of pins used during discovery. Pins may be manually entered, generated from GPS coordinates etc.
		QStringList pins;
		// A list of the trusts assigned to this participant
		QStringList trusts;
		// The most updated trusted type and role for this participant
		DiscoveryRole role;
		DiscoveryType type;

		quint64 lastTime;

		// The most updated trusted contact info for participant
	public:

		NodeAssociate assoc;

		QString publicAddress;
		quint16 publicPort;
		QString localAddress;
		quint16 localPort;
		QBluetoothAddress bluetoothAddress;


	public:

		explicit DiscoveryParticipant();
		explicit DiscoveryParticipant(QString publicKey, QString publicAddress, quint16 publicPort, QString localAddress, quint16 localPort, DiscoveryRole role, DiscoveryType type);
		explicit DiscoveryParticipant(QVariantMap map);
		explicit DiscoveryParticipant(const DiscoveryParticipant &o);
		virtual ~DiscoveryParticipant();

	public:

		void clearTrust();
		void clearPins();

		void addTrust(QString pin);
		void addPin(QString pin);

		void generateID();

		bool operator==(const DiscoveryParticipant &o) const;

		bool isSet();

		bool isValidServer();
		bool isValidClient();

		QVariantMap toVariantMap();

		QString toString();


		QString fullPublicAddress();
		QString fullLocalAddress();


		ClientSignature clientSignature();

};


#endif // DISCOVERYPARTICIPANT_HPP
