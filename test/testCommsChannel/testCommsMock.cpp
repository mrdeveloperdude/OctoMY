#include "TestCommsChannel.hpp"

#include "comms/Comms.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/PacketReadState.hpp"
#include "comms/PacketSendState.hpp"
#include "comms/address/CarrierAddressUDP.hpp"
#include "discovery/AddressBook.hpp"
#include "security/KeySecurityPolicy.hpp"
#include "security/KeyStore.hpp"
#include "test/CommsSignalLogger.hpp"
#include "test/Utility.hpp"
#include "test/mock/MockCommsCarrier.hpp"
#include "test/mock/MockCourier.hpp"
#include "uptime/New.hpp"
#include "utility/time/HumanTime.hpp"


static QByteArray handshakeSynPacket(QSharedPointer<CommsSession> sessA, QString idB, SESSION_ID_TYPE localSessionID, SESSION_NONCE_TYPE synNonce)
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

	QByteArray handshakeSynPacket=state.datagram;
	qDebug()<<"SYN packet size is: "<<handshakeSynPacket.size();
	return handshakeSynPacket;
}


static QByteArray handshakeSynAckPacket(QSharedPointer<CommsSession> sessA, QString idB, SESSION_ID_TYPE localSessionID, SESSION_NONCE_TYPE synAckNonce, SESSION_NONCE_TYPE ackNonce)
{
	PacketSendState state;
	state.setSession(sessA);
	state.writeMultimagic(MULTIMAGIC_SYNACK);

	// FULL-ID
	state.writeEncSenderID(idB);
	// DESIRED SESSION-ID
	state.writeEncSessionID(localSessionID);
	// RETURN NONCE
	state.writeEncNonce(synAckNonce);
	// NONCE
	state.writeEncNonce(ackNonce);
	state.encrypt();
	state.writeProtocolEncryptedMessage();
	QByteArray handshakeSynAckPacket=state.datagram;
	qDebug()<<"SYN-ACK packet size is: "<<handshakeSynAckPacket.size();
	return handshakeSynAckPacket;
}


static QByteArray handshakeAckPacket(QSharedPointer<CommsSession> sessA, QString idB, SESSION_ID_TYPE localSessionID, SESSION_NONCE_TYPE ackNonce)
{
	Q_UNUSED(localSessionID);
	PacketSendState state;
	state.setSession(sessA);
	state.writeMultimagic(MULTIMAGIC_ACK);

	// FULL-ID
	state.writeEncSenderID(idB);
	// RETURN NONCE
	state.writeEncNonce(ackNonce);

	state.encrypt();
	state.writeProtocolEncryptedMessage();
	QByteArray handshakeAckPacket=state.datagram;

	qDebug()<<"SYN-ACK packet size is: "<<handshakeAckPacket.size();
	return handshakeAckPacket;
}

