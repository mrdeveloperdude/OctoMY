#ifndef AGENT_HPP
#define AGENT_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"

#include "widgets/TryToggle.hpp"
#include "camera/CameraList.hpp"

#include "node/Node.hpp"
#include "node/NodeLauncher.hpp"

#include "AgentConfigStore.hpp"
#include "hw/controllers/IActuatorController.hpp"


#include <QObject>
#include <QList>

#include <QCommandLineParser>

class ZooClient;
class DiscoveryClient;
class AgentWindow;
class AgentStateCourier;
class ISyncParameter;
class Agent;

class AgentCourierSet;




class Agent : public Node, public QEnableSharedFromThis<Agent>
{
	Q_OBJECT
private:
	AgentConfigStore mAgentConfigStore;
	IActuatorController *mActuatorController;

	QSharedPointer<AgentWindow> mWindow;


	bool mKeyStoreReady;
	bool mLocalIdentityStoreReady;
	bool mAgentConfigStoreReady;


public:
	explicit Agent(NodeLauncher<Agent> &launcher, QObject *parent = nullptr);
	virtual ~Agent();

	virtual QSharedPointer<QWidget> showWindow();


public:

	virtual void init();
	virtual void deInit();

	void setPanic(bool);

	AgentConfigStore &configurationStore();

	QSharedPointer<PoseMapping> poseMapping();

	IActuatorController *actuatorController();
	void reloadController();


public:

	void identityChanged() Q_DECL_OVERRIDE;

	void setNodeCouriersRegistration(bool reg) Q_DECL_OVERRIDE;

	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;




public:

	template <typename F>
	void synchronizeLocalIdentity(F callBack);


private:

	bool checkLoadCompleted();


	//Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *);

};


#include <QVariantMap>

template <typename F>
void Agent::synchronizeLocalIdentity(F callBack)
{
	mLocalIdentity.synchronize([=](SimpleDataStore &sms, bool ok) {
		QVariantMap map=sms.toMap();
		qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
		//callBack(ok);
		callBack(map, ok);
	});
}



#endif // AGENT_HPP
