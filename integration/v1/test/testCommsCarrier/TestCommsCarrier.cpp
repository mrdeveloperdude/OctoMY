#include "TestCommsCarrier.hpp"



#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"



#include "utility/Standard.hpp"
#include "comms/messages/MessageType.hpp"

#include "rng/RNG.hpp"

#include "security/KeyStore.hpp"
#include "basic/Associate.hpp"
#include "discovery/AddressBook.hpp"

#include "../common/TestCommon.hpp"
#include "../common/Utilities.hpp"
#include "../common/UDPTester.hpp"

#include "../common/mock/MockCommsCarrierLog.hpp"

#include <QHostAddress>
#include <QSignalSpy>
#include <QFileInfo>


void TestCommsCarrier::testBasic()
{
	CommsCarrierUDP coca;
	MockCommsCarrierLog mocalo;
	coca.setHookCarrierSignals(mocalo, true);

	NetworkAddress fromAddress(QHostAddress::Any, 8124);
	NetworkAddress address(QHostAddress::Any, 8123);
	UDPTester dataSender(fromAddress, address);

	QCOMPARE(false, coca.isStarted());
	coca.setListenAddress(address);
	NetworkAddress addressCopy=coca.address();
	QCOMPARE(address, addressCopy);

	QCOMPARE(true, coca.setStarted(true));
	QCOMPARE(true, coca.isStarted());
	QCOMPARE(false, coca.isConnected());

	auto actualInterval=coca.setDesiredOpportunityInterval(100);
	auto tempInterval=coca.opportunityInterval();
	qDebug()<<"actualInterval="<<actualInterval<<", tempInterval="<<tempInterval<<"";
	QCOMPARE(actualInterval, tempInterval);

	QCOMPARE(false, coca.hasPendingData());

	// Send some data to the carrier
	const QByteArray txDatagram="Hello there";
	dataSender.send(txDatagram);

	// Wait for the receive to happen
	testWaitForEvents();
//	QCOMPARE(true, coca.isConnected());
	QCOMPARE(true, coca.isStarted());
	QCOMPARE(true, coca.hasPendingData());

	const qint64 supposedRxBytes=coca.pendingDataSize();
	QVERIFY(supposedRxBytes > 0);
	QCOMPARE(supposedRxBytes, txDatagram.size());

	// Try to receive the data
	QByteArray rxDatagram;
	rxDatagram.resize(static_cast<int>(coca.pendingDataSize()));
	QHostAddress rxHost;
	quint16 rxPort=0;
	const qint64 rxBytes=coca.readData(rxDatagram.data(), rxDatagram.size(), &rxHost, &rxPort);

	qDebug()<<"supposedRxBytes="<<supposedRxBytes<<", txDatagram.size()="<<txDatagram.size()<<", rxBytes="<<rxBytes<<", rxDatagram.size()="<<rxDatagram.size();

	// Check the received data
	QCOMPARE(fromAddress.port(), rxPort);
	QCOMPARE(rxDatagram.size(), txDatagram.size());
	QCOMPARE(rxDatagram.size(), rxBytes);
	QCOMPARE(supposedRxBytes, rxBytes);

	testSleep(actualInterval*2, "Connection to be registered");

	// Check that this received data triggered the connection to be complete
	QCOMPARE(true, coca.isStarted());
	QCOMPARE(true, coca.isConnected());

	//Wait for connection to time out
	const auto timeout=coca.connectionTimeout();
	testSleep(timeout+1000, "Connection to time out");

	// Check that connection actually timed out
	QCOMPARE(false, coca.isConnected());

	// Switch off connection and verify that it actually is off
	QCOMPARE(true, coca.isStarted());
	coca.setStarted(false);
	QCOMPARE(false, coca.isStarted());

	/*
		qint64 writeData(const QByteArray &datagram, const NetworkAddress &address);

		bool hasPendingData();
		qint64 pendingDataSize();

		QString errorString();

		quint64 minimalPacketRate();
		quint64 maximalPacketRate();
	*/

}


void TestCommsCarrier::testConnection()
{

}

OC_TEST_MAIN(test, TestCommsCarrier)

