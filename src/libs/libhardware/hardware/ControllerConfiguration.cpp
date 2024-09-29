#include "ControllerConfiguration.hpp"
#include "hardware/controllers/IActuatorController.hpp"
#include "ui_ControllerConfiguration.h"

#include "uptime/MethodGate.hpp"

#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"


class IActuatorController;


ControllerConfiguration::ControllerConfiguration(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControllerConfiguration)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

ControllerConfiguration::~ControllerConfiguration()
{
	OC_METHODGATE();
	delete ui;
}

void ControllerConfiguration::init(){
	OC_METHODGATE();
	QSharedPointer<IActuatorController> ctl;
	if(!mAgent.isNull()) {
		mAgent->reloadController();
		ctl = mAgent->actuatorController();
	}
	QWidget *configurationWidget = nullptr;
	if(!ctl.isNull()) {
		auto widget = ctl->configurationWidget();
		if(nullptr != widget) {
			qDebug()<<"Configuration UI found";
			configurationWidget = widget;
		} else {
			qDebug()<<"Configuration UI not found";
		}
	}
	auto old = ui->scrollAreaControllerConfiguration->takeWidget();
	if(old != configurationWidget) {
		if(nullptr != old) {
			old->setParent(nullptr);
			old = nullptr;
		}
		ui->scrollAreaControllerConfiguration->setWidget(nullptr);
		if(nullptr != configurationWidget) {
			ui->scrollAreaControllerConfiguration->setWidget(configurationWidget);
		} else {
			ui->scrollAreaControllerConfiguration->setWidget(ui->scrollAreaWidgetContents);
		}
		ui->scrollAreaControllerConfiguration->update();
	}
}


void ControllerConfiguration::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
	init();
	reset();
}


void ControllerConfiguration::reset()
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

void ControllerConfiguration::confirmConfiguration() {
	OC_METHODGATE();
	emit done();
}
