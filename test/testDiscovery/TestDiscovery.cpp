#include "TestDiscovery.hpp"

#include "agent/Agent.hpp"
#include "app/AppContext.hpp"
#include "app/launcher/AppCommandLineParser.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "node/Node.hpp"
#include "remote/Remote.hpp"
#include "store/ASEvent.hpp"
#include "zoo/ZooServer.hpp"

#include <QSignalSpy>
#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QStandardPaths>



void DiscoveryTestCase::log(const QString &message, const LogType type) const{
	switch(type){
		default:
		case(LOG_TYPE_ERROR):{
			qFatal().nospace().noquote() << message;
		}break;
		case(LOG_TYPE_WARNING):{
			qWarning().nospace().noquote() << message;
		}break;
		case(LOG_TYPE_INFO):
		case(LOG_TYPE_DEBUG):{
			qDebug().nospace().noquote() << message;
		}break;
	}
}

QString b2s(bool b){
	return b ? "ON " : "OFF";
}

DiscoveryTestCase::DiscoveryTestCase(quint8 agentCount, quint8 remoteCount, bool useZoo, bool useNetwork, bool useBluetooth, bool useAudio, bool useNFC, bool useCamera)
	: agentCount(agentCount)
	, remoteCount(remoteCount)
	, useZoo(useZoo)
	, useNetwork(useNetwork)
	, useBluetooth(useBluetooth)
	, useAudio(useAudio)
	, useNFC(useNFC)
	, useCamera(useCamera)
	, agents(agentCount)
	, remotes(remoteCount)
{
	log(QString("DiscoveryTestCase(useZoo=%1, useNetwork=%2, useBluetooth=%3, useAudio=%4, useNFC=%5, useCamera=%6)").arg(b2s(useZoo)).arg(b2s(useNetwork)).arg(b2s(useBluetooth)).arg(b2s(useAudio)).arg(b2s(useNFC)).arg(b2s(useCamera)));
}


void DiscoveryTestCase::init(bool initialize)
{
	if(initialized != initialize){
		log(QString("init(%1 --> %2)").arg(b2s(initialized)).arg(b2s(initialize)));
		if(initialize){
			if(useZoo)toggleZooServer(initialize);
			if(useNetwork)toggleNetwork(initialize);
			if(useBluetooth)toggleBluetooth(initialize);
			if(useAudio)toggleAudio(initialize);
			if(useNFC)toggleNFC(initialize);
			if(useCamera)toggleCamera(initialize);
		}
		else{
			if(useCamera)toggleCamera(initialize);
			if(useNFC)toggleNFC(initialize);
			if(useAudio)toggleAudio(initialize);
			if(useBluetooth)toggleBluetooth(initialize);
			if(useNetwork)toggleNetwork(initialize);
			if(useZoo)toggleZooServer(initialize);
		}
		initialized = initialize;
	}
}


QSharedPointer<Agent> DiscoveryTestCase::createAgent(const QString &name) const{
	auto agentLauncher = QSharedPointer<AppLauncher<Agent>>::create();
	agentLauncher->configure(name);
	auto agent = QSharedPointer<Agent>::create();
	agent->appConfigure(agentLauncher);
	auto agentKeystore = agent->keyStore();
	agentKeystore->synchronize([=](ASEvent<QVariantMap> &se) {
		log(QString("AGENT  '%1' KEYSTORE READY=%2").arg(name).arg(b2s(se.isSuccessfull())));
		agent->discoveryClient()->activate(true);
	} );
	log(QString("AGENT  APP DIR: %1").arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)));
	return agent;
}


QSharedPointer<Remote> DiscoveryTestCase::createRemote(const QString &name) const{
	auto remoteLauncher = QSharedPointer<AppLauncher<Remote>>::create();
	remoteLauncher->configure(name);
	auto remote = QSharedPointer<Remote>::create();
	remote->appConfigure(remoteLauncher);
	auto keystore = remote->keyStore();
	keystore->synchronize([=](ASEvent<QVariantMap> &se) {
		log(QString("REMOTE '%1' KEYSTORE READY=%2").arg(name).arg(b2s(se.isSuccessfull())));
		remote->discoveryClient()->activate(true);
	} );
	log(QString("REMOTE APP DIR: %1").arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)));
	return remote;
}



bool DiscoveryTestCase::zooOnline() const{
	return !zooServer.isNull();
}


