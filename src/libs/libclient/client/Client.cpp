#include "Client.hpp"

#include "node/Node.hpp"
#include "comms/CommsSession.hpp"
#include "comms/CommsChannel.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "ClientConnectionStatus.hpp"

static const QString CLIENT_ONLINE_STATUS_BASE_KEY("octomy.client.online.");

Client::Client(QObject *parent)
	: QObject(parent)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING LIENT mController="<<(nullptr!=mNode?mNode->name():"NULL")<<", mNodeAssoc="<<(nullptr!=mNodeAssociate?mNodeAssociate->name():"NULL")<<", parent="<<parent;
	initTimer();
}


Client::~Client()
{
	OC_METHODGATE();
}


void Client::configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc)
{
	OC_METHODGATE();
	mNode=node;
	mAssociate=nodeAssoc;
}


bool Client::needsConnection()
{
	OC_METHODGATE();
	const bool def=false;
	if(!mNode.isNull()) {
		return mNode->settings()->getCustomSettingBool(CLIENT_ONLINE_STATUS_BASE_KEY+associate()->id(), def);
	}
	return def;
}


bool Client::isConnected()
{
	OC_METHODGATE();
	/*
	const bool def=false;
	if(!mNode.isNull()){
		return mNode->settings()->getCustomSettingBool(CLIENT_ONLINE_STATUS_BASE_KEY+nodeAssociate()->id(), def);
	}
	return def;
	*/
	return false;
}


void Client::setNeedsConnection(const bool current)
{
	OC_METHODGATE();
	qDebug()<<"CLIENT::set needs connection: "<<current;
	if(!mNode.isNull()) {
		const QString key=CLIENT_ONLINE_STATUS_BASE_KEY+associate()->id();
		const bool last=needsConnection();
		if(current!=last) {
			mNode->settings()->setCustomSettingBool(key, current);
			updateCourierRegistration();
			QSharedPointer<CommsChannel> comms=mNode->comms();
			if(!comms.isNull()) {
				comms->updateConnection();
			}
		}
	}
}



void Client::setConnected(bool)
{
	OC_METHODGATE();
}



ClientConnectionStatus Client::connectionStatus()
{
	OC_METHODGATE();
	return ClientConnectionStatus( QEnableSharedFromThis<Client>::sharedFromThis() );
}


