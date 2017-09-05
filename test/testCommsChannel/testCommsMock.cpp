#include "TestCommsChannel.hpp"

#include "MockCommsCarrier.hpp"

#include "comms/PacketSendState.hpp"

#include "TestCourier.hpp"



/*
Test CommsChannel with a mock carrier

Allowes fine-grained control over when each event occurs which in turn allows for fine grained validity checks

*/



void TestCommsChannel::testCommsMock()
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

	heading("PARAMETER SUMMARY");
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + keyBits:   "<<keyBits;
	qDebug()<<"  + hashAlgo:  "<<hashAlgo;
	qDebug()<<"  + now:       "<<now;

	heading("INITIALIZING ID FOR PARTY A");/////////////////////////////////////////////////////
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()) {
		keyStoreFileA.remove();
	}
	KeyStore keyStoreA(keyStoreFilenameA, policy);
	keyStoreA.bootstrap(false,false);
	auto keyA=keyStoreA.localKey();
	//qDebug() << keyA->toString();
	QString idA=keyA->id();
	qDebug()<<"Keystore A :"<<idA<<QFileInfo(keyStoreA.filename()).absoluteFilePath();
	NetworkAddress addrA(local, basePort + 0);
	QString peersFilenameA="peersFileA.json";
	QFile peersFileA(peersFilenameA);
	if(peersFileA.exists()) {
		peersFileA.remove();
	}
	NodeAssociateStore peersA(peersFilenameA);
	peersA.bootstrap(false,false);
	QVariantMap peerMapA;
	QString nameA="PARTY A";
	QSharedPointer<NodeAssociate> partA=generatePart(nameA, keyA, addrA, ROLE_AGENT, TYPE_AGENT);

	heading("INITIALIZING ID FOR PARTY B");/////////////////////////////////////////////////////
	QString keyStoreFilenameB="keyFileB.json";
	QFile keyStoreFileB(keyStoreFilenameB);
	if(keyStoreFileB.exists()) {
		QVERIFY(keyStoreFileB.remove());
	}
	KeyStore keyStoreB(keyStoreFilenameB, policy);
	keyStoreB.bootstrap(false,false);
	auto keyB=keyStoreB.localKey();
	//qDebug() << keyB->toString();

	QString idB=keyB->id();
	qDebug()<<"Keystore B :"<<idB<<QFileInfo(keyStoreB.filename()).absoluteFilePath();
	NetworkAddress addrB(local, basePort + 1);
	QString peersFilenameB="peersFileB.json";
	QFile peersFileB(peersFilenameB);
	if(peersFileB.exists()) {
		peersFileB.remove();
	}

	NodeAssociateStore peersB(peersFilenameB);
	peersB.bootstrap(false,false);
	QVariantMap peerMapB;
	QString nameB="PARTY B";
	QVariantMap addrBMap=addrB.toVariantMap();
	QSharedPointer<NodeAssociate> partB=generatePart(nameB, keyB, addrB, ROLE_CONTROL, TYPE_REMOTE);

	heading("BIND PARTY A to B");/////////////////////////////////////////////////////
	partA->addTrust(idB);
	peersA.setParticipant(partB);
	qDebug()<<"IDB="<<idB;
	keyStoreA.setPubKeyForID(keyB->pubKey());

	heading("INITIALIZING COMMS FOR PARTY A");/////////////////////////////////////////////////////
	MockCommsCarrier carrierA;
	CommsChannel chanA(carrierA,keyStoreA, peersA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA.setHookCommsSignals(sigLogA, true);
	TestCourier courA1("Courier A1",idB, "This is datagram A1 123", chanA, maxSends, maxRecs);
	chanA.setCourierRegistered(courA1, true);
	//TestCourier courA2("Courier A2", commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA.getSummary();

	heading("Preparing lots of test data");/////////////////////////////////////////////////////
	quint64 time=QDateTime::currentMSecsSinceEpoch();
	const quint64 stepMS=1000;
	NetworkAddress na(QHostAddress("127.0.0.1"), 8123);
	carrierA.mockSetOverrideSendingtimer(true);
	chanA.start(na);
	CommsSessionDirectory & sessDirA=chanA.sessions();
	QSharedPointer<CommsSession> sessA=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreA.localKey()));
	auto sessAID=sessDirA.generateUnusedSessionID();
	sessA->setLocalSessionID(sessAID);
	sessA->setAddress(na);
	sessDirA.insert(sessA);
	NetworkAddress unknownAddress;
	unknownAddress.fromString("1.2.3.4:5434");
	QByteArray garbledPacket=QString("This is garbled").toUtf8();
	const SESSION_ID_TYPE localSessionID=1337;
	const SESSION_NONCE_TYPE synNonce=1111;
	QByteArray idlePacket;
	{
		PacketSendState state;
		state.writeMultimagic(MULTIMAGIC_IDLE);
		idlePacket=state.datagram;
	}

	QByteArray handshakeSynPacket;
	{
		PacketSendState state;
		state.setSession(sessA);
		state.writeMultimagic(MULTIMAGIC_SYN);

		// FULL-ID
		state.writeEncSenderID(idB);
		// DESIRED SESSION-ID
		state.writeEncSessionID(localSessionID);
		// NONCE
		state.writeEncNonce(synNonce);
		state.encrypt();
		state.writeProtocolEncryptedMessage();

		handshakeSynPacket=state.datagram;
		qDebug()<<"SYN packet size is: "<<handshakeSynPacket.size();
	}

	QByteArray handshakeSynAclPacket;
	{
		PacketSendState state;
		state.writeMultimagic(MULTIMAGIC_SYNACK);
		handshakeSynAclPacket=state.datagram;
	}

	QByteArray handshakeAckPacket;
	{
		PacketSendState state;
		state.writeMultimagic(MULTIMAGIC_ACK);
		handshakeAckPacket=state.datagram;
	}

	heading("RUNNING THE ACTUAL TESTS", "#");/////////////////////////////////////////////////////


	heading("Send garbled packet from unknown address");/////////////////////////////////////////////////////
	carrierA.mockWriteMock(garbledPacket, unknownAddress);


	heading("Send idle packet from unknown address");/////////////////////////////////////////////////////
	carrierA.mockWriteMock(idlePacket, unknownAddress);


	heading("Send garbled packet from known address");/////////////////////////////////////////////////////

	carrierA.mockWriteMock(garbledPacket, addrB);


	heading("Send idle packet from known address");/////////////////////////////////////////////////////

	carrierA.mockWriteMock(idlePacket, addrB);

	heading("Send handshake-syn packet from known address");/////////////////////////////////////////////////////
	carrierA.mockWriteMock(handshakeSynPacket, addrB);


	for(int i=0;i<4;++i){
		heading("Looking for return ACK "+QString::number(i));/////////////////////////////////////////////////////
		carrierA.mockTriggerSendingOpportunity(time+=stepMS);
	}


	heading("SUMMARIES 1st time with no sessions");/////////////////////////////////////////////////////
	courA1.writeSummary();

	heading("STOP 1st time with no sessions");/////////////////////////////////////////////////////
	chanA.stop();

	heading("DELETING");
	QTest::waitForEvents();

}




