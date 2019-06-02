#ifndef AGENT_HPP
#define AGENT_HPP


#include "node/Node.hpp"
#include "app/launcher/AppLauncher.hpp"

#include "AgentConfigStore.hpp"

#include "hardware/controllers/IActuatorController.hpp"

#include <QObject>
#include <QList>
#include <QCommandLineParser>
#include <QEnableSharedFromThis>
#include <QVariantMap>


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
	explicit Agent(AppLauncher<Agent> &launcher, QObject *parent = nullptr);
	virtual ~Agent() Q_DECL_OVERRIDE;

public:
	// Called by launcher to give the app a chance to initialize
	// After calling this, the launcher will pause further initialization and showing of windows until app emits an "appInitDone" signal
	void appInit() Q_DECL_OVERRIDE;

	// Called by launcher as a response to an appRequestClose event being emitted by anyone that wants the app to close.
	// After calling this, the launcher will complete termination and return to OS
	void appDeInit() Q_DECL_OVERRIDE;


	// Called by launcher to get a handle to the app's main window
	// Will be called when launcher wants to show window during initialization
	// NOTE: Launcher will NOT hide the window, as it is expected that app should do this by itself upon de-initialization / destruction
	QSharedPointer<QWidget> appWindow() Q_DECL_OVERRIDE;

// Implemented  in Node.cpp
	/*
	// IConnectionStatus interface
public:
	// We want to go online. We might not be online and we might
	bool needsConnection() Q_DECL_OVERRIDE;
	void setNeedsConnection(bool) Q_DECL_OVERRIDE;

	// We are currently online. We might not need to be online
	bool isConnected() Q_DECL_OVERRIDE;
	void setConnected(bool) Q_DECL_OVERRIDE;
	*/

public:
	void setPanic(bool);
	AgentConfigStore &configurationStore();
	QSharedPointer<PoseMapping> poseMapping();
	IActuatorController *actuatorController();
	void unloadController();
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


signals:
	// Expected to be emitted when someone wants app to terminate.
	// Will be caught by app launcher and app launcher will subsequently call appDeInit() to bring termination to fruition
	void appRequestClose();

	// Expected to be emitted by app as soon as initialization is done in appInit();
	void appInitDone();

	// Expected to be emitted by app during termination as soon as it no longer needs the eventloop in appDeInit().
	void appDeInitDone();

};




template <typename F>
void Agent::synchronizeLocalIdentity(F callBack)
{
	OC_METHODGATE();
	Q_UNUSED(callBack);
	/*
	mLocalIdentity.synchronize([=](SimpleDataStore &sms, bool ok) {
		QVariantMap map=sms.toMap();
		qDebug()<<"Local identity synchronized with ok="<<ok<<" and map="<<map;
		//callBack(ok);
		callBack(map, ok);
	});
	*/
}

#endif
// AGENT_HPP
