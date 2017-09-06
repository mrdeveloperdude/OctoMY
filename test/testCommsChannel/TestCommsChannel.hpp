#ifndef TESTCOMMSCHANNEL_HPP
#define TESTCOMMSCHANNEL_HPP

#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "CommsSignalLogger.hpp"


#include "utility/Standard.hpp"
#include "comms/messages/MessageType.hpp"

#include "rng/RNG.hpp"

#include "security/KeyStore.hpp"
#include "basic/NodeAssociate.hpp"
#include "discovery/NodeAssociateStore.hpp"


#include <QTest>
#include <QHostAddress>
#include <QSignalSpy>
#include <QFileInfo>


class TestCommsChannel:public QObject
{
	Q_OBJECT

public:

	//ok
	void testMultiple();

	void testCarrier();

	void testSingle();

private slots:

	void testCommsMock();

};


void heading(QString msg, QString ch="_");
QSharedPointer<NodeAssociate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, DiscoveryRole role, DiscoveryType type );

#endif // TESTCOMMSCHANNEL_HPP
