#include "ControllerActivity.hpp"
#include "hardware/controllers/IController.hpp"
#include "ui_ControllerActivity.h"

#include "uptime/MethodGate.hpp"

#include "agent/Agent.hpp"


class IController;

ControllerActivity::ControllerActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControllerActivity)
	, mConfigureHelper("ControllerActivity", true, false, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ControllerActivity::~ControllerActivity()
{
	OC_METHODGATE();
	delete ui;
}


void ControllerActivity::updateController(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		QSharedPointer<IController> ctl;
		if(!mAgent.isNull()) {
			mAgent->reloadController();
			ctl = mAgent->controller();
		}
		QWidget *configurationWidget = nullptr;
		if(!ctl.isNull()) {
			auto widget = ctl->configurationWidget();
			if(nullptr != widget) {
				if(mDebug){
					qDebug()<<"Configuration UI found";
				}
				configurationWidget = widget;
			} else {
				if(mDebug){
					qDebug()<<"Configuration UI not found";
				}
			}
		}
		auto old = ui->scrollAreaControllerActivity->takeWidget();
		if(old != configurationWidget) {
			if(nullptr != old) {
				old->setParent(nullptr);
				old = nullptr;
			}
			ui->scrollAreaControllerActivity->setWidget(nullptr);
			if(nullptr != configurationWidget) {
				if(mDebug){
					qDebug() << "Settings displayed";
				}
				ui->scrollAreaControllerActivity->setWidget(configurationWidget);
			} else {
				if(mDebug){
					qDebug() << "No settings available";
				}
				ui->scrollAreaControllerActivity->setWidget(ui->scrollAreaWidgetContents);
			}
			ui->scrollAreaControllerActivity->update();
		}
		else{
			if(mDebug){
				qDebug() << "No change in settings";
			}
		}
	}
}


void ControllerActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mAgent = agent;
		updateController();
	}
}


void ControllerActivity::confirmConfiguration() {
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		mAgent->getControllerConfig();
	}
	pop(QStringList());
}


void ControllerActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()){
		updateController();
	}
}


void ControllerActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
