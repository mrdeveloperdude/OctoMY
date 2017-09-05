#include "Agent.hpp"
#include "AgentWindow.hpp"

#include "utility/Standard.hpp"

#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"
#include "comms/couriers/Courier.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"
#include "comms/SyncParameter.hpp"

#include "discovery/DiscoveryClient.hpp"
#include "zoo/ZooClient.hpp"
#include "node/AppContext.hpp"
#include "hw/controllers/ActuatorControllerFactory.hpp"

#include "AgentCourierSet.hpp"

#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



Agent::Agent(NodeLauncher<Agent> &launcher, QObject *parent)
	: Node(OC_NEW AppContext(launcher.getOptions(), launcher.getEnvironment(), "agent", parent), ROLE_AGENT, TYPE_AGENT, parent)
	, mControls(*this)
	, mAgentConfigStore(mContext->baseDir() + "/agent_config.json")
	, mActuatorController(nullptr)
	, mWindow(nullptr)
{
	OC_METHODGATE();
	mPeers.hookSignals(*this);
	if(mPeers.isReady()) {
		onPeerStoreReady(true);
	}
	mAgentConfigStore.bootstrap(true, false);
}

Agent::~Agent()
{
	OC_METHODGATE();
	if(nullptr!=mActuatorController) {
		mActuatorController->setConnected(false);
	}
}




QWidget *Agent::showWindow()
{
	OC_METHODGATE();
	if(nullptr==mWindow) {
		mWindow=OC_NEW AgentWindow(this, nullptr);
	}
	if(nullptr!=mWindow) {
		mWindow->show();
	}
	return mWindow;
}



QSet<QSharedPointer<NodeAssociate> > Agent::activeControls()
{
	OC_METHODGATE();
	QSet<QSharedPointer<NodeAssociate> > out;

	const quint64 lastActive=QDateTime::currentMSecsSinceEpoch()-(1000*60);//One minute ago. TODO: Turn into constant or setting

	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		NodeAssociateStore &p=peers();
		CommsSessionDirectory &sessionDirectory=cc->sessions();
		QSet<QSharedPointer<CommsSession> > sessions=sessionDirectory.getByActiveTime(lastActive);
		for(QSharedPointer<CommsSession> session: sessions) {
			auto key=session->key();
			if(nullptr!=key) {
				QString id=key->id();
				if(id.size()>0) {
					QSharedPointer<NodeAssociate> peer=p.getParticipant(id);
					if(nullptr!=peer) {
						if(DiscoveryRole::ROLE_CONTROL==peer->role()) {
							out<<peer;
						}
					}
				}
			} else {
				qWarning()<<"ERROR: No key";
			}
		}
	}

	return out;
}

void Agent::setCourierRegistration(QSharedPointer<NodeAssociate> assoc, bool reg)
{
	OC_METHODGATE();
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		if(nullptr != assoc) {
			const QString id=assoc->id();
			if(reg) {
				qDebug()<<"REGISTERING COURIERS FOR " <<id;
				QSet< QSharedPointer<Courier> > set;
				Courier *courier=OC_NEW SensorsCourier(*cc);
				set<< QSharedPointer<Courier>(courier);
				//QMap<const QString, QSet< QSharedPointer<Courier> > > mCourierSets;
				mCourierSets.insert(id, set);

				cc->setCourierRegistered(*courier, true);

			} else {
				qDebug()<<"UN-REGISTERING COURIERS FOR " <<id;
				if(mCourierSets.contains(id)) {
					QSet< QSharedPointer<Courier> > set=mCourierSets.take(id);
					for(QSharedPointer<Courier>  courier:set) {
						if(nullptr!=courier) {

							cc->setCourierRegistered(*courier, false);
						}
					}
				}
			}
		}
		// Adaptively start commschannel when there are couriers registered
		const int ct=cc->courierCount();
		if(ct>0) {
			if(nullptr != assoc) {
				cc->start(assoc->localAddress());
			}
		} else {
			cc->stop();
		}
	}

}

void Agent::updateCourierRegistration()
{
	OC_METHODGATE();
	QSet<QSharedPointer<NodeAssociate> > active=activeControls();
	if(active!=mLastActiveControls) {
		for(QSharedPointer<NodeAssociate> assoc:mLastActiveControls) {
			if(!active.contains(assoc)) {
				//Decomission it
				setCourierRegistration(assoc,false);
			}
		}
		for(QSharedPointer<NodeAssociate> assoc:active) {
			if(!mLastActiveControls.contains(assoc)) {
				//Comission it
				setCourierRegistration(assoc,true);
			}
		}
	}
	mLastActiveControls.clear();
	mLastActiveControls=active;
}



