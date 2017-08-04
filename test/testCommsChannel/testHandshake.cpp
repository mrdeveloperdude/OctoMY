#include "TestCommsChannel.hpp"

#include "TestCourier.hpp"

#include "comms/PacketSendState.hpp"

class CommsChannelTest: public CommsChannel
{
public:
	explicit CommsChannelTest(KeyStore &keystore, NodeAssociateStore &peers, QObject *parent=nullptr)
		: CommsChannel(keystore, peers, parent)
	{

	}

	explicit CommsChannelTest(KeyStore &keystore, QObject *parent=nullptr)
		: CommsChannel(keystore, parent)
	{

	}



	void testSendSyn(PacketSendState &state)
	{
		sendSyn(state);
	}

	void testSendSynAck(PacketSendState &state)
	{
		sendSynAck(state);
	}


	void testSendAck(PacketSendState &state)
	{
		sendAck(state);
	}


	void testSendHandshake(const quint64 &now, const QString handShakeID)
	{
		sendHandshake(now, handShakeID);
	}



};


///////////////////////////////////////////////////////////////////////////////


void TestCommsChannel::testHandshake()
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

	qDebug()<<"";
	qDebug()<<"####################################### PARAMETER SUMMARY";
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + keyBits:   "<<keyBits;
	qDebug()<<"  + hashAlgo:  "<<hashAlgo;
	qDebug()<<"  + now:       "<<now;

	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING ID FOR PARTY A";
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()) {
		QVERIFY(keyStoreFileA.remove());
	}
	QVERIFY(!keyStoreFileA.exists());

	KeyStore keyStoreA(keyStoreFilenameA, policy);
	keyStoreA.bootstrap(false,false);

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
	NodeAssociateStore peersA(peersFilenameA);
	peersA.bootstrap(false,false);
	QVariantMap peerMapA;
	QString nameA="PARTY A";
	peerMapA["publicAddress"]=addrA.toVariantMap();
	peerMapA["localAddress"]=addrA.toVariantMap();
	peerMapA["lastSeenMS"]=0;
	peerMapA["birthDate"]=0;
	peerMapA["key"]=keyA->toVariantMap(true);
	peerMapA["role"]=DiscoveryRoleToString(ROLE_AGENT);
	peerMapA["type"]=DiscoveryTypeToString(TYPE_AGENT);
	peerMapA["name"]=nameA;
	peerMapA["gender"]="Male";
	peerMapA["trusts"]=QStringList();
	QSharedPointer<NodeAssociate> partA(new NodeAssociate(peerMapA));

	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING ID FOR PARTY B";
	QString keyStoreFilenameB="keyFileB.json";
	QFile keyStoreFileB(keyStoreFilenameB);
	if(keyStoreFileB.exists()) {
		QVERIFY(keyStoreFileB.remove());
	}
	QVERIFY(!keyStoreFileB.exists());
	KeyStore keyStoreB(keyStoreFilenameB, policy);
	keyStoreB.bootstrap(false,false);
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

	NodeAssociateStore peersB(peersFilenameB);
	peersB.bootstrap(false,false);
	QVariantMap peerMapB;
	QString nameB="PARTY B";
	QVariantMap addrBMap=addrB.toVariantMap();
	QCOMPARE(addrBMap.size(), 2);
	peerMapB["publicAddress"]=addrBMap;
	peerMapB["localAddress"]=addrBMap;
	peerMapB["lastSeenMS"]=0;
	peerMapB["birthDate"]=0;
	peerMapB["key"]=keyB->toVariantMap(true);
	peerMapB["role"]=DiscoveryRoleToString(ROLE_CONTROL);
	peerMapB["type"]=DiscoveryTypeToString(TYPE_REMOTE);
	peerMapB["name"]=nameB;
	peerMapB["gender"]="Female";
	peerMapB["trusts"]=QStringList();
	QSharedPointer<NodeAssociate> partB(new NodeAssociate(peerMapB));


	qDebug()<<"";
	qDebug()<<"####################################### BIND PARTY A to B";
	partA->addTrust(idB);
	peersA.setParticipant(partB);
	qDebug()<<"IDB="<<idB;
	keyStoreA.setPubKeyForID(keyB->pubKey());


	qDebug()<<"";
	qDebug()<<"####################################### BIND PARTY B to A";
	partB->addTrust(idA);
	peersB.setParticipant(partA);
	keyStoreB.setPubKeyForID(keyA->pubKey());


	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING COMMS FOR PARTY A";
	CommsChannelTest chanA(keyStoreA, peersA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA.setHookCommsSignals(sigLogA, true);
	TestCourier courA1("Courier A1",idB, "This is datagram A1 123", chanA, maxSends, maxRecs);
	chanA.setCourierRegistered(courA1, true);
	//TestCourier courA2("Courier A2", commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA.getSummary();

	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING COMMS FOR PARTY B";
	CommsChannelTest chanB(keyStoreB, peersB);
	CommsSignalLogger sigLogB("LOG-B");
	chanA.setHookCommsSignals(sigLogB, true);
	TestCourier courB1("Courier B1", idA, "This is datagram B1 æøåä", chanB, maxSends, maxRecs);
	chanB.setCourierRegistered(courB1, true);
	//TestCourier courB2("Courier B2", commSigA, "This is datagram B2 Q", maxSends, maxRecs); chanB.setCourierRegistered(courB2, true);
	qDebug()<<"SUMMARY: "<<chanB.getSummary();

	qDebug()<<"";
	qDebug()<<"#######################################";
	qDebug()<<"####################################### STARTING 1st time with no sessions";

	quint64 time=now;
	chanA.rescheduleSending(time++);
	chanB.rescheduleSending(time++);

	chanA.testSendHandshake(time, idB);
	time++;

//	chanB.rece


//	PacketSendState state;
	//state.setRemoteID(handShakeID);
	//state.session=session;

	//chanA.testSendSyn(state);


	qDebug()<<"";
	qDebug()<<"####################################### WAITING 1st time with no sessions";
	{
		quint64 now=QDateTime::currentMSecsSinceEpoch();
		const quint64 end=now+15000;
		while(now<end) {
			//qDebug()<<" * * * Tick Tock.....................";
			now=QDateTime::currentMSecsSinceEpoch();
			QCoreApplication::processEvents();
		}
	}



	qDebug()<<"";
	qDebug()<<"####################################### SUMMARIES 1st time with no sessions";
	courA1.writeSummary();
	courB1.writeSummary();


	qDebug()<<"";
	qDebug()<<"####################################### DELETING";
}
