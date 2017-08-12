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


	void testSingle();
	void testMultiple();
	void testHandshake();
private slots:


	void testCarrier();


};


#endif // TESTCOMMSCHANNEL_HPP
