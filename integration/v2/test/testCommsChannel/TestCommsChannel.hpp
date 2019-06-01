#ifndef TESTCOMMSCHANNEL_HPP
#define TESTCOMMSCHANNEL_HPP

#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "CommsSignalLogger.hpp"

#include "comms/messages/MessageType.hpp"

#include "random/RNG.hpp"

#include "security/KeyStore.hpp"
#include "address/Associate.hpp"
#include "discovery/AddressBook.hpp"


#include "../common_test/Common_test.hpp"
#include <QHostAddress>
#include <QSignalSpy>
#include <QFileInfo>


class TestCommsChannel:public QObject
{
	Q_OBJECT

public:
	void testMultiple();
	void testCommsMock();

private slots:
	void testSingle();

};


QSharedPointer<Associate> generatePart(QString name, QSharedPointer<Key> key, NetworkAddress addr, NodeRole role, NodeType type );

#endif
// TESTCOMMSCHANNEL_HPP
