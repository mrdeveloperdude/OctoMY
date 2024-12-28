#include "ConnectionActivity.hpp"
#include "service/ServiceLevelManager.hpp"
#include "ui_ConnectionActivity.h"

#include "app/Constants.hpp"
#include "comms/CommsSession.hpp"
#include "node/Node.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

ConnectionActivity::ConnectionActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ConnectionActivity)
	, mConfigureHelper("ConnectionActivity", true, false, true, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleConnect->configure("Connect", "Connecting...", "Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	
	if(!connect(ui->tryToggleConnect, &TryToggle::stateChanged, this, &ConnectionActivity::tryConnectChanged, OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}
}


ConnectionActivity::~ConnectionActivity()
{
	OC_METHODGATE();
	delete ui;
}


void ConnectionActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		mNode ->setHookCommsSignals(*this, true);
	}
}


void ConnectionActivity::tryConnectChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	appendLog("TRY-CONNECT: " + ToggleStateToSTring(last) + " --> " + ToggleStateToSTring(current) );
	switch(current) {
		case(OFF): {
		}
		break;
		case(GOING_ON): {
			if(!mNode.isNull()){
				mNode->serviceLevelManager()->enableLevel("", true);
			}
		}
		break;
		case(ON): {
		}
		break;
		case GOING_OFF:{
		}break;
	}
}


void ConnectionActivity::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}


// An error occurred in comms
void ConnectionActivity::onCommsError(QString message){
	OC_METHODGATE();
	appendLog("CONNECTION-COMMS: " + message);
}

// A new comms session was added
void ConnectionActivity::onCommsClientAdded(CommsSession *c){
	OC_METHODGATE();
	appendLog("CONNECTION-COMMS: session added" + c->address() );
}

// The connection state changed for comms channel
void ConnectionActivity::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection){
	OC_METHODGATE();
	appendLog(QString("CONNECTION-COMMS: connected=%1, wants connection=%2 ").arg(isConnected).arg(needsConnection) );
	//const auto tts = createTryToggleState(isConnected, needsConnection);
	//ui->tryToggleConnect->setState(tts);
}


void ConnectionActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	qDebug() << "connection activity push args" << arguments;
}


void ConnectionActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "connection activity pop args" << returnActivity << returnArguments;
}
