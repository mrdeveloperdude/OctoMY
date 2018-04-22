 #include "TestCommsChannel.hpp"

#include "../common/mock/MockCourier.hpp"

#include "../common/Utilities.hpp"

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
	const KeySecurityPolicy policy(keyBits, hashAlgo);
	const quint64 now=QDateTime::currentMSecsSinceEpoch();


	testHeading("PARAMETER SUMMARY");
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + keyBits:   "<<keyBits;
	qDebug()<<"  + hashAlgo:  "<<hashAlgo;
	qDebug()<<"  + now:       "<<now;

	testHeading("INITIALIZING ID FOR PARTY A");/////////////////////////////////////////////
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()) {
		QVERIFY(keyStoreFileA.remove());
	}
	QVERIFY(!keyStoreFileA.exists());

	KeyStore keyStoreA(keyStoreFilenameA, true, policy);

	auto keyA=keyStoreA.localKey();
	QVERIFY(nullptr!=keyA);
	qDebug() << keyA->toString();
	QVERIFY(keyA->isValid(true));
	QVERIFY(keyA->hasPrivate(true));
	QVERIFY(keyA->hasPublic(true));

	QString idA=keyA->id();
	qDebug()<<"Keystore A :"<<idA<<QFileInfo(keyStoreA.filename()).absoluteFilePath();
	NetworkAddress addrA(local, basePort + 0);
	QString peersFilenameA="peersFileA.json";
	QFile peersFileA(peersFilenameA);
	if(peersFileA.exists()) {
		QVERIFY(peersFileA.remove());
	}
	QVERIFY(!peersFileA.exists());
	AddressBook peersA(peersFilenameA);

	QString nameA="PARTY A";
	QSharedPointer<Associate> partA=generatePart(nameA, keyA, addrA, ROLE_AGENT, TYPE_AGENT);

	testHeading("INITIALIZING ID FOR PARTY B");////////////////////////////////////////////////
	QString keyStoreFilenameB="keyFileB.json";
	QFile keyStoreFileB(keyStoreFilenameB);
	if(keyStoreFileB.exists()) {
		QVERIFY(keyStoreFileB.remove());
	}
	QVERIFY(!keyStoreFileB.exists());
	KeyStore keyStoreB(keyStoreFilenameB, true, policy);

	auto keyB=keyStoreB.localKey();
	QVERIFY(nullptr!=keyB);
	qDebug() << keyB->toString();
	QVERIFY(keyB->isValid(true));
	QVERIFY(keyB->hasPrivate(true));
	QVERIFY(keyB->hasPublic(true));

	QString idB=keyB->id();
	qDebug()<<"Keystore B :"<<idB<<QFileInfo(keyStoreB.filename()).absoluteFilePath();
	NetworkAddress addrB(local, basePort + 1);
	QString peersFilenameB="peersFileB.json";
	QFile peersFileB(peersFilenameB);
	if(peersFileB.exists()) {
		QVERIFY(peersFileB.remove());
	}
	QVERIFY(!peersFileB.exists());

	AddressBook peersB(peersFilenameB);

	QString nameB="PARTY B";
	QVariantMap addrBMap=addrB.toVariantMap();
	QCOMPARE(addrBMap.size(), 2);
	QSharedPointer<Associate> partB=generatePart(nameB, keyB, addrB, ROLE_CONTROL, TYPE_REMOTE);


	testHeading("BIND PARTY A to B");////////////////////////////////////////////////
	partA->addTrust(idB);
	peersA.upsertAssociate(partB);
	qDebug()<<"IDB="<<idB;
	keyStoreA.setPubKeyForID(keyB->pubKey());


	testHeading("BIND PARTY B to A");/////////////////////////////////////////////////
	partB->addTrust(idA);
	peersB.upsertAssociate(partA);
	keyStoreB.setPubKeyForID(keyA->pubKey());


	testHeading("INITIALIZING COMMS FOR PARTY A");///////////////////////////////////
	CommsCarrierUDP carrierA;
	CommsChannel chanA(carrierA,keyStoreA, peersA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA.setHookCommsSignals(sigLogA, true);
	QSharedPointer<MockCourier> courA1(OC_NEW MockCourier("Courier A1",idB, "This is datagram A1 123", chanA, maxSends, maxRecs));
	chanA.setCourierRegistered(courA1, true);

	//TestCourier courA2("Courier A2", commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA.getSummary();

	testHeading("INITIALIZING COMMS FOR PARTY B");//////////////////////////////////
	CommsCarrierUDP carrierB;
	CommsChannel chanB(carrierB, keyStoreB, peersB);
	CommsSignalLogger sigLogB("LOG-B");
	chanA.setHookCommsSignals(sigLogB, true);
	QSharedPointer<MockCourier> courB1(OC_NEW MockCourier("Courier B1", idA, "This is datagram B1 æøåä", chanB, maxSends, maxRecs));
	chanB.setCourierRegistered(courB1, true);
	//TestCourier courB2("Courier B2", commSigA, "This is datagram B2 Q", maxSends, maxRecs); chanB.setCourierRegistered(courB2, true);
	qDebug()<<"SUMMARY: "<<chanB.getSummary();

	testHeading("STARTING 1st time with no sessions","#");//////////////////////////////////
	chanA.rescheduleSending(now);
	chanA.carrier().setListenAddress(addrA);
	chanA.carrier().setStarted(true);
	chanB.rescheduleSending(now+1000);
	chanB.carrier().setListenAddress(addrB);
	chanB.carrier().setStarted(true);

	testHeading("WAITING 1st time with no sessions");///////////////////////////////
	{
		quint64 now=QDateTime::currentMSecsSinceEpoch();
		const quint64 end=now+15000;
		while(now<end) {
			//qDebug()<<" * * * Tick Tock.....................";
			now=QDateTime::currentMSecsSinceEpoch();
			QCoreApplication::processEvents();
		}
	}

	testHeading("SUMMARIES 1st time with no sessions");//////////////////////////////
	courA1->writeSummary();
	courB1->writeSummary();

	testHeading("STOP 1st time with no sessions");///////////////////////////////////
	chanA.carrier().setStarted(false);
	chanB.carrier().setStarted(false);


	/*
	CommsSessionDirectory & sessDirA=chanA.sessions();
	CommsSessionDirectory & sessDirB=chanB.sessions();

	QSharedPointer<CommsSession> sessA=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreA.localKey()));
	QSharedPointer<CommsSession> sessB=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreB.localKey()));

	sessA->setLocalSessionID(sessDirA.generateUnusedSessionID());
	sessB->setLocalSessionID(sessDirB.generateUnusedSessionID());

	sessDirA.insert(sessB);
	sessDirB.insert(sessA);

	heading("SESSION SUMMARIES");/////////////////////////////////////////////////////
	qDebug()<<"SUM A: "<<sessDirA.summary();
	qDebug()<<"SUM B: "<<sessDirB.summary();


	heading("STARTING 2nd time with sessions","#");///////////////////////////////////////////
	chanA.rescheduleSending(now);
	chanA.start(addrA);
	chanB.rescheduleSending(now);
	chanB.start(addrB);
	qDebug()<<"";
	qDebug()<<"####################################### WAITING 2nd time with sessions";
	{
		const quint64 start=QDateTime::currentMSecsSinceEpoch();
		quint64 now=start;
		while(now<start+10000) {
			now=QDateTime::currentMSecsSinceEpoch();
			QCoreApplication::processEvents();
		}
	}

	heading("SUMMARIES 2nd time with sessions");////////////////////////////////////////////
	courA1.writeSummary();
	courB1.writeSummary();

	*/
	testHeading("DELETING");///////////////////////////////////////////////////////////

}


