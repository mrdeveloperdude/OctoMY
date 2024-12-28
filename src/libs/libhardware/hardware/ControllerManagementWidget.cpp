#include "ControllerManagementWidget.hpp"
#include "ui_ControllerManagementWidget.h"

#include "uptime/MethodGate.hpp"
#include "hardware/controllers/ControllerHandler.hpp"

ControllerManagementWidget::ControllerManagementWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ControllerManagementWidget)
{
	ui->setupUi(this);
}

ControllerManagementWidget::~ControllerManagementWidget()
{
	delete ui;
}

void ControllerManagementWidget::configure(QSharedPointer<ControllerHandler> handler)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "configure with controller handler";
	}
	ui->tabController->configure(handler);
	ui->tabActuators->configure(handler);
	if(mHandler != handler){
		mHandler = handler;
		updateController();
		if(!mHandler.isNull()){
			if(!connect(mHandler.data(), &ControllerHandler::controllerChanged, this, &ControllerManagementWidget::updateController)){
				qWarning() << "Could not connect";
			}
			auto controller = mHandler->controller();
			if(!controller.isNull()){
				if(!connect(controller.data(), &IController::definitionsChanged, this, &ControllerManagementWidget::updateController)){
					qWarning() << "Could not connect";
				}
			}
		}
	}
}


void ControllerManagementWidget::updateController(){
	if(mDebug){
		qDebug() << "update controller";
	}
	if(!mHandler.isNull()){
		const auto actuatorsEnabled = mHandler->actuatorsSupported();
		const auto sensorsEnabled = mHandler->sensorsSupported();
		const auto lobesEnabled = mHandler->lobesSupported();
		ui->tabActuators->setEnabled(actuatorsEnabled);
		ui->tabSensors->setEnabled(sensorsEnabled);
		ui->tabLobes->setEnabled(lobesEnabled);
		ui->tabWidget->setCurrentWidget(ui->tabController);
		ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabActuators), actuatorsEnabled);
		ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabSensors), sensorsEnabled);
		ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabLobes), lobesEnabled);
	}
	
}
