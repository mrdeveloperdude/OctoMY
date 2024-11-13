#include "HardwareActivity.hpp"

#include "agent/Agent.hpp"
#include "hardware/controllers/ControllerHandler.hpp"
#include "hardware/controllers/IController.hpp"
#include "ui_HardwareActivity.h"
#include "uptime/MethodGate.hpp"

class IController;

HardwareActivity::HardwareActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::HardwareActivity)
	, mConfigureHelper("HardwareActivity", true, false, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

HardwareActivity::~HardwareActivity()
{
	OC_METHODGATE();
	delete ui;
}

void HardwareActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	if (mConfigureHelper.configure()) {
		if (mAgent != agent) {
			mAgent = agent;
		}
	}
}


void HardwareActivity::actuators(){
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		push("ControllerActivity");
	}
}

void HardwareActivity::controller(){
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		push("ControllerActivity");
	}
}



void HardwareActivity::done()
{
	OC_METHODGATE();
	pop(QStringList());
}



void HardwareActivity::lobes(){
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		push("ControllerActivity");
	}
}

void HardwareActivity::sensors(){
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		push("ControllerActivity");
	}
}

void HardwareActivity::stanzas(){
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		push("StanzaManagerActivity");
	}
}



void HardwareActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if (mConfigureHelper.isConfiguredAsExpected()) {

	}
}

void HardwareActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
