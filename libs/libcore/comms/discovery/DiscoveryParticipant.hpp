#ifndef DISCOVERYPARTICIPANT_HPP
#define DISCOVERYPARTICIPANT_HPP

#include "DiscoveryRole.hpp"
#include "comms/ClientSignature.hpp"
#include "basic/NodeAssociate.hpp"

#include <QString>
#include <QVariantMap>
#include <QBluetoothAddress>
#include <QDateTime>
#include <QSharedPointer>


/*
 QSharedPointer<NodeAssociate> is an ephemeral representation of the data kept during
 node discovery by DiscoveryClient and DiscoveryServer.

 The actual trust that is gathered will be stored in the NodeAssociate member
 which acts as the persistable counterpart.

*/

//typedef QSharedPointer<NodeAssociate> QSharedPointer<NodeAssociate>;

/*
class QSharedPointer<NodeAssociate>{

	public:
//		NodeAssociate assoc;
		// An ephemeral list of pins used during discovery. Pins may be manually entered, generated from GPS coordinates etc.
		QStringList pins;
//		bool isDeleted;

	public:

		explicit QSharedPointer<NodeAssociate>();
		//explicit QSharedPointer<NodeAssociate>(QString publicKey, QString publicAddress, quint16 publicPort, QString localAddress, quint16 localPort, DiscoveryRole role, DiscoveryType type);
		explicit QSharedPointer<NodeAssociate>(QVariantMap map);
		explicit QSharedPointer<NodeAssociate>(const QSharedPointer<NodeAssociate> &o);
		virtual ~QSharedPointer<NodeAssociate>();

	public:



		void clearTrust();
		void addTrust(QString pin);

		bool operator==(const QSharedPointer<NodeAssociate> &o) const;

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

const QDebug &operator<<(QDebug &d, QSharedPointer<NodeAssociate> &part);

*/
#endif // DISCOVERYPARTICIPANT_HPP
