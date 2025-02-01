#include "TestCommsCarrier.hpp"

#include "comms/address/CarrierAddressUDP.hpp"
#include "comms/carriers/CommsCarrierLocal.hpp"
#include "comms/carriers/CommsCarrierUDP.hpp"
#include "uptime/New.hpp"

#include <QFileInfo>
#include <QHostAddress>
#include <QObject>
#include <QSharedPointer>
#include <QSignalSpy>

#include "test/Utility.hpp"
#include "test/UDPTester.hpp"
#include "test/mock/MockCommsCarrierLog.hpp"

#include "CommsCarrierTestWidget.hpp"
/*
class CommsCarrierTester:public QObject
{
	Q_OBJECT
	
public:
	QString mName;
	QHostAddress mMyAddress;
	quint16 mMyPort;
	
	quint16 mBasePort;
	quint16 mPortRange;
	QSharedPointer<CommsCarrierUDP> mCarrier;
	QSharedPointer<KeyStore> mKeyStore;
	QSharedPointer<AddressBook> mAssociates;
	QSharedPointer<CommsChannel> mCc;
	quint16 mTestCount;
	
	RNG *mRng;
	
public:
	explicit CommsCarrierTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, QSharedPointer<KeyStore> keyStore, QSharedPointer<AddressBook> peers, QObject *parent=nullptr);
	virtual ~CommsCarrierTester() {}
	
public:
	QString toString();
	
public slots:
	void startSendTest();
	void onError(QString);
	void onReadyRead();
	
signals:
	void finished();
	
};
*/



void TestCommsCarrier::testBasic()
{
	CommsCarrierUDP commsCarrier;
	MockCommsCarrierLog mocalo;
	commsCarrier.setHookCarrierSignals(mocalo, true);

	auto fromAddress = QSharedPointer<CarrierAddressUDP>::create(QHostAddress::Any, 8124);
	auto address = QSharedPointer<CarrierAddressUDP>::create(QHostAddress::Any, 8123);
	UDPTester dataSender(fromAddress, address);

	QCOMPARE(commsCarrier.isConnectionMaintained(), false);

	commsCarrier.setListenAddress(qSharedPointerDynamicCast<CarrierAddress>(address));
	auto addressCopy = commsCarrier.address();
	QCOMPARE(address, addressCopy);

	commsCarrier.maintainConnection(true);
	QCOMPARE(commsCarrier.isConnectionMaintained(), true);
	QCOMPARE(commsCarrier.isConnected(), false);

	auto actualInterval=commsCarrier.setDesiredOpportunityInterval(100);
	auto tempInterval=commsCarrier.opportunityInterval();
	qDebug()<<"actualInterval="<<actualInterval<<", tempInterval="<<tempInterval<<"";
	QCOMPARE(actualInterval, tempInterval);

	QCOMPARE(commsCarrier.hasPendingData(), false);

	// Send some data to the carrier
	const QByteArray txDatagram="Hello there";
	dataSender.send(txDatagram);

	// Wait for the receive to happen
	test::utility::testWaitForEvents();
//	QCOMPARE(coca.isConnected(), true);
	QCOMPARE(commsCarrier.isConnectionMaintained(), true);
	QCOMPARE(commsCarrier.hasPendingData(), true);

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
	QCOMPARE(fromAddress->port(), rxPort);
	QCOMPARE(rxDatagram.size(), txDatagram.size());
	QCOMPARE(rxDatagram.size(), rxBytes);
	QCOMPARE(supposedRxBytes, rxBytes);

	test::utility::testSleep(actualInterval*2, "Connection to be registered");

	// Check that this received data triggered the connection to be complete
	QCOMPARE(commsCarrier.isConnectionMaintained(), true);
	QCOMPARE(commsCarrier.isConnected(), true);

	//Wait for connection to time out
	const auto timeout=commsCarrier.connectionTimeout();
	test::utility::testSleep(timeout+1000, "Connection to time out");

	// Check that connection actually timed out
	QCOMPARE(commsCarrier.isConnected(), false);

	// Switch off connection and verify that it actually is off
	QCOMPARE(commsCarrier.isConnectionMaintained(), true);
	commsCarrier.maintainConnection(false);
	QCOMPARE(commsCarrier.isConnectionMaintained(), false);

	/*
		qint64 writeData(const QByteArray &datagram, const CarrierAddress &address);

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


void TestCommsCarrier::testInteractive()
{
	Q_INIT_RESOURCE(icons);
	const auto editor = OC_NEW CommsCarrierTestWidget();
	auto *cA = OC_NEW CommsCarrierLocal();
	auto *cB = OC_NEW CommsCarrierLocal();
	MockCommsCarrierLog mocaloA(nullptr, "Alice");
	MockCommsCarrierLog mocaloB(nullptr, "Bob");
	connect(&mocaloA, &MockCommsCarrierLog::logMessage, editor, &CommsCarrierTestWidget::log);
	connect(&mocaloB, &MockCommsCarrierLog::logMessage, editor, &CommsCarrierTestWidget::log);
	cA->setHookCarrierSignals(mocaloA, true);
	cB->setHookCarrierSignals(mocaloB, true);
	connect(editor, &CommsCarrierTestWidget::configureAImpl, this, [=](){
		cA->configure();
		editor->log("Alice", QString("configure"));
	});
	connect(editor, &CommsCarrierTestWidget::configureBImpl, this, [=](){
		cB->configure();
		editor->log("Bob", QString("configure"));
	});
	connect(editor, &CommsCarrierTestWidget::connectAImpl, this, [=](bool con){
		cA->maintainConnection(con);
		editor->log("Alice", QString("maintain con: %1").arg(con?"YES":"NO"));
	});
	connect(editor, &CommsCarrierTestWidget::connectBImpl, this, [=](bool con){
		cB->maintainConnection(con);
		editor->log("Bob", QString("maintain con: %1").arg(con?"YES":"NO"));
	});
	connect(editor, &CommsCarrierTestWidget::activateAImpl, this, [=](bool con){
		cA->activate(con);
		editor->log("Alice", QString("activate: %1").arg(con?"YES":"NO"));
	});
	connect(editor, &CommsCarrierTestWidget::activateBImpl, this, [=](bool con){
		cB->activate(con);
		editor->log("Bob", QString("activate(con);: %1").arg(con?"YES":"NO"));
	});
	connect(editor, &CommsCarrierTestWidget::intervalChangeAImpl, this, [=](int interval){
		const auto actualInterval = cA->setDesiredOpportunityInterval(interval);
		editor->log("Alice", QString("interval: %1 (%2)").arg(interval).arg(actualInterval));
	});
	connect(editor, &CommsCarrierTestWidget::intervalChangeBImpl, this, [=](int interval){
		const auto actualInterval = cB->setDesiredOpportunityInterval(interval);
		editor->log("Bob", QString("interval: %1 (%2)").arg(interval).arg(actualInterval));
	});
	editor->show();
	editor->setWindowTitle("CommsCarrierTestWidget");
	editor->setMinimumSize(300, 200);
	test::utility::waitForUIEnd(editor);
}



OC_TEST_MAIN(test, TestCommsCarrier)

