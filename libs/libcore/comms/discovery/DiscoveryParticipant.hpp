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

	public:

		explicit DiscoveryParticipant();
		explicit DiscoveryParticipant(QString publicKey, QString publicAddress, quint16 publicPort, QString localAddress, quint16 localPort, DiscoveryRole role, DiscoveryType type);
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

		bool isValidServer();
		bool isValidClient();

		QVariantMap toVariantMap();

		QString toString();


		//QString fullPublicAddress();
		//QString fullLocalAddress();


		ClientSignature clientSignature();

	public:

		inline QString id() const
		{
			return assoc.id();
		}


		inline DiscoveryType type() const
		{
			return assoc.type();
		}

		inline NodeAssociate &associate()
		{
			return assoc;
		}


};


#endif // DISCOVERYPARTICIPANT_HPP
