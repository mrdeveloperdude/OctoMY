#include "ControllerStatsWidget.hpp"
#include "hardware/controllers/ControllerHandler.hpp"
#include "ui_ControllerStatsWidget.h"
#include "uptime/MethodGate.hpp"

ControllerStatsWidget::ControllerStatsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ControllerStatsWidget)
{
	ui->setupUi(this);
}


ControllerStatsWidget::~ControllerStatsWidget()
{
	delete ui;
}


void ControllerStatsWidget::configure(QSharedPointer<ControllerHandler> handler)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "configure with controller handler";
	}
	if(mHandler != handler){
		mHandler = handler;
		updateController();
		if(!mHandler.isNull()){
			if(!connect(mHandler.data(), &ControllerHandler::controllerChanged, this, &ControllerStatsWidget::updateController)){
				qWarning() << "Could not connect";
			}
			auto controller = mHandler->controller();
			if(!controller.isNull()){
				if(!connect(controller.data(), &IController::definitionsChanged, this, &ControllerStatsWidget::updateController)){
					qWarning() << "Could not connect";
				}
			}
		}
	}
}


void ControllerStatsWidget::updateController(){
	OC_METHODGATE();
	QString controllerTitle{"N/A"};
	QString controllerIcon{":icons/unknown.svg"};
	QString controllerDescription{"N/A"};
	bool actuatorsSupported{false};
	QString actuatorsAvailable{"N/A"};
	QString actuatorsUsed{"N/A"};
	bool sensorsSupported{false};
	QString sensorsAvailable{"N/A"};
	QString sensorsUsed{"N/A"};
	bool lobesSupported{false};
	QString lobesAvailable{"N/A"};
	QString lobesUsed{"N/A"};
	if(!mHandler.isNull()){
		actuatorsSupported = mHandler->actuatorsSupported();
		sensorsSupported = mHandler->sensorsSupported();
		lobesSupported = mHandler->lobesSupported();
		auto controller = mHandler->controller();
		if(!controller.isNull()){
			if(actuatorsSupported){
				actuatorsAvailable = QString::number(controller->maxActuatorsSupported());
				actuatorsUsed = QString::number(controller->actuatorCount());
			}
			if(sensorsSupported){
				sensorsAvailable = QString::number(controller->maxSensorsSupported());
				sensorsUsed = QString::number(controller->sensorCount());
			}
			if(lobesSupported){
				lobesAvailable = QString::number(0);
				lobesUsed = QString::number(0);
			}
			controllerTitle = controller->controllerTitle();
			controllerDescription = controller->controllerDescription();
			controllerIcon = controller->controllerIcon();
		}
		ui->labelTitle->setText(controllerTitle);
		ui->labelDescription->setText(controllerDescription);
		ui->labelIcon->setPixmap(QPixmap(controllerIcon));
		
		ui->widgetActuatorsEnabled->setLightOn(actuatorsSupported);
		ui->labelActuatorsAvailable->setText(actuatorsAvailable);
		ui->labelActuatorsUsed->setText(actuatorsUsed);
		
		
		ui->widgetSensorsEnabled->setLightOn(sensorsSupported);
		ui->labelSensorsAvailable->setText(sensorsAvailable);
		ui->labelSensorsUsed->setText(sensorsUsed);
		
		ui->widgetLobesEnabled->setLightOn(lobesSupported);
		ui->labelLobesAvailable->setText(lobesAvailable);
		ui->labelLobesUsed->setText(lobesUsed);
	}
}
