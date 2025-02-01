#include "TestCarrierAddress.hpp"

#include "test/Common.hpp"

#include "comms/address/CarrierAddress.hpp"
#include "uptime/New.hpp"

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
void scrutinize(CarrierAddress *na, bool expectedValid, QHostAddress expectedAddress, quint16 expectedPort)
{
	QVERIFY(nullptr!=na);
	QCOMPARE(na->isValid(), expectedValid);
	QCOMPARE(na->ip(), expectedAddress);
	QCOMPARE(na->port(), expectedPort);
}
*/


TestCarrierAddress::TestCarrierAddress()
	: expectedEmptyPort(0)
	, expectedValidAddress("127.0.0.1")
	, expectedValidPort(8128)
{

}


void TestCarrierAddress::initTestCase()
{
}


void TestCarrierAddress::testInitial()
{
	QVERIFY(expectedEmptyAddress.isNull());
	QCOMPARE(expectedEmptyPort, (quint16)0);
	QVERIFY(!expectedValidAddress.isNull());
	QCOMPARE(expectedValidPort, (quint16)8128);
}

// TODO: Fix after CarrierAddress refactor
/*
void TestCarrierAddress::testWithoutConstructorArguments()
{
	CarrierAddress *naHeap=OC_NEW CarrierAddress();
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	CarrierAddress naStack;
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testEmptyMapConstructorArguments()
{
	QVariantMap emptyMap;
	CarrierAddress *naHeap=OC_NEW CarrierAddress(emptyMap);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	CarrierAddress naStack(emptyMap);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testInvalidMapConstructorArguments()
{
	QVariantMap invalidMap;
	invalidMap["ip"]="invalidIP";
	invalidMap["port"]="invalidPort";
	CarrierAddress *naHeap=OC_NEW CarrierAddress(invalidMap);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	CarrierAddress naStack(invalidMap);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testValidMapConstructorArguments()
{
	QVariantMap validMap;
	validMap["ip"]=expectedValidAddress.toString();
	qDebug()<<validMap["ip"].toString();
	validMap["port"]=QString::number(expectedValidPort);
	CarrierAddress *naHeap=OC_NEW CarrierAddress(validMap);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);
	CarrierAddress naStack(validMap);
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	DECEND(naHeap);
	ASCEND(naHeap);
	DECEND(&naStack);
	ASCEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testEmptyDirectConstructorArguments()
{
	QHostAddress emptyAddress;
	quint16 invalidPort=0;
	CarrierAddress *naHeap=OC_NEW CarrierAddress(emptyAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	CarrierAddress naStack(emptyAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testInvalidDirectConstructorArguments()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	CarrierAddress *naHeap=OC_NEW CarrierAddress(invalidAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);
	CarrierAddress naStack(invalidAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	ASCEND(naHeap);
	DECEND(naHeap);
	ASCEND(&naStack);
	DECEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testValidDirectConstructorArguments()
{
	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	CarrierAddress *naHeap=OC_NEW CarrierAddress(validAddress, validPort);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);
	CarrierAddress naStack(validAddress, validPort);
	scrutinize(&naStack, true, expectedValidAddress, expectedValidPort);

	DECEND(naHeap);
	ASCEND(naHeap);
	DECEND(&naStack);
	ASCEND(&naStack);

	delete naHeap;
}


void TestCarrierAddress::testAssignment1()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	CarrierAddress *naHeap=OC_NEW CarrierAddress(invalidAddress, invalidPort);
	scrutinize(naHeap, false, expectedEmptyAddress, expectedEmptyPort);

	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	CarrierAddress naStack(validAddress, validPort);
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


void TestCarrierAddress::testAssignment2()
{
	QHostAddress invalidAddress("invalidIP");
	quint16 invalidPort=0;
	CarrierAddress naStack(invalidAddress, invalidPort);
	scrutinize(&naStack, false, expectedEmptyAddress, expectedEmptyPort);

	quint16 validPort=8128;
	QHostAddress validAddress("127.0.0.1");
	CarrierAddress *naHeap=OC_NEW CarrierAddress(validAddress, validPort);
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


void TestCarrierAddress::testIsValid()
{
	quint16 validPort=8128;
	QHostAddress validLoopbackAddress("127.0.0.1");
	CarrierAddress *naHeap=OC_NEW CarrierAddress(validLoopbackAddress, validPort);
	scrutinize(naHeap, true, expectedValidAddress, expectedValidPort);

	QVERIFY(naHeap->isValid(true,true));
	QVERIFY(!naHeap->isValid(false,true));

	QHostAddress validMulticastAddress("224.0.0.1"); // https://en.wikipedia.org/wiki/Multicast_address
	naHeap->setIP(validMulticastAddress);

	QVERIFY(naHeap->isValid(true,true));
	QVERIFY(!naHeap->isValid(true,false));

	delete naHeap;

}


void TestCarrierAddress::testToFromString()
{
	qDebug()<<"TO FROM STRING!";
	const QString validAddressString="127.0.0.1";
	const QHostAddress validAddress(validAddressString);
	const quint16 validPort=8128;
	const QString validPortString=QString::number(validPort);
	const QString full=validAddressString+":"+validPortString;
	const QString onlyAddress=validAddressString+":0";
	const QString onlyPort=":"+validPortString;
	const QString unInitialized(":0");

	CarrierAddress na;
	QString unInitializedCheck=na.toString();


	QCOMPARE(unInitialized, unInitializedCheck);

	na.fromString(full);
	QString fullCheck=na.toString();
	QCOMPARE(full, fullCheck);

	na.fromString(onlyAddress);
	QString onlyAddressCheck=na.toString();
	QCOMPARE(onlyAddress, onlyAddressCheck);

	na.fromString(onlyPort);
	QString onlyPortCheck=na.toString();
	QCOMPARE(onlyPort, onlyPortCheck);

}
*/

OC_TEST_MAIN(test, TestCarrierAddress)
