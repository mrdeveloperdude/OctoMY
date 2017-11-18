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




class Agent : public Node
{
	Q_OBJECT
private:

	AgentConfigStore mAgentConfigStore;
	IActuatorController *mActuatorController;

	AgentWindow *mWindow;

	QSharedPointer <Agent> mThis;

public:
	explicit Agent(NodeLauncher<Agent> &launcher, QObject *parent = nullptr);
	virtual ~Agent();

	virtual QWidget *showWindow();


public:

	void setPanic(bool);

	AgentConfigStore &configurationStore();

	QSharedPointer<PoseMapping> poseMapping();

	IActuatorController *actuatorController();
	void reloadController();


	void setNodeCouriersRegistered(bool reg) Q_DECL_OVERRIDE;

	// Agent Config Store slots
public slots:
	void onAgentConfigStoreReady(bool);

	// NodeAssociate slots
public slots:
	void onAddressBookReady(bool);
	void onAssociateAdded(QString);
	void onAssociateRemoved(QString);
	void onAssociateChanged();

	//Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *);

	// CommsChannel slots
private slots:
	virtual void onCommsError(QString) Q_DECL_OVERRIDE;
	virtual void onCommsClientAdded(CommsSession *)Q_DECL_OVERRIDE;
	virtual void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)Q_DECL_OVERRIDE;



};



#endif // AGENT_HPP
