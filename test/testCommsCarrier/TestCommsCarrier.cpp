#include "TestCommsCarrier.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "comms/messages/MessageType.hpp"

#include "random/RNG.hpp"

#include "security/KeyStore.hpp"
#include "address/Associate.hpp"
#include "discovery/AddressBook.hpp"


#include "test/Utility.hpp"
#include "test/UDPTester.hpp"
#include "test/mock/MockCommsCarrierLog.hpp"

#include <QHostAddress>
#include <QSignalSpy>
#include <QFileInfo>


void TestCommsCarrier::testBasic()
{
	CommsCarrierUDP commsCarrier;
	MockCommsCarrierLog mocalo;
	commsCarrier.setHookCarrierSignals(mocalo, true);

	NetworkAddress fromAddress(QHostAddress::Any, 8124);
	NetworkAddress address(QHostAddress::Any, 8123);
	UDPTester dataSender(fromAddress, address);

	QCOMPARE(false, commsCarrier.isConnectionMaintained());
	commsCarrier.setListenAddress(address);
	NetworkAddress addressCopy=commsCarrier.address();
	QCOMPARE(address, addressCopy);

	commsCarrier.maintainConnection(true);
	QCOMPARE(true, commsCarrier.isConnectionMaintained());
	QCOMPARE(false, commsCarrier.isConnected());

	auto actualInterval=commsCarrier.setDesiredOpportunityInterval(100);
	auto tempInterval=commsCarrier.opportunityInterval();
	qDebug()<<"actualInterval="<<actualInterval<<", tempInterval="<<tempInterval<<"";
	QCOMPARE(actualInterval, tempInterval);

	QCOMPARE(false, commsCarrier.hasPendingData());

	// Send some data to the carrier
	const QByteArray txDatagram="Hello there";
	dataSender.send(txDatagram);

	// Wait for the receive to happen
	test::utility::testWaitForEvents();
//	QCOMPARE(true, coca.isConnected());
	QCOMPARE(true, commsCarrier.isConnectionMaintained());
	QCOMPARE(true, commsCarrier.hasPendingData());

	const qint64 supposedRxBytes=commsCarrier.pendingDataSize();
	QVERIFY(supposedRxBytes > 0);
	QCOMPARE(supposedRxBytes, txDatagram.size());

	// Try to receive the data
	QByteArray rxDatagram;
	rxDatagram.resize(static_cast<int>(commsCarrier.pendingDataSize()));
	QHostAddress rxHost;
	quint16 rxPort=0;
	const qint64 rxBytes=commsCarrier.readData(rxDatagram.data(), rxDatagram.size(), &rxHost, &rxPort);

	qDebug()<<"supposedRxBytes="<<supposedRxBytes<<", txDatagram.size()="<<txDatagram.size()<<", rxBytes="<<rxBytes<<", rxDatagram.size()="<<rxDatagram.size();

	// Check the received data
	QCOMPARE(fromAddress.port(), rxPort);
	QCOMPARE(rxDatagram.size(), txDatagram.size());
	QCOMPARE(rxDatagram.size(), rxBytes);
	QCOMPARE(supposedRxBytes, rxBytes);

	test::utility::testSleep(actualInterval*2, "Connection to be registered");

	// Check that this received data triggered the connection to be complete
	QCOMPARE(true, commsCarrier.isConnectionMaintained());
	QCOMPARE(true, commsCarrier.isConnected());

	//Wait for connection to time out
	const auto timeout=commsCarrier.connectionTimeout();
	test::utility::testSleep(timeout+1000, "Connection to time out");

	// Check that connection actually timed out
	QCOMPARE(false, commsCarrier.isConnected());

	// Switch off connection and verify that it actually is off
	QCOMPARE(true, commsCarrier.isConnectionMaintained());
	commsCarrier.maintainConnection(false);
	QCOMPARE(false, commsCarrier.isConnectionMaintained());

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

