#include "TestDiscovery.hpp"

#include "zoo/ZooServer.hpp"
#include "client/zoo/ZooClient.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "node/NodeRole.hpp"
#include "node/Node.hpp"
#include "app/NodeLauncher.hpp"
#include "app/AppContext.hpp"
#include "agent/Agent.hpp"
#include "remote/Remote.hpp"

#include <QSignalSpy>

#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QStandardPaths>


void TestDiscovery::test()
{
	qDebug()<<"INIT -----------------------";
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	QCommandLineParser opts;
	opts.setApplicationDescription("Test Discovery");
	opts.addHelpOption();

	AppContext *zooContext=OC_NEW AppContext(opts, env, "testDiscoveryZoo", this);
	AppContext *agentContext=OC_NEW AppContext(opts, env, "testDiscoveryAgent", this);
	AppContext *remoteContext=OC_NEW AppContext(opts, env, "testDiscoveryRemote", this);

	QVERIFY(nullptr!=zooContext);
	qDebug()<<"ZOO -----------------------";
	QString port="8123";
	ZooServer *zooServer=OC_NEW ZooServer (zooContext, this);
	QVERIFY(nullptr!=zooServer);
	zooServer->start(port);
	qDebug()<<"ZOO    APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	QVERIFY(nullptr!=agentContext);
	qDebug()<<"AGENT -----------------------";
	NodeLauncher<Agent> agentLauncher;
	Node *testAgent=OC_NEW Node(agentLauncher, agentContext, NodeRole::ROLE_AGENT, NodeType::TYPE_AGENT, this);
	QVERIFY(nullptr!=testAgent);
	KeyStore *agentKeystore=&testAgent->keyStore();
	QVERIFY(nullptr!=agentKeystore);
	agentKeystore->synchronize([=](SimpleDataStore &sds, bool ok) {
		qDebug()<<"AGENT KEYSTORE READY "<<ok;
		testAgent->updateDiscoveryClient();
		testAgent->discoveryClient()->setStart(true);
//			Jeg avbryter herved denne samtalen fordi jeg tror ikke vi er noe god match for hverandre. Du er ute etter "kjærligheten" og jeg er ute etter en livlig samtalepartner. Så langt har jeg måtte hale ut av deg hver minste lille detalj,
	} );
	qDebug()<<"AGENT  APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	QVERIFY(nullptr!=remoteContext);
	qDebug()<<"REMOTE -----------------------";
	NodeLauncher<Remote> remoteLauncher;
	Node *testRemote=OC_NEW Node(remoteLauncher, remoteContext, NodeRole::ROLE_CONTROL, NodeType::TYPE_REMOTE, this);
	QVERIFY(nullptr!=testRemote);
	KeyStore *remoteKeystore=&testRemote->keyStore();
	QVERIFY(nullptr!=remoteKeystore);
	remoteKeystore->synchronize([=](SimpleDataStore &sds, bool ok) {
		qDebug()<<"REMOTE KEYSTORE READY "<<ok;
		testRemote->updateDiscoveryClient();
		testRemote->discoveryClient()->setStart(true);
	} );
	qDebug()<<"REMOTE APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"DE-INIT -----------------------";

	qint64 spyWait=20000;//20 sec
	qDebug()<<"SPYTIME: "<<spyWait;
	QTest::qSleep(spyWait);
	if(nullptr!=agentContext) {
		testAgent->discoveryClient()->setStart(false);
	}
	if(nullptr!=remoteContext) {
		testRemote->discoveryClient()->setStart(false);
	}
}


OC_TEST_MAIN(test, TestDiscovery)
