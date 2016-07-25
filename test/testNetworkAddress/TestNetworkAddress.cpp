#include "TestNetworkAddress.hpp"


#include "basic/NetworkAddress.hpp"

#include <QVariantMap>


#define scrutinize(na, expectedValid, expectedAddress, expectedPort) \
	QVERIFY( nullptr != (na) ); \
	qDebug()<< "----" <<(#na)<<(na)->toString(); \
	QCOMPARE((na)->isValid(), (expectedValid)); \
	QCOMPARE((na)->ip(), (expectedAddress)); \
	QCOMPARE((na)->port(), (expectedPort))


#define DECEND(na) \
	(na)->setIP(expectedEmptyAddress); \
	scrutinize((na), false, expectedEmptyAddress, expectedValidPort); \
	(na)->setPort(expectedEmptyPort); \
	scrutinize((na), false, expectedEmptyAddress, expectedEmptyPort)

#define ASCEND(na) \
	(na)->setIP(expectedValidAddress); \
	scrutinize((na), false, expectedValidAddress, expectedEmptyPort); \
	(na)->setPort(expectedValidPort); \
	scrutinize((na), true, expectedValidAddress, expectedValidPort)


/*
void scrutinize(NetworkAddress *na, bool expectedValid, QHostAddress expectedAddress, quint16 expectedPort)
{
	QVERIFY(nullptr!=na);
	QCOMPARE(na->isValid(), expectedValid);
	QCOMPARE(na->ip(), expectedAddress);
	QCOMPARE(na->port(), expectedPort);
}
*/


TestNetworkAddress::TestNetworkAddress()
	: expectedEmptyPort(0)
	, expectedValidAddress("127.0.0.1")
	, expectedValidPort(8128)
{

}


void TestNetworkAddress::initTestCase()
{
}

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestNetworkAddress::testInitial(){
	QVERIFY(expectedEmptyAddress.isNull());
	QCOMPARE(expectedEmptyPort, (quint16)0);
	QVERIFY(!expectedValidAddress.isNull());
	QCOMPARE(expectedValidPort, (quint16)8128);
}


void TestNetworkAddress::testWithoutConstructorArguments()
{
	NetworkAddress *naHeap=new NetworkAddress();
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	NetworkAddress naStack;
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testEmptyMapConstructorArguments()
{
	QVariantMap emptyMap;
	NetworkAddress *naHeap=new NetworkAddress(emptyMap);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	NetworkAddress naStack(emptyMap);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testInvalidMapConstructorArguments()
{
	QVariantMap invalidMap;
	invalidMap["ip"]="invalidIP";
	invalidMap["port"]="invalidPort";
	NetworkAddress *naHeap=new NetworkAddress(invalidMap);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	NetworkAddress naStack(invalidMap);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testValidMapConstructorArguments()
{
	QVariantMap validMap;
	validMap["ip"]=expectedValidAddress.toString();
	qDebug()<<validMap["ip"].toString();
	validMap["port"]=QString::number(expectedValidPort);
	NetworkAddress *naHeap=new NetworkAddress(validMap);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);
	NetworkAddress naStack(validMap);
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	DECEND(naHeap);
	ASCEND(naHeap);
	DECEND(&naStack);
	ASCEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testEmptyDirectConstructorArguments()
{
	QHostAddress emptyAddress;
	quint16 invalidPort=0;
	NetworkAddress *naHeap=new NetworkAddress(emptyAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	NetworkAddress naStack(emptyAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testInvalidDirectConstructorArguments()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	NetworkAddress *naHeap=new NetworkAddress(invalidAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	NetworkAddress naStack(invalidAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}

void TestNetworkAddress::testValidDirectConstructorArguments()
{
	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	NetworkAddress *naHeap=new NetworkAddress(validAddress, validPort);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);
	NetworkAddress naStack(validAddress, validPort);
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	DECEND(naHeap);
	ASCEND(naHeap);
	DECEND(&naStack);
	ASCEND(&naStack);

	delete naHeap;
}


void TestNetworkAddress::testAssignment1()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	NetworkAddress *naHeap=new NetworkAddress(invalidAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);

	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	NetworkAddress naStack(validAddress, validPort);
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	naStack=*naHeap;
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	QVERIFY(*naHeap == naStack);
	QVERIFY(naHeap != &naStack);

	ASCEND(naHeap);

	QVERIFY(*naHeap != naStack);
	QVERIFY(naHeap != &naStack);

	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	delete naHeap;
}



void TestNetworkAddress::testAssignment2()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	NetworkAddress naStack(invalidAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	NetworkAddress *naHeap=new NetworkAddress(validAddress, validPort);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);

	naStack=*naHeap;
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	QVERIFY(*naHeap == naStack);
	QVERIFY(naHeap != &naStack);

	DECEND(naHeap);

	QVERIFY(*naHeap != naStack);
	QVERIFY(naHeap != &naStack);

	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	delete naHeap;
}


void TestNetworkAddress::testIsValid()
{
	quint16 validPort=8128;
	QHostAddress validLoopbackAddress("127.0.0.1");
	NetworkAddress *naHeap=new NetworkAddress(validLoopbackAddress, validPort);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);

	QVERIFY(naHeap->isValid(true,true));
	QVERIFY(!naHeap->isValid(false,true));

	QHostAddress validMulticastAddress("224.0.0.1"); // https://en.wikipedia.org/wiki/Multicast_address
	naHeap->setIP(validMulticastAddress);

	QVERIFY(naHeap->isValid(true,true));
	QVERIFY(!naHeap->isValid(true,false));

	delete naHeap;

}

QTEST_MAIN(TestNetworkAddress)
