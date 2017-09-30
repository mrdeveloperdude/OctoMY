#include "TestCommsChannel.hpp"

#include "CommsTester.hpp"

/*
Sets up X commchannels and have them sending data
via courier to a random but deterministically selected subset of X.

Result should stress the system in a way that lays bare any shortcomings
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
	CommsCarrierUDP carrier;
	KeyStore keyStore;
	qDebug()<<"####################################### INITIALIZING";
	for(quint16 i=0; i<portRange; ++i) {
		const quint16 tport=basePort+i;
		QString peersFilenameB;
		Key keyB;
		AddressBook peersB(peersFilenameB);
		peersB.bootstrap(false,false);
		QVariantMap peerMapB;
		QString nameB="PARTY B";
		/*
		QVariantMap addrBMap=local.toVariantMap();
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
		QSharedPointer<NodeAssociate> partB(OC_NEW NodeAssociate(peerMapB));
		CommsTester *cc=OC_NEW CommsTester("instance_"+QString::number(tport), local, tport, basePort, portRange, testCount, keyStore, peersB, carrier);

		QVERIFY(nullptr!=cc);
		qDebug()<<" + INIT CourierTester "<<cc->toString();
		senders.push_back(cc);
		*/
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