/*
Test CommsChannel with a mock getCarrier

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
	const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();

	test::utility::testHeading("PARAMETER SUMMARY");
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + keyBits:   "<<keyBits;
	qDebug()<<"  + hashAlgo:  "<<hashAlgo;
	qDebug()<<"  + now:       "<<now;

	test::utility::testHeading("INITIALIZING ID FOR PARTY A");/////////////////////////////////////////////////////
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()) {
		keyStoreFileA.remove();
	}
	QSharedPointer<KeyStore> keyStoreA(OC_NEW KeyStore());
	keyStoreA->configure(keyStoreFilenameA, true, policy);

	auto keyA=keyStoreA->localKey();
	//qDebug() << keyA->toString();
	QString idA=keyA->id();
	qDebug()<<"Keystore A :"<<idA<<QFileInfo(keyStoreA->filename()).absoluteFilePath();
	auto addrA = QSharedPointer<CarrierAddressUDP>::create(local, basePort + 0);
	QString peersFilenameA="peersFileA.json";
	QFile peersFileA(peersFilenameA);
	if(peersFileA.exists()) {
		peersFileA.remove();
	}
	QSharedPointer<AddressBook> peersA(OC_NEW AddressBook());
	peersA->configure(peersFilenameA);

	QVariantMap peerMapA;
	QString nameA="PARTY A";
	QSharedPointer<Associate> partA=generatePart(nameA, keyA, addrA, ROLE_AGENT, TYPE_AGENT);

	test::utility::testHeading("INITIALIZING ID FOR PARTY B");/////////////////////////////////////////////////////
	QString keyStoreFilenameB="keyFileB.json";
	QFile keyStoreFileB(keyStoreFilenameB);
	if(keyStoreFileB.exists()) {
		QVERIFY(keyStoreFileB.remove());
	}
	KeyStore keyStoreB;
	keyStoreB.configure(keyStoreFilenameB, true, policy);

	auto keyB=keyStoreB.localKey();
	//qDebug() << keyB->toString();

	QString idB=keyB->id();
	qDebug()<<"Keystore B :"<<idB<<QFileInfo(keyStoreB.filename()).absoluteFilePath();
	auto addrB = QSharedPointer<CarrierAddressUDP>::create(local, basePort + 1);
	QString peersFilenameB="peersFileB.json";
	QFile peersFileB(peersFilenameB);
	if(peersFileB.exists()) {
		peersFileB.remove();
	}

	AddressBook peersB;
	peersB.configure(peersFilenameB);

	QVariantMap peerMapB;
	QString nameB="PARTY B";
	QVariantMap addrBMap=addrB->toVariantMap();
	QSharedPointer<Associate> partB=generatePart(nameB, keyB, addrB, ROLE_CONTROL, TYPE_REMOTE);

	test::utility::testHeading("BIND PARTY A to B");/////////////////////////////////////////////////////
	partA->trusts().addTrust(idB);
	peersA->upsertAssociate(partB);
	qDebug()<<"IDB="<<idB;
	keyStoreA->setPubKeyForID(keyB->pubKey());

	test::utility::testHeading("INITIALIZING COMMS FOR PARTY A");/////////////////////////////////////////////////////
	auto carrierA = QSharedPointer<MockCommsCarrier>::create();
	auto commsA = QSharedPointer<Comms>::create();
	auto chanA = QSharedPointer<CommsChannel>::create();
	carrierA->configure();
	commsA->configure(keyStoreA, peersA);
	chanA->configure(commsA, carrierA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA->hookCommsSignals(sigLogA, true);
	auto courA1 = QSharedPointer<MockCourier>::create("Courier A1",idB, "This is datagram A1 123", commsA, maxSends, maxRecs);
	chanA->registerCourier(courA1, true);
	//TestCourier courA2("Courier A2", commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA->getSummary();

	test::utility::testHeading("Preparing lots of test data");/////////////////////////////////////////////////////
	quint64 time=utility::time::currentMsecsSinceEpoch<quint64>();
	const quint64 stepMS=1000;
	auto na = QSharedPointer<CarrierAddressUDP>::create(QHostAddress("127.0.0.1"), 8123);
	carrierA->mockSetOverrideSendingtimer(true);
	chanA->getCarrier()->setListenAddress(na);
	chanA->getCarrier()->maintainConnection(true);
	CommsSessionDirectory & sessDirA=chanA->sessions();
	QSharedPointer<CommsSession> sessA=QSharedPointer<CommsSession> (OC_NEW CommsSession(keyStoreA->localKey()));
	auto sessAID=sessDirA.generateUnusedSessionID();
	sessA->setLocalSessionID(sessAID);
	sessA->setAddress(na);
	sessDirA.insert(sessA);
	auto unknownAddress = QSharedPointer<CarrierAddressUDP>::create();
	unknownAddress->fromString("1.2.3.4:5434");
	QByteArray garbledPacket=QString("This is garbled").toUtf8();
	const SESSION_ID_TYPE localSessionID=1337;
	const SESSION_NONCE_TYPE synNonce=1111;
	//const SESSION_NONCE_TYPE synAckNonce=2222;
	//const SESSION_NONCE_TYPE ackNonce=3333;
	QByteArray idlePacket;
	{
		PacketSendState state;
		state.writeMultimagic(MULTIMAGIC_IDLE);
		idlePacket=state.datagram;
	}


	test::utility::testHeading("RUNNING THE ACTUAL TESTS", "#");/////////////////////////////////////////////////////


	test::utility::testHeading("Send garbled packet from unknown address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(garbledPacket, unknownAddress);
	carrierA->mockReadMock(addrB);


	test::utility::testHeading("Send idle packet from unknown address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(idlePacket, unknownAddress);
	carrierA->mockReadMock(addrB);


	test::utility::testHeading("Send garbled packet from known address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(garbledPacket, addrB);
	carrierA->mockReadMock(addrB);


	test::utility::testHeading("Send idle packet from known address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(idlePacket, addrB);
	carrierA->mockReadMock(addrB);

	test::utility::testHeading("Send handshake-syn packet from known address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(handshakeSynPacket(sessA, idB, localSessionID, synNonce), addrB);


	SESSION_NONCE_TYPE ackReturnNonce=INVALID_NONCE;
	for(int i=0; i<4; ++i) {
		test::utility::testHeading("Looking for return SYN-ACK "+QString::number(i));/////////////////////////////////////////////////////
		carrierA->mockTriggerSendingOpportunity(time+=stepMS);
		QByteArray data=carrierA->mockReadMock(addrB);
		PacketReadState rstate(data, addrB->ip(), addrB->port());
		// MULTIMAGIC
		rstate.readMultimagic();
		Multimagic mm=static_cast<Multimagic>(rstate.multimagic);
		qDebug()<<" MM= "<<MultimagicToString(mm);
		if(MULTIMAGIC_SYNACK== mm) {

			// REMOTE FULL-ID
			rstate.readEncSenderID();

			// REMOTE DESIRED SESSION-ID
			rstate.readEncDesiredRemoteSessionID();

			// RETURN NONCE
			rstate.readEncReturnNonce();

			ackReturnNonce=rstate.octomyProtocolReturnNonce;
		}
	}

	test::utility::testHeading("Send handshake-ack packet from known address");/////////////////////////////////////////////////////
	carrierA->mockWriteMock(handshakeAckPacket(sessA, idB, localSessionID, ackReturnNonce), addrB);


	Q_UNUSED(handshakeSynAckPacket);
	/*TODO implement the return trip


	for(int i=0;i<4;++i){
		heading("Looking for return ACK "+QString::number(i));/////////////////////////////////////////////////////
		carrierA.mockTriggerSendingOpportunity(time+=stepMS);
	}

	*/
	test::utility::testHeading("SUMMARIES 1st time with no sessions");/////////////////////////////////////////////////////
	courA1->writeSummary();

	test::utility::testHeading("STOP 1st time with no sessions");/////////////////////////////////////////////////////
	chanA->getCarrier()->maintainConnection(false);

	test::utility::testHeading("DELETING");
	test::utility::testWaitForEvents();
}
