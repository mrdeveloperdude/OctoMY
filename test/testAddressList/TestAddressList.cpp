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
	AddressEntry ae1(NetworkAddress(ha, port), description, created, lastSuccess, lastError, numSuccessful, numErraneous);
	QVariantMap map=ae1.toVariantMap();
	QCOMPARE(map["address"].toMap(), NetworkAddress(ha,port).toVariantMap());
	QCOMPARE(map["description"].toString(), description);
	QCOMPARE(map["createdMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(created));
	QCOMPARE(map["lastSuccessMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(lastSuccess));
	QCOMPARE(map["lastErrorMS"].toDateTime(), QDateTime::fromMSecsSinceEpoch(lastError));
	QCOMPARE(map["numSuccessful"].toULongLong(), numSuccessful);
	QCOMPARE(map["numErraneous"].toULongLong(), numErraneous);
	AddressEntry ae1copy(map);
	QCOMPARE(ae1copy.address, ae1.address);
	QCOMPARE(ae1copy.address.isValid(), ae1.address.isValid());
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

	NetworkAddress addr1(QHostAddress("10.0.0.1"), 8123);
	NetworkAddress addr2(QHostAddress("10.0.0.2"), 8123);
	NetworkAddress addr3(QHostAddress("10.0.0.3"), 8123);
	NetworkAddress addr4(QHostAddress("10.0.0.4"), 8123);

	QCOMPARE(addr1.isValid(), true);
	QCOMPARE(addr2.isValid(), true);
	QCOMPARE(addr3.isValid(), true);
	QCOMPARE(addr4.isValid(), true);

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

	NetworkAddress addr1(QHostAddress("10.0.0.1"), 8123);
	NetworkAddress addr2(QHostAddress("10.0.0.2"), 8123);
	NetworkAddress addr3(QHostAddress("10.0.0.3"), 8123);
	NetworkAddress addr4(QHostAddress::Any, 0);

	QCOMPARE(addr1.isValid(), true);
	QCOMPARE(addr2.isValid(), true);
	QCOMPARE(addr3.isValid(), true);
	QCOMPARE(addr4.isValid(), false);

	QSharedPointer<AddressEntry> ae00(nullptr);
	QSharedPointer<AddressEntry> ae1(new AddressEntry(addr1, "first"));
	QSharedPointer<AddressEntry> ae2(new AddressEntry(addr2, "second"));
	QSharedPointer<AddressEntry> ae3(new AddressEntry(addr3, "third"));
	QSharedPointer<AddressEntry> ae4(new AddressEntry(addr4, "invalid-4"));

	QCOMPARE(ae00.isNull(), true);
	QCOMPARE(ae1.isNull(), false);
	QCOMPARE(ae2.isNull(), false);
	QCOMPARE(ae3.isNull(), false);
	QCOMPARE(ae4.isNull(), false);


	QCOMPARE(list.size(), (int)0);
	list.add(ae00);
	QCOMPARE(list.size(), (int)0);
	QCOMPARE(list.isValid(true), false);
	QCOMPARE(list.isValid(false), false);

	list.add(ae1);
	QCOMPARE(list.size(), (int)1);
	QCOMPARE(list.isValid(true), true);
	QCOMPARE(list.isValid(false), true);

	list.add(ae2);
	QCOMPARE(list.size(), (int)2);
	QCOMPARE(list.isValid(true), true);
	QCOMPARE(list.isValid(false), true);

	list.add(ae3);
	QCOMPARE(list.size(), (int)3);
	QCOMPARE(list.isValid(true), true);
	QCOMPARE(list.isValid(false), true);

	list.add(ae4);
	QCOMPARE(list.size(), (int)4);
	QCOMPARE(list.isValid(true), false);
	QCOMPARE(list.isValid(false), true);


	//NOTE: ae4 is invalid thus we shanln't get a good result here
	ae4->lastSuccess=2;
	auto hs=list.highestScore();
	QCOMPARE(hs.isNull(), true);

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




void TestAddressList::testAddressListStorage()
{
	AddressList list;
	for(int i=0; i<4; ++i) {
		{
			QVariantList vlist=list.toVariantList();
			AddressList list2(vlist);
			//TODO: Implement operato== for AddressList if it is ever needed
			//QCOMPARE(list,list2);
			AddressList list3;
			list3.fromVariantList(vlist);
			//TODO: Implement operato== for AddressList if it is ever needed
			//QCOMPARE(list,list3);
		}

		NetworkAddress addr1(QHostAddress("10.0.0."+QString::number(i)), 8123);
		QCOMPARE(addr1.isValid(), true);
		list.merge(addr1);
		QCOMPARE(list.size(), (int)(i+1));
		QCOMPARE(list.isValid(true), true);
		QCOMPARE(list.isValid(false), true);
	}
}


QTEST_MAIN(TestAddressList)