void Client::initTimer()
{
	OC_METHODGATE();
	mUpdateTimer.setInterval(100);
	if(!connect(&mUpdateTimer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	mUpdateTimer.start();
}

QSharedPointer<Node> Client::node()
{
	OC_METHODGATE();
	return mNode;
}


QSharedPointer<CommsChannel> Client::comms()
{
	OC_METHODGATE();
	return mNode.isNull()?nullptr:mNode->comms();
}


QSharedPointer<Associate> Client::associate()
{
	OC_METHODGATE();
	return mAssociate;
}





bool Client::courierRegistration()
{
	OC_METHODGATE();
	QSharedPointer<CourierSet> set=courierSet();
	if(!set.isNull()) {
		return set->commsEnabled();
	}
	return false;
}

void Client::registerCouriers(bool reg)
{
	OC_METHODGATE();
	QSharedPointer<CourierSet> set=courierSet();
	if(!set.isNull()) {
		set->enableComms(reg);
	}
}



void Client::updateCourierRegistration()
{
	OC_METHODGATE();
	const bool needs=needsConnection();
	qDebug()<<"update courier reg with needs="<<needs;
	registerCouriers(needs);
}






// NOTE: This has a sister method in Node.cpp around line 388
//       Please remember that while they are similar they are very different!
//       While the other one is a "One agent, one remote" deal
//       this is a "one remote, many agents" deal.
void Client::updateOnlineStatus()
{
	OC_METHODGATE();
	//TODO : Fix
	/*
	//qDebug()<<"START UPDATE ONLINE STATUS # # # # ";
	QSharedPointer<Node> ctl=controller();
	QSharedPointer<Associate> nodeAss=nodeAssociate();
	if(!ctl.isNull() && !nodeAss.isNull()) {
		// Find if we ARE trying to get online
		bool isTryingToGoOnline=false;
		isTryingToGoOnline=ctl->isCommsStarted() && courierRegistration();
		// Find if we WANT to be online
		bool wantToBeOnline=false;
		Settings *s=&ctl->settings();
		if(nullptr!=s) {
			wantToBeOnline=s->getCustomSettingBool("octomy.online."+nodeAss->name(), false);
		} else {
			qWarning()<<"ERROR: No settings for client";
		}
		//Spell it out for debugging
		qDebug()<<"Client '"<<nodeAss->name()<<"' is currently trying to be "<<(isTryingToGoOnline?"ONLINE":"OFFLINE")<<" and wants to try for "<<(wantToBeOnline?"ONLINE":"OFFLINE")<<".";
		// Make necessary changes to state
		const TryToggleState currentTryState=ui->tryToggleListen->state();
		TryToggleState nextTryState=currentTryState;
		bool nextOnlineStatus=isTryingToGoOnline;
		if(wantToBeOnline ) {
			if(isTryingToGoOnline ) {
				nextTryState=ON;
			} else {
				nextTryState=GOING_ON;
				//qDebug()<<"Decided to go online";
				nextOnlineStatus=true;
			}
		} else {
			if(isTryingToGoOnline ) {
				//qDebug()<<"Decided to go offline";
				nextOnlineStatus=false;
			} else {
				nextTryState=OFF;
			}
		}
		if(nextOnlineStatus!=isTryingToGoOnline) {
			ctl->comms()->setHoneymoonEnd(utility::time::currentMsecsSinceEpoch<quint64>()+(1000*60*5));//Set 5 minute honeymoon at every state change
			qDebug()<<"Decided to change online for "<<nodeAss->name();
			setCourierRegistration(nextOnlineStatus);

		} else {
			//qDebug()<<"No change in online status ("<<nextOnlineStatus<<")";
		}
		if(nextTryState!=currentTryState) {
			//qDebug()<<"Decided to change tristate button from "<<currentTryState<<" -> "<<nextTryState;
			ui->tryToggleListen->setState(nextTryState, false);
		} else {
			//qDebug()<<"No change tristate button ("<<nextTryState<<")";
		}
	}
	//qDebug()<<"END UPDATE ONLINE STATUS # # # # ";
	*/
}



//////////////////////////////////////////////////
// Agent State Courier slots

void Client::onSyncParameterChanged(ISyncParameter *sp)
{
	Q_UNUSED(sp);
	//TODO: Refactor so this plays nice with client class
	/*
	OC_METHODGATE();
	qDebug()<<"ClientWidget ASC: ON VALUE CHANGED: "<<sp->toString();
	QSharedPointer<AgentStateCourier> asc=mCouriers.agentStateCourier();
	if(!asc.isNull()) {
		const bool panic=asc->panic();
		ui->widgetPanic->setPanic(panic);
	}
	*/
}

//////////////////////////////////////////////////
// CommsChannel slots


void Client::onCommsError(QString e)
{
	OC_METHODGATE();
	qDebug()<<"Client UNIMP Comms error: "<<e;
}

void Client::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	Q_UNUSED(c);
	//qDebug()<<"ClientWidget UNIMP Client added: "<<c->toString();
}

void Client::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	OC_METHODGATE();
	//qDebug() <<"CLIENT WIDGET New connection status: "<<(s?"ONLINE":"OFFLINE");
	//updateOnlineStatus();

	setNeedsConnection(needsConnection);
	setConnected(isConnected);
}


///////////////////////////////////////// // Internal slots


void Client::onUpdateTimer()
{
	OC_METHODGATE();
	//qDebug()<<"TIME for summary";
}



void Client::appendLog(const QString& text)
{
	OC_METHODGATE();
	// TODO Make sure log ends up in UI
	qDebug()<<"LOG: "<<text;
//	ui->logScroll->appendLog(text);
}
