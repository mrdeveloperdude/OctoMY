#ifndef TESTDISCOVERY_HPP
#define TESTDISCOVERY_HPP

#include "test/Common.hpp"

#include "log/LogType.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class ZooServer;
class Agent;
class Remote;

struct DiscoveryTestCase{
	const quint8 agentCount;
	const quint8 remoteCount;
	const QString zooPort{"8123"};
	const bool useZoo{false};
	const bool useNetwork{false};
	const bool useBluetooth{false};
	const bool useAudio{false};
	const bool useNFC{false};
	const bool useCamera{false};
	bool initialized{false};
	QSharedPointer<ZooServer> zooServer;
	QVector<QSharedPointer<Agent> > agents;
	QVector<QSharedPointer<Remote> > remotes;

	explicit DiscoveryTestCase(quint8 agentCount, quint8 remoteCount, bool useZoo, bool useNetwork, bool useBluetooth, bool useAudio, bool useNFC, bool useCamera);
	void log(const QString &message, const LogType type=LOG_TYPE_INFO) const;

	bool zooOnline() const;
	void init(bool initialize);
	void toggleZooServer(bool initialize);
	void toggleAgents(bool initialize);
	void toggleRemotes(bool initialize);
	void toggleNetwork(bool initialize);
	void toggleBluetooth(bool initialize);
	void toggleAudio(bool initialize);
	void toggleNFC(bool initialize);
	void toggleCamera(bool initialize);

private:
	QSharedPointer<Agent> createAgent(const QString &name) const;
	QSharedPointer<Remote> createRemote(const QString &name) const;

};


class TestDiscovery:public QObject
{
	Q_OBJECT
	
	void testOld();
private slots:
	void testCases();

};


#endif
// TESTDISCOVERY_HPP
