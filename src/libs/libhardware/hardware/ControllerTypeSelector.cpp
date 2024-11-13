#include "ControllerTypeSelector.hpp"
#include "hardware/controllers/ControllerFactory.hpp"
#include "hardware/controllers/IController.hpp"
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
	OC_METHODGATE();
	ui->setupUi(this);
	updateList();
}

ControllerTypeSelector::~ControllerTypeSelector()
{
	OC_METHODGATE();
	delete ui;
}



void ControllerTypeSelector::updateList()
{
	OC_METHODGATE();
	mControllerTypeModel->clear();
	ControllerFactory factory;
	const auto available = factory.availableControllers();
	QString title("None");
	QString description("No controller");
	QString icon(":/icons/unknown.svg");
	for(auto const &type:available){
		auto controller = factory.controllerFactory(type);
		if(controller){
			title = controller->controllerTitle();
			icon = controller->controllerIcon();
			description = controller->controllerDescription();
		}
		mControllerTypeModel->registerControllerType(title, description, icon);
	}
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
	pop(QStringList() << item);
}