void DiscoveryTestCase::toggleZooServer(bool initialize)
{
	auto initialized = zooOnline();
	if(initialized != initialize){
		log("ZOO -----------------------");
		log(QString("toggleZooServer(%1 --> %2)").arg(b2s(initialized)).arg(b2s(initialize)));
		if(initialize){
			QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
			auto opts = QSharedPointer<AppCommandLineParser>::create();
			QCOMPARE_NE(opts, nullptr);
			auto zooContext = QSharedPointer<AppContext>::create(opts, env, "DiscoveryTestCaseZoo", false);
			QCOMPARE_NE(zooContext, nullptr);
			zooServer = QSharedPointer<ZooServer>::create();
			QCOMPARE_NE(zooServer, nullptr);
			zooServer->configure(zooContext);
			zooServer->start(zooPort);
			log("ZOO    APP DIR: "+QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
		}
		else{
			zooServer->stop();
			zooServer = nullptr;
		}
	}
}


void DiscoveryTestCase::toggleAgents(bool initialize)
{
	for(quint8 i = 0; i < agentCount; ++i){
		auto initialized = agents.size() >= i && !agents[i].isNull();
		if(initialized != initialize){
			log(QString("AGENT  %1 -----------------------").arg(i));
			log(QString("toggleAgent(%1: %2 --> %3)").arg(i).arg(b2s(initialized)).arg(b2s(initialize)));
			if(initialize){
				auto agent = createAgent("Agent-1");
				QCOMPARE_NE(agent, nullptr);
				agents[i] = agent;
			}
			else{
				auto agent = agents[i];
				if(!agent.isNull()){
					agent->discoveryClient()->activate(false);
					agents[i].clear();
				}
			}
		}
	}
}



void DiscoveryTestCase::toggleRemotes(bool initialize)
{
	for(quint8 i = 0; i < remoteCount; ++i){
		auto initialized = remotes.size() >= i && !remotes[i].isNull();
		if(initialized != initialize){
			log(QString("REMOTE %1 -----------------------").arg(i));
			log(QString("toggleRemote(%1: %2 --> %3)").arg(i).arg(b2s(initialized), b2s(initialize)));
			if(initialize){
				auto remote = createRemote("Remote-1");
				QCOMPARE_NE(remote, nullptr);
				remotes[i] = remote;
			}
			else{
				auto remote = remotes[i];
				if(!remote.isNull()){
					remote->discoveryClient()->activate(false);
					remotes[i].clear();
				}
			}
		}
	}
}


void DiscoveryTestCase::toggleNetwork(bool initialize){
	log(QString("toggleNetwork(%1 --> %2)").arg(b2s(initialize), b2s(initialize)));
}


void DiscoveryTestCase::toggleBluetooth(bool initialize){
	log(QString("toggleBluetooth(%1 --> %2)").arg(b2s(initialize)).arg(b2s(initialize)));
}


void DiscoveryTestCase::toggleAudio(bool initialize){
	log(QString("toggleAudio(%1 --> %2)").arg(b2s(initialize)).arg(b2s(initialize)));
}


void DiscoveryTestCase::toggleNFC(bool initialize){
	log(QString("toggleNFC(%1 --> %2)").arg(b2s(initialize)).arg(b2s(initialize)));
}


void DiscoveryTestCase::toggleCamera(bool initialize){
	log(QString("toggleCamera(%1 --> %2)").arg(b2s(initialize)).arg(b2s(initialize)));
}


////////////////////////////



void TestDiscovery::testCases()
{
	DiscoveryTestCase dtc(1, 1, true, true, true, true, true, true);
	dtc.init(true);
	int spyWait=20000;//20 sec
	qDebug()<<"SPYTIME: "<<spyWait;
	QTest::qSleep(spyWait);
	dtc.init(false);
}




void TestDiscovery::testOld()
{
	qDebug()<<"ZOO -----------------------";
	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
	auto opts = QSharedPointer<AppCommandLineParser>::create();
	QCOMPARE_NE(opts, nullptr);
	auto zooContext = QSharedPointer<AppContext>::create(opts, env, "testDiscoveryZoo", false);
	QCOMPARE_NE(zooContext, nullptr);
	QString port="8123";
	auto zooServer = QSharedPointer<ZooServer>::create();
	QCOMPARE_NE(zooServer, nullptr);
	zooServer->configure(zooContext);
	zooServer->start(port);
	qDebug()<<"ZOO    APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"AGENT -----------------------";
	auto agentLauncher = QSharedPointer<AppLauncher<Agent>>::create();
	agentLauncher->configure("testDiscoveryAgent");
	auto testAgent = QSharedPointer<Agent>::create();
	QCOMPARE_NE(testAgent, nullptr);
	testAgent->appConfigure(agentLauncher);
	auto agentKeystore = testAgent->keyStore();
	QCOMPARE_NE(agentKeystore, nullptr);
	agentKeystore->synchronize([testAgent](ASEvent<QVariantMap> &se) {
		qDebug()<<"AGENT KEYSTORE READY "<<se.isSuccessfull();
		testAgent->discoveryClient()->activate(true);
	} );
	qDebug()<<"AGENT  APP DIR: "<<QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

	qDebug()<<"REMOTE -----------------------";
	auto remoteLauncher = QSharedPointer<AppLauncher<Remote> >::create();
	remoteLauncher->configure("testDiscoveryRemote");
	auto testRemote = QSharedPointer<Remote>::create();
	QCOMPARE_NE(testRemote, nullptr);
	testRemote->appConfigure(remoteLauncher);
	auto remoteKeystore = testRemote->keyStore();
	QCOMPARE_NE(remoteKeystore, nullptr);
	remoteKeystore->synchronize([testRemote](ASEvent<QVariantMap> &se) {
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
