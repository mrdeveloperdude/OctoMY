#include "ControllerActivity.hpp"

#include "agent/Agent.hpp"
#include "hardware/controllers/ControllerHandler.hpp"
#include "hardware/controllers/IController.hpp"
#include "ui_ControllerActivity.h"
#include "uptime/MethodGate.hpp"


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
		QSharedPointer<ControllerHandler> handler;
		QSharedPointer<IController> ctl;
		if(!mAgent.isNull()) {
			handler = mAgent->controllerHandler();
			if(!handler.isNull()){
				handler->reloadController();
				ctl = handler->controller();
			}
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
		ui->widgetActuatorManager->configure(handler);
	}
}


void ControllerActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		if(mAgent != agent){
			mAgent = agent;
			updateController();
		}
	}
}


void ControllerActivity::toggleEnableAll() {
	OC_METHODGATE();
	ui->widgetActuatorManager->toggleEnableAll();
}


void ControllerActivity::toggleLimpAll() {
	OC_METHODGATE();
	ui->widgetActuatorManager->toggleLimpAll();
}


void ControllerActivity::done() {
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		auto handler = mAgent->controllerHandler();
		if(!handler.isNull()){
			handler->getControllerConfig();
		}
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
