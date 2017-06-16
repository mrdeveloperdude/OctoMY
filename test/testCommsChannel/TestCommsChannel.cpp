#include "TestCommsChannel.hpp"

#include "utility/Standard.hpp"
#include "comms/messages/MessageType.hpp"

#include "RNG.hpp"

#include "security/KeyStore.hpp"
#include "basic/NodeAssociate.hpp"
#include "discovery/NodeAssociateStore.hpp"

#include <QSignalSpy>
#include <QFileInfo>

TestCourier::TestCourier(QString  dest, QByteArray datagram, CommsChannel &comms, const qint32 maxSends , const qint32 maxRecs, CommsTester *parent)
	: Courier("Test", Courier::FIRST_USER_ID+313, comms, parent)
	, mCt(parent)
	, mSoFar(0)
	, mDatagram(datagram)
	, mMaxSends(maxSends)
	, mMaxRecs(maxRecs)
	, mSendCount(0)
	, mRecCount(0)

	, mMandate(mDatagram.size()+4 // payloadSize
			   ,10 // priority
			   ,0 // nextSend. We set nextSend to 0. Basically this meaens "always overdue, send at every opportunity"
			   ,true // receiveActive
			   ,true // sendActive
			  )

{
	setDestination(dest);
}

TestCourier::~TestCourier()
{

}



// Let the CommChannel know what we want
CourierMandate TestCourier::mandate() const
{
	return mMandate;
}

// Override to act on sending opportunity.
// Return number of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 TestCourier::sendingOpportunity(QDataStream &ds )
{
	mSendCount++;
	const qint16 sendsLeft=mMaxSends-mSendCount;
	const bool done=(sendsLeft<=0);
	if(done) {
		qDebug()<<"MAX SENDS ("<<mMaxSends<<") RECEIVED FOR "<<destination()<<" STOPPING";
		mMandate.sendActive=false;
	}
	ds << mDatagram;
	qDebug()<<"SENDING OPPORTUNITY @ "<<destination()<< " bytes:" <<mMandate.payloadSize<<", "<<(done?"DONE":QString("SENDS LEFT: %1").arg(sendsLeft));
	if(nullptr!=mCt && done) {
		qDebug()<<"EMIT FINISHED --==##==--";
		emit mCt->finished();
	}
	return mMandate.payloadSize;
}

// Override to act on data received
// Return number of bytes actually read.
quint16 TestCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	mRecCount++;
	const qint16 recLeft=mMaxRecs-mRecCount;
	const bool done=(recLeft<=0);
	if(done) {
		qDebug()<<"MAX RECS ("<<mMaxRecs<<") RECEIVED FOR "<<destination()<<" STOPPING";
		mMandate.receiveActive=false;
	}

	QByteArray in;
	ds>>in;
	qDebug()<<"DATA BYTES RECEIVED="<<availableBytes<<" ("<<in<<")";
	return in.size()+4;
}

void TestCourier::writeSummary()
{

	qDebug()<<"";
	qDebug()<<" Summary for    "<<name();
	qDebug()<<"  + SoFar:      "<<mSoFar;
	qDebug()<<"  + mSendCount: "<<mSendCount<<"/"<<mMaxSends;
	qDebug()<<"  + mRecCount:  "<<mRecCount<<"/"<<mMaxRecs;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



CommsTester::CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, KeyStore &keyStore, QObject *parent)
	: QObject(parent)
	, mName(name)
	, mMyAddress(myAddress)
	, mMyPort(myPort)
	, mBasePort(basePort)
	, mPortRange(portRange)
	, mKeyStore(keyStore)
	, mCc(mKeyStore)
	, mTestCount(testCount)
	, mRng(RNG::sourceFactory("mt"))
{
	QVERIFY(nullptr!=mRng);
	mRng->init(mMyPort);
	QVERIFY(mMyPort>=mBasePort);
	QVERIFY(mMyPort<=mBasePort+mPortRange);
	QVERIFY(connect(&mCc,SIGNAL(commsError(QString)),this,SLOT(onError(QString)),OC_CONTYPE));
	for(quint16 toPort=mBasePort; toPort<mBasePort+mPortRange; ++toPort) {
		if(mMyPort==toPort) {
			continue;
		}
		if(mRng->generateReal2()>0.7 || true) {
			qDebug() << mMyAddress << ":" << mMyPort << " --> " << toPort;
			QString myID="1234";
			//CommsSignature sig(myID, NetworkAddress(mMyAddress, toPort));
			TestCourier *tc=new TestCourier(myID, "This is my humble payload", mCc, mTestCount, mTestCount, this);
			QVERIFY(nullptr!=tc);
			mCc.setCourierRegistered(*tc, true);
		} else {
			qDebug() << mMyAddress << ":" << mMyPort << " --> SKIPPED";
		}
	}
}


void CommsTester::onError(QString e)
{
	qWarning()<<toString()<<"::onError(): "<<e;
}



