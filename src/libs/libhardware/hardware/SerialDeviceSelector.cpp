#include "SerialDeviceSelector.hpp"
#include "ui_SerialDeviceSelector.h"

#include "hardware/serial/SerialDeviceListModel.hpp"
#include "agent/Agent.hpp"
#include "agent/AgentConfigStore.hpp"
#include "agent/AgentConfig.hpp"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QListWidgetItem>


SerialDeviceSelector::SerialDeviceSelector(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::SerialDeviceSelector)
	, mSerialDevicesModel(OC_NEW SerialDeviceListModel(this))
{
	ui->setupUi(this);
	ui->listViewSerialDevice->setModel(mSerialDevicesModel);
}


SerialDeviceSelector::~SerialDeviceSelector()
{
	delete ui;
}


void SerialDeviceSelector::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
	reset();
}


void SerialDeviceSelector::reset()
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


void SerialDeviceSelector::selectController(){
	OC_METHODGATE();
	auto const index = ui->listViewSerialDevice->currentIndex();
	auto const item = mSerialDevicesModel->deviceInfo(index);
	emit done(item.serialNumber());
}
