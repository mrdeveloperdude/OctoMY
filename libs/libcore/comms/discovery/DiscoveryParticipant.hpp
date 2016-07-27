#ifndef DISCOVERYPARTICIPANT_HPP
#define DISCOVERYPARTICIPANT_HPP

#include "DiscoveryRole.hpp"
#include "comms/ClientSignature.hpp"
#include "basic/NodeAssociate.hpp"

#include <QString>
#include <QVariantMap>
#include <QBluetoothAddress>
#include <QDateTime>


/*
 DiscoveryParticipant is an ephemeral representation of the data kept during
 node discovery by DiscoveryClient and DiscoveryServer.

 The actual trust that is gathered will be stored in the NodeAssociate member
 which acts as the persistable counterpart.

*/
class DiscoveryParticipant{

	public:
		NodeAssociate assoc;
		// An ephemeral list of pins used during discovery. Pins may be manually entered, generated from GPS coordinates etc.
		QStringList pins;
		bool isDeleted;

	public:

		explicit DiscoveryParticipant();
		//explicit DiscoveryParticipant(QString publicKey, QString publicAddress, quint16 publicPort, QString localAddress, quint16 localPort, DiscoveryRole role, DiscoveryType type);
		explicit DiscoveryParticipant(QVariantMap map);
		explicit DiscoveryParticipant(const DiscoveryParticipant &o);
		virtual ~DiscoveryParticipant();

	public:


		void clearPins();
		void addPin(QString pin);

		void clearTrust();
		void addTrust(QString pin);

		bool operator==(const DiscoveryParticipant &o) const;

		//bool isSet();

		bool isValidForServer();
		bool isValidForClient();

		QVariantMap toVariantMap();

		QString toString();

		ClientSignature clientSignature();

		bool updateFromServer(QVariantMap &map, bool trustedSource=false);

	public:

		inline QString id() const
		{
			OC_METHODGATE();
			return assoc.id();
		}


		inline DiscoveryType type() const
		{
			OC_METHODGATE();
			return assoc.type();
		}

		inline NodeAssociate &associate()
		{
			OC_METHODGATE();
			return assoc;
		}


		inline quint64 lastSeen() const
		{
			OC_METHODGATE();
			return assoc.lastSeen();
		}


};

const QDebug &operator<<(QDebug &d, DiscoveryParticipant &part);


#endif // DISCOVERYPARTICIPANT_HPP
