#include "TestDiscovery.hpp"

#include "zoo/ZooServer.hpp"
#include "client/zoo/ZooClient.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "node/NodeRole.hpp"
#include "node/Node.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "app/launcher/AppCommandLineParser.hpp"

#include "app/AppContext.hpp"
#include "agent/Agent.hpp"
#include "remote/Remote.hpp"
#include "store/ASEvent.hpp"

#include "uptime/New.hpp"

#include <QSignalSpy>

#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QStandardPaths>


void TestDiscovery::test()
{
	qDebug()<<"ZOO -----------------------";
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	QSharedPointer<AppCommandLineParser> opts(OC_NEW AppCommandLineParser());
	QVERIFY(!opts.isNull());
	QSharedPointer<AppContext> zooContext(OC_NEW AppContext(opts, env, "testDiscoveryZoo", false) );
	QVERIFY(!zooContext.isNull());
	QString port="8123";
	QSharedPointer<ZooServer> zooServer(OC_NEW ZooServer) ;
	zooServer->configure(zooContext);
	QVERIFY(nullptr!=zooServer);
	zooServer->start(port);
	qDebug()<<"ZOO    APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"AGENT -----------------------";
	QSharedPointer<AppLauncher<Agent>> agentLauncher(OC_NEW AppLauncher<Agent>() );
	agentLauncher->configure("testDiscoveryAgent");
	QSharedPointer<Agent> testAgent(OC_NEW Agent());
	QVERIFY(!testAgent.isNull());
	testAgent->appConfigure(agentLauncher);
	QSharedPointer<KeyStore> agentKeystore=testAgent->keyStore();
	QVERIFY(!agentKeystore.isNull());
	agentKeystore->synchronize([=](ASEvent<QVariantMap> &se) {
		qDebug()<<"AGENT KEYSTORE READY "<<se.isSuccessfull();
		testAgent->discoveryClient()->activate(true);
	} );
	qDebug()<<"AGENT  APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"REMOTE -----------------------";
	QSharedPointer<AppLauncher<Remote> > remoteLauncher(OC_NEW AppLauncher<Remote>() );
	remoteLauncher->configure("testDiscoveryRemote");
	QSharedPointer<Node> testRemote(OC_NEW Remote());
	QVERIFY(!testRemote.isNull());
	testRemote->appConfigure(remoteLauncher);
	QSharedPointer<KeyStore> remoteKeystore(testRemote->keyStore());
	QVERIFY(!remoteKeystore.isNull());
	remoteKeystore->synchronize([=](ASEvent<QVariantMap> &se) {
		qDebug()<<"REMOTE KEYSTORE READY "<<se.isSuccessfull();
		testRemote->discoveryClient()->activate(true);
	} );
	qDebug()<<"REMOTE APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"DE-INIT -----------------------";

	int spyWait=20000;//20 sec
	qDebug()<<"SPYTIME: "<<spyWait;
	QTest::qSleep(spyWait);
	testAgent->discoveryClient()->activate(false);
	testRemote->discoveryClient()->activate(false);
}


OC_TEST_MAIN(test, TestDiscovery)
