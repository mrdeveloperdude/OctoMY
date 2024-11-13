#include "DeveloperWidget.hpp"
#include "ui_DeveloperWidget.h"

#include "app/Constants.hpp"
#include "gait/GaitController.hpp"
#include "hub/Hub.hpp"
#include "node/LogDevice.hpp"
#include "pose/PoseMapping.hpp"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/graphics/WidgetCaptureTool.hpp"


#include <QFileDialog>

DeveloperWidget::DeveloperWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DeveloperWidget)
	, mGait(nullptr)
	, mLogDevice{OC_NEW LogDeviceNoop}
	, mNodeFactory("development-")
	, mTopDir("/")
	, mWidgetIllustrationOutputDir(mTopDir)
	, mConfigureHelper("DeveloperWidget", true, false, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

DeveloperWidget::~DeveloperWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mGait;
	mGait=nullptr;
	
}


QSharedPointer<Hub> DeveloperWidget::hub(){
	return mHub;
}

void DeveloperWidget::onListenStateChanged(const TryToggleState last, const TryToggleState current)
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
			hub()->localAddressList()->setPort(ui->lineEditBindPort->text().toInt());
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



void DeveloperWidget::configure(QSharedPointer<Settings> _settings, QSharedPointer<Hub> hub){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mHub = hub;
		auto settings = _settings.data();
		ui->tabWidgetDevelopment->setCurrentWidget(ui->tabComms);
		ui->comboBoxLocalAddress->configure(settings, "hub-listen-address", "Local address");
		ui->lineEditBindPort->configure(settings, "","hub-listen-port","The port to listen for incomming netork traffic");
		ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
		if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SLOT(onListenStateChanged(const TryToggleState, const TryToggleState)),OC_CONTYPE)) {
			qWarning() << "ERROR: Could not connect";
		}
		//TODO: WOW we need to update this
		//		ui->widgetActuatorControl->configure(5);
		auto pm = QSharedPointer<PoseMapping>::create(5);
		ui->widgetPoseMapping->configure(pm);
		auto h = this->hub();
		auto cmd = h->context()->commandLine();
		ui->lineEditBindPort->setText(cmd->localPortOption.valueName());
		if(mDebug){
			qDebug() << "OVERRIDING LOCAL PORT WITH VALUE FROM CMDLINE: " << ui->lineEditBindPort->text();
		}
		ui->lineEditBindPort->setValidator( OC_NEW QIntValidator(0, 65535, this) );
		mGait=OC_NEW GaitController<qreal> ();
		if(nullptr!=mGait) {
			ui->widgetGait->setGait(*mGait);
		}
		ui->tabWidgetIllustration->configure();
		ui->tabIdentity->configure(_settings, hub);
		generateTriggers();
	}
}



void DeveloperWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	if(mLogDevice){
		mLogDevice->appendLog(text);
	}
}



void DeveloperWidget::generateTriggers()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const auto ts = mockTriggers();
		ui->widgetTriggerManager->configure(*ts);
	}
}


void DeveloperWidget::startConstilation(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		const auto agentStatus = mNodeFactory.forkNodeEventloop(NodeType::TYPE_AGENT);
		const auto remoteStatus = mNodeFactory.forkNodeEventloop(NodeType::TYPE_REMOTE);
		if(mDebug){
			qDebug() << "CONSTILLATIONS" << agentStatus << remoteStatus;
		}
	}
}





void DeveloperWidget::updateIdentityWidgets(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->tabIdentity->updateIdentityWidgets();
	}
}
