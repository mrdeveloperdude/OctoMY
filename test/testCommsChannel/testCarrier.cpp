#include "TestCommsChannel.hpp"

#include "comms/CommsCarrier.hpp"
#include "comms/CommsCarrierUDP.hpp"

#include "MockCommsCarrierLog.hpp"
#include "MockCommsCarrier.hpp"

#include "../common/TestCommon.hpp"


static void testSleep(quint64 ms)
{
	const quint64 start=QDateTime::currentMSecsSinceEpoch();
	quint64 now=start;
	while(now<start+ms) {
		now=QDateTime::currentMSecsSinceEpoch();
		QCoreApplication::processEvents();
	}

}

void TestCommsChannel::testCarrier()
{
	CommsCarrierUDP coca;
	MockCommsCarrier moca;
	coca.setHookCarrierSignals(moca, true);

	NetworkAddress address(QHostAddress::LocalHost, 8123);

	QCOMPARE(false, coca.isStarted());
	coca.setListenAddress(address);
	coca.setStarted(true);
	QCOMPARE(true, coca.isStarted());

	NetworkAddress addressCopy=coca.address();

	QCOMPARE(address, addressCopy);

	auto actualInterval=coca.setDesiredOpportunityInterval(100);

	auto tempInterval=coca.opportunityInterval();

	QCOMPARE(actualInterval, tempInterval);

	QCOMPARE(false, coca.isConnected());

	testSleep(200);

	QCOMPARE(false, coca.isConnected());
	QCOMPARE(true, coca.isStarted());


	QByteArray datagram="Hello there";
	NetworkAddress fromAddress(QHostAddress::LocalHost, 8124);
	qint64 written=coca.writeData(datagram, fromAddress);
	QCOMPARE(written, (qint64)datagram.size());
	testSleep(2000);

//	QCOMPARE(true, coca.isConnected());

	//Wait for connection to time out
	const auto timeout=coca.connectionTimeout();
	testSleep(timeout+100);

	QCOMPARE(true, coca.isStarted());
	coca.setStarted(false);
	QCOMPARE(false, coca.isStarted());
	QCOMPARE(false, coca.isConnected());

/*
	qint64 writeData(const QByteArray &datagram, const NetworkAddress &address);
	qint64 readData(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr);

	bool hasPendingData();
	qint64 pendingDataSize();

	QString errorString();

	quint64 minimalPacketRate();
	quint64 maximalPacketRate();
*/

}
