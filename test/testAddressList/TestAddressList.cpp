#include "TestAddressList.hpp"


#include "basic/AddressList.hpp"
#include "basic/AddressEntry.hpp"

#include "comms/NetworkAddress.hpp"



void TestAddressList::testAddressEntry()
{
	const QString ip="10.1.2.3";
	const QHostAddress ha(ip);
	const quint16 port=8123;
	const QString description="first";
	const quint64 created=111;
	const quint64 lastSuccess=222;
	const quint64 lastError=333;
	const quint64 numSuccessful=444;
	const quint64 numErraneous=555;
	AddressEntry ae1(QSharedPointer<NetworkAddress>(new NetworkAddress(ha, port)), description, created, lastSuccess, lastError, numSuccessful, numErraneous);
	QVariantMap map=ae1.toVariantMap();
	QCOMPARE(map["address"].toMap(), NetworkAddress(ha,port).toVariantMap());
	QCOMPARE(map["description"].toString(), description);
	QCOMPARE(map["createdMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(created));
	QCOMPARE(map["lastSuccessMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(lastSuccess));
	QCOMPARE(map["lastErrorMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(lastError));
	QCOMPARE(map["numSuccessful"].toULongLong(), numSuccessful);
	QCOMPARE(map["numErraneous"].toULongLong(), numErraneous);
	AddressEntry ae1copy(map);
	QCOMPARE(ae1copy.address.isNull(), ae1.address.isNull());
	QCOMPARE(*ae1copy.address.data(), *ae1.address.data());
	QCOMPARE(ae1copy.description, ae1.description);
	QCOMPARE(ae1copy.created, ae1.created);
	QCOMPARE(ae1copy.lastSuccess, ae1.lastSuccess);
	QCOMPARE(ae1copy.lastError, ae1.lastError);
	QCOMPARE(ae1copy.numSuccessful, ae1.numSuccessful);
	QCOMPARE(ae1copy.numErraneous, ae1.numErraneous);

	QCOMPARE(ae1copy.toString(), ae1.toString());

	qDebug()<< "ORIG:" <<ae1.toString();
	qDebug()<< "COPY:" <<ae1copy.toString();

	QCOMPARE(ae1copy, ae1);
}


void TestAddressList::testAddressList()
{
	AddressList list;

	QSharedPointer<NetworkAddress> addr1(new NetworkAddress(QHostAddress("10.0.0.1"), 8123));will
	QSharedPointer<NetworkAddress> addr2(new NetworkAddress(QHostAddress("10.0.0.2"), 8123));
	QSharedPointer<NetworkAddress> addr3(new NetworkAddress(QHostAddress("10.0.0.3"), 8123));
	QSharedPointer<NetworkAddress> addr4(new NetworkAddress(QHostAddress("10.0.0.4"), 8123));

	QCOMPARE(addr1->isValid(), true);
	QCOMPARE(addr2->isValid(), true);
	QCOMPARE(addr3->isValid(), true);
	QCOMPARE(addr4->isValid(), true);

	QSharedPointer<AddressEntry> ae1(new AddressEntry(addr1, "first"));
	QSharedPointer<AddressEntry> ae2(new AddressEntry(addr2, "second"));
	QSharedPointer<AddressEntry> ae3(new AddressEntry(addr3, "third"));
	QSharedPointer<AddressEntry> ae4(new AddressEntry(addr4, "fourth"));

	list.add(ae1);
	list.add(ae2);
	list.add(ae3);
	list.add(ae4);

	QVariantList vlist=list.toVariantList();

	AddressList listcopy(vlist);

	QVariantList vlistcopy=listcopy.toVariantList();



	QCOMPARE(listcopy.size(),list.size());
	QCOMPARE(listcopy.toString(), list.toString());

	QCOMPARE(vlistcopy,vlist);




}


void TestAddressList::testAddressListScore()
{
	AddressList list;

	QSharedPointer<NetworkAddress> addr0(nullptr);
	QSharedPointer<NetworkAddress> addr1(new NetworkAddress(QHostAddress("10.0.0.1"), 8123));
	QSharedPointer<NetworkAddress> addr2(new NetworkAddress(QHostAddress("10.0.0.2"), 8123));
	QSharedPointer<NetworkAddress> addr3(new NetworkAddress(QHostAddress("10.0.0.3"), 8123));
	QSharedPointer<NetworkAddress> addr4(new NetworkAddress(QHostAddress("10.0.0.4"), 8123));

	QCOMPARE(addr1->isValid(), true);
	QCOMPARE(addr2->isValid(), true);
	QCOMPARE(addr3->isValid(), true);
	QCOMPARE(addr4->isValid(), true);

	QSharedPointer<AddressEntry> ae00(nullptr);
	QSharedPointer<AddressEntry> ae0(new AddressEntry(addr0, "zeroeth"));
	QSharedPointer<AddressEntry> ae1(new AddressEntry(addr1, "first"));
	QSharedPointer<AddressEntry> ae2(new AddressEntry(addr2, "second"));
	QSharedPointer<AddressEntry> ae3(new AddressEntry(addr3, "third"));
	QSharedPointer<AddressEntry> ae4(new AddressEntry(addr4, "fourth"));

	QCOMPARE(ae00.isNull(), true);
	QCOMPARE(ae0.isNull(), false);
	QCOMPARE(ae1.isNull(), false);
	QCOMPARE(ae2.isNull(), false);
	QCOMPARE(ae3.isNull(), false);
	QCOMPARE(ae4.isNull(), false);


	QCOMPARE(list.size(), (quint64)0);
	list.add(ae00);
	QCOMPARE(list.size(), (quint64)0);
	list.add(ae0);
	QCOMPARE(list.size(), (quint64)0);


	list.add(ae1);
	QCOMPARE(list.size(), (quint64)1);
	list.add(ae2);
	QCOMPARE(list.size(), (quint64)2);
	list.add(ae3);
	QCOMPARE(list.size(), (quint64)3);
	list.add(ae4);
	QCOMPARE(list.size(), (quint64)4);

	ae4->lastSuccess=2;
	QCOMPARE(list.highestScore().isNull(), false);
	qDebug()<<"AE4: "<<*ae4.data();
	qDebug()<<"COMP: "<<*list.highestScore().data();
	QCOMPARE(ae4, list.highestScore());

	ae3->lastSuccess=3;
	QCOMPARE(list.highestScore().isNull(), false);
	QCOMPARE(ae3, list.highestScore());

	ae2->lastSuccess=4;
	QCOMPARE(list.highestScore().isNull(), false);
	QCOMPARE(ae2, list.highestScore());

	ae1->lastSuccess=5;
	QCOMPARE(list.highestScore().isNull(), false);
	QCOMPARE(ae1, list.highestScore());

}



QTEST_MAIN(TestAddressList)