void CommsTester::onReadyRead()
{
	qDebug()<<toString()<<"::onReadyRead()";
}


void CommsTester::startSendTest()
{
	qDebug()<<"Starting test for "<<toString();
	mCc.start(NetworkAddress(mMyAddress, mMyPort));
}


QString CommsTester::toString()
{
	QString ret=mName;
	ret+="(";
	ret+=mMyAddress.toString();
	ret+=":";
	ret+=QString::number(mMyPort);
	ret+=")";
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CommsSignalLogger::CommsSignalLogger(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{

}
CommsSignalLogger::~CommsSignalLogger()
{

}


void CommsSignalLogger::onCommsError(QString message)
{
	qDebug()<<mName<<": onCommsError("<<message<<")";
}

void CommsSignalLogger::onCommsClientAdded(CommsSession *c)
{
	qDebug()<<mName<<": onCommsClientAdded("<<c<<")";
}

void CommsSignalLogger::onCommsConnectionStatusChanged(bool c)
{
	qDebug()<<mName<<": onCommsConnectionStatusChanged("<<c<<")";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
	const quint64 now=QDateTime::currentMSecsSinceEpoch();

	qDebug()<<"";
	qDebug()<<"####################################### PARAMETER SUMMARY";
	qDebug()<<"  + localhost: "<<local;
	qDebug()<<"  + basePort:  "<<basePort;
	qDebug()<<"  + maxSends:  "<<maxSends;
	qDebug()<<"  + maxRecs:   "<<maxRecs;
	qDebug()<<"  + now:       "<<now;

	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING ID FOR PARTY A";
	QString keyStoreFilenameA="keyFileA.json";
	QFile keyStoreFileA(keyStoreFilenameA);
	if(keyStoreFileA.exists()){
		QVERIFY(keyStoreFileA.remove());
	}
	QVERIFY(!keyStoreFileA.exists());

	KeyStore keyStoreA(keyStoreFilenameA);
	keyStoreA.bootstrap(false,false);
	Key &keyA=keyStoreA.localKey();
	qDebug() << keyA.toString();
	QVERIFY(keyA.isValid(true));
	QVERIFY(keyA.hasPrivate(true));
	QVERIFY(keyA.hasPublic(true));

	QString idA=keyA.id();
	qDebug()<<"Keystore A :"<<idA<<QFileInfo(keyStoreA.filename()).absoluteFilePath();
	NetworkAddress addrA(local, basePort + 0);
	QString peersFilenameA="peersFileA.json";
	QFile peersFileA(peersFilenameA);
	if(peersFileA.exists()){
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
	peerMapA["key"]=keyA.toVariantMap(true);
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
	if(keyStoreFileB.exists()){
		QVERIFY(keyStoreFileB.remove());
	}
	QVERIFY(!keyStoreFileB.exists());
	KeyStore keyStoreB(keyStoreFilenameB);
	keyStoreB.bootstrap(false,false);
	Key &keyB=keyStoreB.localKey();
	qDebug() << keyB.toString();
	QVERIFY(keyB.isValid(true));
	QVERIFY(keyB.hasPrivate(true));
	QVERIFY(keyB.hasPublic(true));

	QString idB=keyB.id();
	qDebug()<<"Keystore B :"<<idB<<QFileInfo(keyStoreB.filename()).absoluteFilePath();
	NetworkAddress addrB(local, basePort + 1);
	QString peersFilenameB="peersFileB.json";
	QFile peersFileB(peersFilenameB);
	if(peersFileB.exists()){
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
	peerMapB["key"]=keyB.toVariantMap(true);
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
	keyStoreA.setPubKeyForID(keyB.pubKey());


	qDebug()<<"";
	qDebug()<<"####################################### BIND PARTY B to A";
	partB->addTrust(idA);
	peersB.setParticipant(partA);
	keyStoreB.setPubKeyForID(keyA.pubKey());


	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING COMMS FOR PARTY A";
	CommsChannel chanA(keyStoreA, peersA);
	CommsSignalLogger sigLogA("LOG-A");
	chanA.setHookCommsSignals(sigLogA, true);
	TestCourier courA1(idB, "This is datagram A1 123", chanA, maxSends, maxRecs);
	//chanA.setCourierRegistered(courA1, true);
	//TestCourier courA2(commSigB, "This is datagram A2 uvw xyz", maxSends, maxRecs); chanA.setCourierRegistered(courA2, true);
	qDebug()<<"SUMMARY: "<<chanA.getSummary();

	qDebug()<<"";
	qDebug()<<"####################################### INITIALIZING COMMS FOR PARTY B";
	CommsChannel chanB(keyStoreB, peersB);
	CommsSignalLogger sigLogB("LOG-B");
	chanA.setHookCommsSignals(sigLogB, true);
	TestCourier courB1(idA, "This is datagram B1 æøåä", chanB, maxSends, maxRecs);
	chanB.setCourierRegistered(courB1, true);
	//TestCourier courB2(commSigA, "This is datagram B2 Q", maxSends, maxRecs); chanB.setCourierRegistered(courB2, true);
	qDebug()<<"SUMMARY: "<<chanB.getSummary();

	qDebug()<<"";
	qDebug()<<"#######################################";
	qDebug()<<"####################################### STARTING 1st time with no sessions";
	chanA.rescheduleSending(now);
	chanA.start(addrA);
	chanB.rescheduleSending(now);
	chanB.start(addrB);
	qDebug()<<"";
	qDebug()<<"####################################### WAITING 1st time with no sessions";
	{
		const quint64 start=QDateTime::currentMSecsSinceEpoch();
		quint64 now=start;
		while(now<start+10000) {
			now=QDateTime::currentMSecsSinceEpoch();
			QCoreApplication::processEvents();
		}
	}

	qDebug()<<"";
	qDebug()<<"####################################### SUMMARIES 1st time with no sessions";
	courA1.writeSummary();
	courB1.writeSummary();

	qDebug()<<"";
	qDebug()<<"####################################### STOP 1st time with no sessions";
	chanA.stop();
	chanB.stop();


	/*
	CommsSessionDirectory & sessDirA=chanA.sessions();
	CommsSessionDirectory & sessDirB=chanB.sessions();

	QSharedPointer<CommsSession> sessA=QSharedPointer<CommsSession> (new CommsSession(keyStoreA.localKey()));
	QSharedPointer<CommsSession> sessB=QSharedPointer<CommsSession> (new CommsSession(keyStoreB.localKey()));

	sessA->setLocalSessionID(sessDirA.generateUnusedSessionID());
	sessB->setLocalSessionID(sessDirB.generateUnusedSessionID());

	sessDirA.insert(sessB);
	sessDirB.insert(sessA);

	qDebug()<<"";
	qDebug()<<"####################################### SESSION SUMMARIES";
	qDebug()<<"SUM A: "<<sessDirA.summary();
	qDebug()<<"SUM B: "<<sessDirB.summary();


	qDebug()<<"";
	qDebug()<<"#######################################";
	qDebug()<<"#######################################";
	qDebug()<<"#######################################";
	qDebug()<<"####################################### STARTING 2nd time with sessions";
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

	qDebug()<<"";
	qDebug()<<"####################################### SUMMARIES 2nd time with sessions";
	courA1.writeSummary();
	courB1.writeSummary();

*/
	qDebug()<<"";
	qDebug()<<"####################################### DELETING";

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/*
Sets up X commchannels and have them sending data
via courier to a random but deterministically selected subset of X.

Result should stress the UDP system in a way that lays bare any shortcomings
when used in real world scenario.

TODO: add simulated unreliability factors such as dropped/corrupted/delayed packets
*/
// http://doc.qt.io/qt-5/qtest.html
void TestCommsChannel::testMultiple()
{
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QByteArray>("QByteArray");
	const QHostAddress local("127.0.0.1");
	const quint16 basePort=54345;
	const quint16 portRange=2;
	const quint16 testCount=2;
	QList<CommsTester *> senders;
	KeyStore keyStore;
	qDebug()<<"####################################### INITIALIZING";
	for(quint16 i=0; i<portRange; ++i) {
		const quint16 tport=basePort+i;
		CommsTester *cc=new CommsTester("instance_"+QString::number(tport), local, tport, basePort, portRange, testCount, keyStore);
		QVERIFY(nullptr!=cc);
		qDebug()<<" + INIT CourierTester "<<cc->toString();
		senders.push_back(cc);
	}
	qDebug()<<"####################################### STARTING";
	for(quint16 i=0; i<portRange; ++i) {
		//const quint16 tport=basePort+1+i;
		qDebug()<<" + STARTING CourierTester "<<senders[i]->toString();
		QVERIFY(QMetaObject::invokeMethod(senders[i], "startSendTest",  Qt::QueuedConnection));
	}
	qDebug()<<"####################################### RUNNING";
	for(quint16 i=0; i<portRange; ++i) {
		qApp->processEvents();
		qDebug()<<" + WAITING FOR CourierTester "<<senders[i]->toString();
		QSignalSpy spy(senders[i],SIGNAL(finished()));
		qApp->processEvents();

		spy.wait(11000);
		qApp->processEvents();
		qDebug()<<" + WAIT OVER FOR CourierTester "<<senders[i]->toString()<<" * * * * * * << GOT "<<spy.count()<<" FINISH SIGNALS";
	}
	const quint64 start=QDateTime::currentMSecsSinceEpoch();
	quint64 now=start;
	while(now<start+3000) {
		now=QDateTime::currentMSecsSinceEpoch();
		QCoreApplication::processEvents();
	}
	qDebug()<<"####################################### DELETING";
	for(quint16 i=0; i<portRange; ++i) {
		qDebug()<<" + DELETING CourierTester "<<senders[i]->toString();
		senders[i]->deleteLater();
	}
	senders.clear();
}



QTEST_MAIN(TestCommsChannel)
