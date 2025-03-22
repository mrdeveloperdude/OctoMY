#include "NetworkUtilsActivity.hpp"
#include "ui_NetworkUtilsActivity.h"

#include "app/AppContext.hpp"
#include "app/launcher/AppCommandLineParser.hpp"
#include "hexedit/QHexEdit.hpp"
#include "hexedit/QHexEditData.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QHostAddress>
#include <QCommandLineParser>

NetworkUtilsActivity::NetworkUtilsActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::NetworkUtilsActivity)
	, mConfigureHelper("NetworkUtilsActivity", true, false, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


NetworkUtilsActivity::~NetworkUtilsActivity()
{
	OC_METHODGATE();
	delete ui;
}

void NetworkUtilsActivity::configureSend(){
	OC_METHODGATE();
	auto settings = mNode->settings();
	auto cmd = mNode->context()->commandLine();
	
	ui->lineEditRemoteAddress->configure(settings, "localhost","hub-listen_address","The address of the remote host");
	ui->lineEditRemotePort->configure(settings, "","hub-port","The port of the remote host");
	
	ui->lineEditRemotePort->setText(cmd->remotePortOption.valueName());
	qDebug()<<"OVERRIDING REMOTE PORT WITH VALUE FROM CMDLINE: "<<ui->lineEditRemotePort->text();
	
	ui->lineEditRemotePort->setValidator( OC_NEW QIntValidator(0, 65535, this) );
	
	QByteArray ba("HELLO WORLD");
	QHexEditData* hexdata = QHexEditData::fromMemory(ba);
	ui->hexEditor->setData(hexdata);
	
}
void NetworkUtilsActivity::configureConnection(){
	OC_METHODGATE();
	auto _settings = mNode->settings();
	auto settings = _settings.data();
	
	ui->comboBoxLocalAddress->configure(settings, "hub-listen-address", "Local address");
	ui->lineEditBindPort->configure(_settings, "","hub-listen-port","The port to listen for incomming netork traffic");
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SLOT(onListenStateChanged(const TryToggleState, const TryToggleState)))) {
		qWarning() << "ERROR: Could not connect";
	}
	auto cmd = mNode->context()->commandLine();
	ui->lineEditBindPort->setText(cmd->localPortOption.valueName());
	ui->lineEditBindPort->setValidator( OC_NEW QIntValidator(0, 65535, this) );
}


void NetworkUtilsActivity::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mNode = node;
		configureSend();
		configureConnection();
	}
}

void NetworkUtilsActivity::log(const QString &text, LogType type) const{
	OC_METHODGATE();
	if(mNode){
		mNode->log(text, type);
	}
	else{
		qDebug() << type << ": " << text;
	}
}


void NetworkUtilsActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()){
		
	}
}


void NetworkUtilsActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()){
		
	}
}

void NetworkUtilsActivity::onRemoteHostLookupComplete(QHostInfo hi)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		qDebug()<<"## onRemoteHostLookupComplete";
		for(QHostAddress adr:hi.addresses()) {
			if(adr.isNull()) {
				log("Skipping invalid address during remote host lookup: "+adr.toString());
			} else {
				const int l=ui->horizontalSliderSendCount->value();
				const int sz=int(500*ui->horizontalSliderSendSize->value())/100;
				for(int i=0; i<l; ++i) {
					QByteArray ba=("PING "+QString::number(i)+"/"+QString::number(l)).toUtf8()+" ";
					const int sz2=sz-ba.size();
					for(int i=0; i<sz2; ++i) {
						ba.append("X");
					}
					//				quint64 port=ui->lineEditRemotePort->text().toInt();
					//			CommsChannel *comms=hub->getComms();
					//TODO: convert this to use couriers
					
					//	comms->sendPackage(ba,adr,port);
				}
				log( "SENDING "+QString::number(l)+" DATA PACKETS OF SIZE "+QString::number(sz)+" to "+ui->lineEditRemoteAddress->text()+ "("+adr.toString()+"):"+ui->lineEditRemotePort->text());
				return;
			}
		}
		log("No valid addresses found during remote host lookup, aborting send");
	}
}



void NetworkUtilsActivity::onListenStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	Q_UNUSED(last);
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->lineEditBindPort->setEnabled(OFF==current);
		ui->comboBoxLocalAddress->setEnabled(OFF==current);
		// TODO: this should affect HubWindow
		//ui->pushButtonSendData->setEnabled(ON==current);
		const bool on=(ON==current);
		if(!on) {
			// TODO: this should affect HubWindow
			//ui->tabWidget->setCurrentWidget(ui->tabNodes);
		}
		//ui->tabWidget->setEnabled(on);
		// TODO: this should affect HubWindow
		//ui->tabNodes->setListening(on);
		//appendLog("New listening state: "+ToggleStateToSTring(current));
		if(GOING_ON==current) {
			// TODO: this should affect HubWindow
			mNode->localAddressList()->setPort(ui->lineEditBindPort->text().toInt());
			// TODO: Implement needConnection mechanism as in Remote/ClientWindow
			//		mHub->startComms();
			ui->tryToggleListen->setState(ON);
			
		} else if(OFF==current) {
			/*
			comms=mHub->comms();
			if(0!=comms) {
				//		mHub->stopComms();
			}
			*/
		}
	}
}



