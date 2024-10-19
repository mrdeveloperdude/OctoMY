#include "AgentUnboxingWizard.hpp"
#include "hardware/controllers/IController.hpp"
#include "ui_AgentUnboxingWizard.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentWindow.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

AgentUnboxingWizard::AgentUnboxingWizard(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::AgentUnboxingWizard)
	, mConfigureHelper("AgentUnboxingWizard", true, false, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


AgentUnboxingWizard::~AgentUnboxingWizard()
{
	OC_METHODGATE();
	delete ui;
}


bool AgentUnboxingWizard::isDelivered() const{
	OC_METHODGATE();
	auto keystore = mAgent->keyStore();
	if(!keystore.isNull()) {
		auto key = keystore->localKey();
		if(!key.isNull()) {
			return true;
		}
		else {
			qWarning()<<"No local key";
		}
	}
	else {
		qWarning()<<"No keystore";
		
	}
	return false;
}


QString AgentUnboxingWizard::controllerName() const{
	OC_METHODGATE();
	auto config = mAgent->configuration();
	if(!config.isNull()) {
		return config->controllerName();
	}
	return QString();
}


bool AgentUnboxingWizard::isControllerSet() const{
	OC_METHODGATE();
	auto name = controllerName();
	return !name.isEmpty();

}


bool AgentUnboxingWizard::controllerNeedsConfig() const{
	OC_METHODGATE();
	const auto controller = mAgent->controller();
	if(controller){
		return controller->hasConfigurationWidget();
	}
	const auto name = controllerName();
	return (!name.isEmpty()) && name != "none";
}


bool AgentUnboxingWizard::isControllerConfigured() const{
	OC_METHODGATE();
	auto config = mAgent->configuration();
	if(!config.isNull()) {
		//qDebug() << *config;
		auto controllerConfig = config->controllerConfig();
		if(!controllerConfig.isEmpty()){
			return true;
		}
	}
	return false;
}


bool AgentUnboxingWizard::isPaierd() const{
	OC_METHODGATE();
	QVector<QueryRule> f;
	f.append(QueryRule(TYPE_REMOTE, false, true, true));
	f.append(QueryRule(TYPE_HUB, false, true, true));
	auto result = mAgent->addressBook()->filter(f);
	return result.size() > 0;
}


bool AgentUnboxingWizard::controllerIsSerial() const{
	OC_METHODGATE();
	return controllerName() == "serial";
}




UnboxingStage AgentUnboxingWizard::unboxingStage(){
	OC_METHODGATE();
	auto stage = UNKNOWN_STAGE;
	if(mConfigureHelper.isConfiguredAsExpected()){
		auto identity = mAgent->nodeIdentity();
		auto controllerType = controllerName();
		if(!isDelivered()){
			stage = DELIVERY_STAGE;
		}
		else if(!isControllerSet()){
			stage = CONTROLLER_TYPE_STAGE;
		}
		else if(controllerNeedsConfig() && !isControllerConfigured()){
			stage = CONTROLLER_CONFIGURATION_STAGE;
		}
		else if(!isPaierd()){
			stage = PAIRING_STAGE;
		}
		else{
			stage = HANDOVER_STAGE;
		}
	}
	return stage;
}


void AgentUnboxingWizard::updateStage(){
	OC_METHODGATE();
	bool delivered{false};
	bool controllerSet{false};
	bool controllerConfigured{false};
	bool controllerConfigNeeded{false};
	bool paired{false};
	bool allOK{false};
	QString name;
	UnboxingStage stage{UNKNOWN_STAGE};
	PortableID id;
	QString text{"Next"};
	if(mConfigureHelper.isConfiguredAsExpected()){
		delivered = isDelivered();
		if(delivered){
			id = mAgent->nodeIdentity()->toPortableID();
		}
		controllerSet = isControllerSet();
		controllerConfigNeeded = controllerNeedsConfig();
		controllerConfigured = isControllerConfigured();
		paired = isPaierd();
		name = controllerName();
		stage = unboxingStage();
	}
	switch(stage){
		case(DELIVERY_STAGE): text = "Start unboxing";break;
		case(HANDOVER_STAGE): text = "Launch Agent!";break;
		default:break;
	}
	allOK = delivered && controllerSet && controllerConfigured && paired;
	ui->widgetDeliveredIdenticon->setVisible(!id.id().isEmpty());
	ui->widgetDeliveredIdenticon->setPortableID(id, true, true);
	ui->lightWidgetDelivered->setLightOn(delivered);
	ui->labelDelivered->setText(delivered?"Delivered:":"Delivery");
	ui->lightWidgetControllerSet->setLightOn(controllerSet);
	ui->labelControllerName->setText(name);
	ui->labelControllerSet->setText(controllerSet?"Controller:":"Select Controller");
	ui->labelControllerConfigured->setText(controllerConfigured?"Configured":"Configure");
	ui->labelControllerConfigured->setVisible(controllerConfigNeeded);
	ui->lightWidgetControllerConfigured->setVisible(controllerConfigNeeded);
	ui->lightWidgetControllerConfigured->setLightOn(controllerConfigured);
	ui->lightWidgetPaired->setLightOn(paired);
	ui->labelPaired->setText(paired?"Paired":"Pairing");

	ui->lightWidgetAllDone->setLightOn(allOK);
	ui->labelAllDone->setText(allOK?"All Done":"Steps Remain");
	
	ui->pushButtonNextStep->setText(text);
}


void AgentUnboxingWizard::nextStageClicked(){
	OC_METHODGATE();
	const auto stage = unboxingStage();
	qDebug()<<"CLICKED STAGE"<<stage;
	if(!mAgent.isNull()) {
		bool pairing{false};
		switch(stage){
			case DELIVERY_STAGE: push("AgentDeliveryActivity"); break;
			case CONTROLLER_TYPE_STAGE: push("ControllerTypeSelector"); break;
			case CONTROLLER_CONFIGURATION_STAGE: push("ControllerActivity"); break;
			case PAIRING_STAGE: push("PairingActivity"); pairing = true; break;
			case UNBOXING_COMPLETE: push("FaceWidget"); break;
			case HANDOVER_STAGE: push("FaceWidget"); break;
			default:
			case UNKNOWN_STAGE: push("UnboxingWizard"); break;
			}
			mAgent->discoveryActivate(pairing);
	}
}


bool AgentUnboxingWizard::unboxingDone(){
	OC_METHODGATE();
	return unboxingStage() == UNBOXING_COMPLETE;
}


void AgentUnboxingWizard::configure(QSharedPointer<Agent> agent){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mAgent = agent;
		if(!mAgent.isNull()) {
			if(!connect(mAgent.data(), &Node::identityChanged, this, [this]() {
					updateStage();
				},OC_CONTYPE_NON_UNIQUE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
		updateStage();
	}
}


void AgentUnboxingWizard::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void AgentUnboxingWizard::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "agent unboxing pop args" << returnActivity << returnArguments;
	if("ControllerTypeSelector" == returnActivity){
		auto window = qSharedPointerCast<AgentWindow>(mAgent->nodeWindow());
		if(window){
			const auto controllerType = returnArguments[0];
			qDebug()<<"Got controller type " << controllerType;
			window->controllerTypeSelected(controllerType);
			mAgent->reloadController();
		}
		else{
			qWarning()<<"No agent window";
		}
	}
	else if("MessageActivity" == returnActivity){
		if(returnArguments.size() >=2){
			const auto id = returnArguments[0];
			const auto response = returnArguments[1];
			if("quit" == id){
				if("true" == response){
					auto window = qSharedPointerCast<AgentWindow>(mAgent->nodeWindow());
					if(window){
						window->quitApplication();
					}
					else{
						qWarning()<<"No agent window";
					}
				}
			}
		}
	}
	updateStage();
}
