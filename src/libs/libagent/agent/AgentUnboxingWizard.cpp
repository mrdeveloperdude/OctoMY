#include "AgentUnboxingWizard.hpp"
#include "ui_AgentUnboxingWizard.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

AgentUnboxingWizard::AgentUnboxingWizard(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::AgentUnboxingWizard)
	, mConfigureHelper("AgentUnboxingWizard", true, false, true)
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
	auto configStore = mAgent->configurationStore();
	if(!configStore.isNull()) {
		auto config = configStore->agentConfig();
		if(!config.isNull()) {
			return config->controllerName();
		}
	}
	return QString();
}


bool AgentUnboxingWizard::isControllerSet() const{
	auto name = controllerName();
	return !name.isEmpty();

}


bool AgentUnboxingWizard::isControllerConfigured() const{
	auto configStore = mAgent->configurationStore();
	if(!configStore.isNull()) {
		auto config = configStore->agentConfig();
		if(!config.isNull()) {
			auto controllerConfig = config->controllerConfig();
			if(!controllerConfig.isEmpty()){
				return true;
			}
		}
	}
	return false;
}



bool AgentUnboxingWizard::isPaierd() const{
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


bool AgentUnboxingWizard::controllerNeedsConfig() const{
	OC_METHODGATE();
	const auto name = controllerName();
	return (!name.isEmpty()) && name != "none";
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
		else if(!isControllerSet()){
			stage = CONTROLLER_CONFIGURATION_STAGE;
		}
		else if(controllerNeedsConfig() && !isControllerConfigured()){
			if(controllerIsSerial()){
				stage = SERIAL_DEVICE_STAGE;
			}
			else{
				stage = ARDUMY_DEVICE_STAGE;
			}
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
	ui->labelAgentName->setVisible(!id.id().isEmpty());
	ui->labelAgentName->setPortableID(id);
	ui->lightWidgetDelivered->setLightOn(delivered);
	ui->lightWidgetControllerSet->setLightOn(controllerSet);
	ui->labelControllerName->setText(name);
	
	ui->labelControllerConfigured->setVisible(controllerConfigNeeded);
	ui->lightWidgetControllerConfigured->setVisible(controllerConfigNeeded);
	ui->lightWidgetControllerConfigured->setLightOn(controllerConfigured);
	
	ui->lightWidgetPaired->setLightOn(paired);
	
	ui->lightWidgetAllDone->setLightOn(allOK);
	
	ui->pushButtonNextStep->setText(text);
}


void AgentUnboxingWizard::nextStageClicked(){
	const auto stage = unboxingStage();
	qDebug()<<"CLICKED STAGE"<<stage;
	if(HANDOVER_STAGE){
		
	}
	emit nextUnboxingStage(stage);
}


bool AgentUnboxingWizard::unboxingDone(){
	return unboxingStage() == UNBOXING_COMPLETE;
}


void AgentUnboxingWizard::configure(QSharedPointer<Agent> agent){
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
		const auto stage = unboxingStage();
		emit unboxingStageChanged(stage);
	}
}
