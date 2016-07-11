#include "TestDiscovery.hpp"

#include "zoo/ZooServer.hpp"
#include "zoo/ZooClient.hpp"
#include "comms/discovery/DiscoveryClient.hpp"
#include "comms/discovery/DiscoveryRole.hpp"
#include "basic/Node.hpp"
#include "basic/AppContext.hpp"

#include <QSignalSpy>

#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QStandardPaths>

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestDiscovery::test(){

	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	QCommandLineParser opts;
	opts.setApplicationDescription("Test Discovery");
	opts.addHelpOption();

	AppContext *context=new AppContext(opts, env, "testDiscoveryAgent", this);
	QVERIFY(nullptr!=context);


	/*
	QString port="8123";
	ZooServer *zooServer=new ZooServer (opts, env, this);
	QVERIFY(nullptr!=zooServer);
	zooServer->start(port);
*/


	//	qDebug()<<"APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);



	Node *testAgent=new Node(context, DiscoveryRole::ROLE_AGENT, DiscoveryType::TYPE_AGENT, this);
	QVERIFY(nullptr!=testAgent);
	KeyStore *agentKeystore=&testAgent->getKeyStore();
	QVERIFY(nullptr!=agentKeystore);
	QSignalSpy spyAgentKeyReady(agentKeystore,SIGNAL(keystoreReady(bool)));
	bool conret1=connect(agentKeystore, &KeyStore::keystoreReady, [=](bool ok){
		qDebug()<<"AGENT KEYSTORE READY "<<ok;
		QVERIFY(agentKeystore->isReady());
		QVERIFY(!agentKeystore->hasError());
	} );

	QVERIFY(conret1);

	/*
	qDebug()<<"APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	Node *testRemote=new Node(opts, "testDiscoveryRemote", DiscoveryRole::ROLE_CONTROL, DiscoveryType::TYPE_REMOTE, this);
	QVERIFY(nullptr!=testRemote);
	KeyStore *remoteKeystore=&testRemote->getKeyStore();
	QVERIFY(nullptr!=remoteKeystore);
	QSignalSpy spyRemoteKeyReady(remoteKeystore, SIGNAL(keystoreReady(bool)));
	bool conret2=connect(remoteKeystore, &KeyStore::keystoreReady, [=](){
		qDebug()<<"REMOTE KEYSTORE READY";
		QVERIFY(remoteKeystore->isReady());
		QVERIFY(!remoteKeystore->isError());
	} );
	QVERIFY(conret2);
	qDebug()<<"APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
*/
	quint64 spyWait=20000;//20 sec
	QVERIFY(spyAgentKeyReady.wait(spyWait));	QVERIFY(1==spyAgentKeyReady.count());

	//QVERIFY(spyRemoteKeyReady.wait(spyWait));	QVERIFY(1==spyRemoteKeyReady.count());

	//zooServer->start(port);

}




QTEST_MAIN(TestDiscovery)
