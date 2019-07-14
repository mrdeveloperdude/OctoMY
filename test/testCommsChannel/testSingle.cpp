 #include "TestCommsChannel.hpp"

#include "../common_test/mock/MockCourier.hpp"

#include "../common_test/Utility_test.hpp"

#include "utility/time/HumanTime.hpp"

#include <QTest>
/*
Basic test that sets up two CommsChannels ( A & B ) and have them sending data via couriers to eachother.

Since the CommsChannel architecture is a bit involved, having a basic test is useful both for
learning/remembering the intricacies and for debugging and testing basic features.
*/

void TestCommsChannel::testSingle()
{
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QByteArray>("QByteArray");
	const QHostAddress local("127.0.0.1");
	const quint16 basePort=54345;
	const qint32 maxSends=10;
	const qint32 maxRecs=10;
	const qint16 keyBits=128;
	const QCryptographicHash::Algorithm hashAlgo=QCryptographicHash::Md5;
	const KeySecurityPolicy quickAndInsecurePolicy(keyBits, hashAlgo);
	const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();


	test::utility::testHeading("PARAMETER SUMMARY");
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + keyBits:   "<<keyBits;
	qDebug()<<"  + hashAlgo:  "<<hashAlgo;
	qDebug()<<"  + now:       "<<now;

	test::utility::testHeading("INITIALIZING ID FOR PARTY A");/////////////////////////////////////////////
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()) {
		QVERIFY(keyStoreFileA.remove());
	}
	QVERIFY(!keyStoreFileA.exists());

	QSharedPointer<KeyStore> keyStoreA(OC_NEW KeyStore());
	keyStoreA->configure(keyStoreFilenameA, true, quickAndInsecurePolicy);


	QVERIFY(!keyStoreFileA.exists());

	keyStoreA->waitForSync();
	QVERIFY(keyStoreFileA.exists());

	auto keyA=keyStoreA->localKey();
	QVERIFY(!keyA.isNull());
	qDebug() << keyA->toString();
	QVERIFY(keyA->isValid(true));
	QVERIFY(keyA->hasPrivate(true));
	QVERIFY(keyA->hasPublic(true));

	QString idA=keyA->id();
	qDebug()<<"Keystore A :"<<idA<<QFileInfo(keyStoreA->filename()).absoluteFilePath();
	NetworkAddress addrA(local, basePort + 0);
	QString peersFilenameA="peersFileA.json";
	QFile peersFileA(peersFilenameA);
	if(peersFileA.exists()) {
		QVERIFY(peersFileA.remove());
	}
	QVERIFY(!peersFileA.exists());
	QSharedPointer<AddressBook> peersA(OC_NEW AddressBook());
	peersA->configure(peersFilenameA);

	QString nameA="PARTY A";
	QSharedPointer<Associate> partA=generatePart(nameA, keyA, addrA, ROLE_AGENT, TYPE_AGENT);

	test::utility::testHeading("INITIALIZING ID FOR PARTY B");////////////////////////////////////////////////
	QString keyStoreFilenameB="keyFileB.json";
	QFile keyStoreFileB(keyStoreFilenameB);
	if(keyStoreFileB.exists()) {
		QVERIFY(keyStoreFileB.remove());
	}
	QVERIFY(!keyStoreFileB.exists());
	QSharedPointer<KeyStore> keyStoreB(OC_NEW KeyStore());
	keyStoreB->configure(keyStoreFilenameB, true, quickAndInsecurePolicy);


	QVERIFY(!keyStoreFileB.exists());

	keyStoreB->waitForSync();
	QVERIFY(keyStoreFileB.exists());

	auto keyB=keyStoreB->localKey();
	QVERIFY(nullptr!=keyB);
	qDebug() << keyB->toString();
	QVERIFY(keyB->isValid(true));
	QVERIFY(keyB->hasPrivate(true));
	QVERIFY(keyB->hasPublic(true));

	QString idB=keyB->id();
	qDebug()<<"Keystore B :"<<idB<<QFileInfo(keyStoreB->filename()).absoluteFilePath();
	NetworkAddress addrB(local, basePort + 1);
	QString peersFilenameB="peersFileB.json";
	QFile peersFileB(peersFilenameB);
	if(peersFileB.exists()) {
		QVERIFY(peersFileB.remove());
	}
	QVERIFY(!peersFileB.exists());

	QSharedPointer<AddressBook> peersB(OC_NEW AddressBook());
	peersB->configure(peersFilenameB);

	QString nameB="PARTY B";
	QVariantMap addrBMap=addrB.toVariantMap();
	QCOMPARE(addrBMap.size(), 2);
	QSharedPointer<Associate> partB=generatePart(nameB, keyB, addrB, ROLE_CONTROL, TYPE_REMOTE);


	test::utility::testHeading("BIND PARTY A to B");////////////////////////////////////////////////
	partA->trusts().addTrust(idB);
	peersA->upsertAssociate(partB);
	qDebug()<<"IDB="<<idB;
	keyStoreA->setPubKeyForID(keyB->pubKey());


	test::utility::testHeading("BIND PARTY B to A");/////////////////////////////////////////////////
	partB->trusts().addTrust(idA);
	peersB->upsertAssociate(partA);
	keyStoreB->setPubKeyForID(keyA->pubKey());


	test::utility::testHeading("INITIALIZING COMMS FOR PARTY A");///////////////////////////////////
	QSharedPointer<CommsCarrierUDP> carrierA(OC_NEW CommsCarrierUDP());
	carrierA->configure();
	QSharedPointer<CommsChannel> chanA(OC_NEW CommsChannel());
	chanA->configure(carrierA,keyStoreA, peersA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA->hookCommsSignals(sigLogA, true);
	QSharedPointer<MockCourier> courA1(OC_NEW MockCourier("Courier A1",idB, "This is datagram A1 123", chanA, maxSends, maxRecs));
	chanA->registerCourier(courA1, true);

	//TestCourier courA2("Courier A2", commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA->getSummary();

	test::utility::testHeading ("INITIALIZING COMMS FOR PARTY B");//////////////////////////////////
	QSharedPointer<CommsCarrierUDP> carrierB(OC_NEW CommsCarrierUDP());
	carrierB->configure();
	QSharedPointer<CommsChannel> chanB(OC_NEW CommsChannel());
	chanB->configure(carrierB, keyStoreB, peersB);
	CommsSignalLogger sigLogB("LOG-B");
	chanA->hookCommsSignals(sigLogB, true);
	QSharedPointer<MockCourier> courB1(OC_NEW MockCourier("Courier B1", idA, "This is datagram B1 æøåä", chanB, maxSends, maxRecs));
	chanB->registerCourier(courB1, true);
	//TestCourier courB2("Courier B2", commSigA, "This is datagram B2 Q", maxSends, maxRecs); chanB.setCourierRegistered(courB2, true);
	qDebug()<<"SUMMARY: "<<chanB->getSummary();

	test::utility::testHeading("STARTING 1st time with no sessions","#");//////////////////////////////////
	chanA->rescheduleSending(now);
	chanA->carrier()->setListenAddress(addrA);
	chanA->carrier()->maintainConnection(true);
	chanB->rescheduleSending(now+1000);
	chanB->carrier()->setListenAddress(addrB);
	chanB->carrier()->maintainConnection(true);

	test::utility::testHeading("WAITING 1st time with no sessions");///////////////////////////////
	{
		quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
		const quint64 end=now+15000;
		while(now<end) {
			//qDebug()<<" * * * Tick Tock.....................";
			now=utility::time::currentMsecsSinceEpoch<quint64>();
			QCoreApplication::processEvents();
		}
	}

	test::utility::testHeading("SUMMARIES 1st time with no sessions");//////////////////////////////
	courA1->writeSummary();
	courB1->writeSummary();

	test::utility::testHeading("STOP 1st time with no sessions");///////////////////////////////////
	chanA->carrier()->maintainConnection(false);
	chanB->carrier()->maintainConnection(false);

	CommsSessionDirectory & sessDirA=chanA->sessions();
	CommsSessionDirectory & sessDirB=chanB->sessions();

	QSharedPointer<CommsSession> sessA=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreA->localKey()));
	QSharedPointer<CommsSession> sessB=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreB->localKey()));

	QVERIFY(nullptr != sessA);
	QVERIFY(nullptr != sessB);

	sessA->setAddress(addrA);
	sessB->setAddress(addrB);

	sessA->setLocalSessionID(sessDirA.generateUnusedSessionID());
	sessB->setLocalSessionID(sessDirB.generateUnusedSessionID());

	QVERIFY(sessA->address().isValid());
	QVERIFY(sessB->address().isValid());

	sessDirA.insert(sessB);
	sessDirB.insert(sessA);

	test::utility::testHeading("SESSION SUMMARIES");/////////////////////////////////////////////////////
	qDebug()<<"SUM A: "<<sessDirA.summary();
	qDebug()<<"SUM B: "<<sessDirB.summary();


	test::utility::testHeading("STARTING 2nd time with sessions","#");///////////////////////////////////////////
	chanA->rescheduleSending(now);
	chanA->carrier()->setListenAddress(addrA);
	chanA->carrier()->maintainConnection(true);
	QVERIFY(chanA->carrier()->isConnectionMaintained());

	chanB->rescheduleSending(now);
	chanB->carrier()->setListenAddress(addrB);
	chanB->carrier()->maintainConnection(true);
	QVERIFY(chanB->carrier()->isConnectionMaintained());

	qDebug()<<"";
	qDebug()<<"####################################### WAITING 2nd time with sessions";
	{
		const quint64 start=utility::time::currentMsecsSinceEpoch<quint64>();
		quint64 now=start;
		while(now<start+10000) {
			now=utility::time::currentMsecsSinceEpoch<quint64>();
			QCoreApplication::processEvents();
		}
	}

	test::utility::testHeading("SUMMARIES 2nd time with sessions");////////////////////////////////////////////
	courA1->writeSummary();
	courB1->writeSummary();


	test::utility::testHeading("DELETING");///////////////////////////////////////////////////////////

}
