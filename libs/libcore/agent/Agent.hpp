#ifndef AGENT_HPP
#define AGENT_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"

#include "widgets/TryToggle.hpp"
#include "camera/CameraList.hpp"

#include "basic/Node.hpp"
#include "basic/NodeLauncher.hpp"


#include <QObject>
#include <QList>
#include <QCommandLineParser>

class ZooClient;
class DiscoveryClient;
class AgentWindow;
class AgentStateCourier;
class ISyncParameter;
class Agent;


class CourierSet: public QList<Courier *>
{
private:
	Agent &mAgent;
	QDataStream mDS;
	AgentStateCourier *mAgentStateCourier;
	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

public:
	explicit CourierSet(ClientSignature &sig, Agent &agent);
	virtual ~CourierSet();
public:

	AgentStateCourier *agentStateCourier();

	void setCommsEnabled(bool enable);
};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




class AgentControls
{
private:

	Agent &mAgent;
	QHash <quint64, CourierSet *> mCouriers;
public:

	explicit AgentControls(Agent &);
	virtual	~AgentControls();

public:
	void registerClient(ClientSignature &sig);
	void unRegisterClient(ClientSignature &sig);

	void setCommsEnabled(bool);

	CourierSet* activeControl() const;
};




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



class Agent : public Node
{
	Q_OBJECT
private:
	AgentControls mControls;
public:
	AgentWindow *window;
public:
	explicit Agent(NodeLauncher<Agent> &launcher, QObject *parent = nullptr);
	virtual ~Agent();

	virtual QWidget *showWindow();

	void setPanic(bool);


	const AgentControls &controls() const ;

	// NodeAssociate slots
public slots:
	void onPeerStoreReady(bool);
	void onPeerAdded(QString);
	void onPeerRemoved(QString);
	void onPeersChanged();

	//Agent State Courier slots
public slots:
	void onSyncParameterChanged(ISyncParameter *);

	// CommsChannel slots
private slots:
	virtual void onCommsError(QString) Q_DECL_OVERRIDE;
	virtual void onCommsClientAdded(Client *)Q_DECL_OVERRIDE;
	virtual void onCommsConnectionStatusChanged(bool)Q_DECL_OVERRIDE;



};



#endif // AGENT_HPP