void Agent::setPanic(bool panic)
{
	OC_METHODGATE();
	AgentCourierSet *set=mControls.activeControl();

	if(nullptr!=set) {
		AgentStateCourier *asc=set->agentStateCourier();
		if(nullptr!=asc) {
			asc->setPanic(panic);
		} else {
			qWarning()<<"ERROR: No active control";
		}
	} else {
		qWarning()<<"ERROR: No set";
	}

}



const AgentControls &Agent::controls() const
{
	OC_METHODGATE();
	return mControls;
}
AgentConfigStore &Agent::configurationStore()
{
	OC_METHODGATE();
	return mAgentConfigStore;
}

QSharedPointer<PoseMapping> Agent::poseMapping()
{
	return mAgentConfigStore.agentConfig()->poseMapping();
}

IActuatorController *Agent::actuatorController()
{
	return mActuatorController;
}

void Agent::reloadController()
{
	// Unload old controller
	if(nullptr!=mActuatorController) {
		qDebug()<<"Unloading old controller";
		mActuatorController->setConnected(false);
		mActuatorController->deleteLater();
		mActuatorController=nullptr;
	}
	QSharedPointer<AgentConfig> configp=mAgentConfigStore.agentConfig();
	if(!configp.isNull()) {
		AgentConfig &config=*configp;
		QString controllerName=config.controllerName().trimmed();
		if(!controllerName.isEmpty()) {
			qDebug()<<"Attempting to generate controller of type "<<controllerName;
			ActuatorControllerFactory factory;
			mActuatorController=factory.controllerFactory(controllerName);
			if(nullptr!=mActuatorController) {
				qDebug()<<"Controller created, configuring";
				QVariantMap controllerConfig=config.controllerConfig();
				mActuatorController->setConfiguration(controllerConfig);
				mActuatorController->setConnected(true);
			}
		} else {
			qDebug()<<"No controller named in agent config";
		}
	} else {
		qDebug()<<"No agent config";
	}
}

//////////////////////////////////////////////////
// Agent Config Store slots



void Agent::onAgentConfigStoreReady(bool ok)
{
	qDebug()<<"Agent config store ready";
	reloadController();
}



//////////////////////////////////////////////////
// Node Associate Store slots
void Agent::onPeerStoreReady(bool ready)
{
	OC_METHODGATE();
	qDebug()<<"AGENT found peer store "<< (ready?"READY":"UNREADY");
	QMap<QString, QSharedPointer<NodeAssociate> > &peers=mPeers.getParticipants();
	for(QSharedPointer<NodeAssociate> peer:peers) {
		NetworkAddress sig=peer->localAddress();
		if(sig.isValid() && ROLE_CONTROL==peer->role()) {
			qDebug()<<"Adding "<<sig.toString()<<"("<<peer->toString()<<") to agent control's list";
			mControls.registerClient(peer->id());
		}
	}
	mControls.setCommsEnabled(true);
}



void Agent::onPeerAdded(QString)
{
	OC_METHODGATE();
}

void Agent::onPeerRemoved(QString)
{
	OC_METHODGATE();
}

void Agent::onPeersChanged()
{
	OC_METHODGATE();
}



//////////////////////////////////////////////////
// Agent State Courier slots

void Agent::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
	//qDebug()<<"Agent ASC changed: "<<sp->toString();
	if(nullptr!=sp) {
		if("TargetPose"==sp->name()) {
			SyncParameter<Pose> *targetPoseParameter=(SyncParameter<Pose> *)sp;
			Pose targetPose=targetPoseParameter->bestValue(true);
			qDebug()<<"TARGET POSE: "<<targetPose.toString();
			if(nullptr!=mActuatorController) {
				mActuatorController->move(targetPose);
			}
		}
	} else {
		qWarning()<<"ERROR: sp was nullptr";
	}
}


//////////////////////////////////////////////////
// CommsChannel slots


void Agent::onCommsError(QString e)
{
	OC_METHODGATE();
	//qDebug()<<"AGENT UNIMP Comms error: "<<e;
}

void Agent::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	//qDebug()<<"AGENT UNIMP Client added: "<<c->toString();
}

void Agent::onCommsConnectionStatusChanged(bool s)
{
	OC_METHODGATE();
	//qDebug() <<"AGENT UNIMP New connection status: "<<(s?"ONLINE":"OFFLINE");
}
