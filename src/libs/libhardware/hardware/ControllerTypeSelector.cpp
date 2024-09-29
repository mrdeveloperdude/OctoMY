#include "ControllerTypeSelector.hpp"
#include "ui_ControllerTypeSelector.h"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "hardware/models/ControllerTypeModel.hpp"


#include <QListWidgetItem>


ControllerTypeSelector::ControllerTypeSelector(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControllerTypeSelector)
	, mControllerTypeModel(OC_NEW ControllerTypeModel())
{
	ui->setupUi(this);
	initControllerTypeList();
}

ControllerTypeSelector::~ControllerTypeSelector()
{
	delete ui;
}



void ControllerTypeSelector::initControllerTypeList()
{
	OC_METHODGATE();
	mControllerTypeModel->registerControllerType("none", "None", ":/icons/unknown.svg");
	mControllerTypeModel->registerControllerType("ardumy", "ArduMY™ native OctoMY™ <--> Arduino© binding", ":/icons/ardumy.svg");
	mControllerTypeModel->registerControllerType("servotor32", "ArcBotics Servotor32 v2.1", ":/icons/arcbotics_logo.svg");
	ui->listViewControllerType->setModel(mControllerTypeModel);
}




void ControllerTypeSelector::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
	reset();
}


void ControllerTypeSelector::reset()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		auto configStore = mAgent->configurationStore();
		if(!configStore.isNull()) {
			auto config = configStore->agentConfig();
			if(!config.isNull()) {
				auto name = config->controllerName();
				//auto ctlIndex = controllerIndexByName(name);
				//ui->listWidgetController->setCurrentRow(ctlIndex);
			} else {
				qWarning()<<"ERROR: config was null while reseting hardware wizard";
			}
		} else {
			qWarning()<<"ERROR: configStore was null while reseting hardware wizard";
		}
	} else {
		qWarning()<<"ERROR: Agent was null while reseting hardware wizard";
	}
}


void ControllerTypeSelector::selectController(){
	OC_METHODGATE();
	auto const index = ui->listViewControllerType->currentIndex();
	auto const item = mControllerTypeModel->getNickName(index);
	emit done(item);
}
