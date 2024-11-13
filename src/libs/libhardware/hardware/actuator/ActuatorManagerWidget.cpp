#include "ActuatorManagerWidget.hpp"
#include "hardware/actuator/ActuatorWidget.hpp"
#include "ui_ActuatorManagerWidget.h"

#include "uptime/New.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

//#include "hardware/controllers/ardumy/ArduMYActuatorWidget.hpp"
//#include "ardumy_arduino/actuator/ArduMYActuatorSet.hpp"
//#include "hardware/controllers/ardumy/ArduMYController.hpp"
#include "hardware/controllers/IController.hpp"
#include "hardware/controllers/ControllerHandler.hpp"

#include <QSpacerItem>
#include <QDebug>

ActuatorManagerWidget::ActuatorManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ActuatorManagerWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(mDebug){
		qDebug() << "CTOR";
	}
}

ActuatorManagerWidget::~ActuatorManagerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	if(mDebug){
		qDebug() << "DTOR";
	}
}

void ActuatorManagerWidget::updateWidgetCount(quint32 actualNum)
{
	OC_METHODGATE();
	quint32 existingNum = static_cast<quint32>(mWidgets.count());
	if(mDebug){
		qDebug() << "---- UPDATING WIDGET COUNT: FROM " << existingNum << " TO " << actualNum;
	}
	// Trivial reject, no change
	if(actualNum == existingNum) {
		return;
	} else {
		if(actualNum > existingNum) {
			quint32 end = actualNum;
			actualNum -= existingNum;
			for(quint32 i = 0; i < actualNum; ++i) {
				if(mDebug){
					qDebug() << "ADDING ACTUATOR " << (i + 1) << " of " << actualNum << " to get to " << end;
				}

				auto *actuatorWidget = OC_NEW ActuatorWidget();
				if(nullptr != actuatorWidget) {
					mWidgets.push_back(actuatorWidget);
					ui->widgetCompressedContent->layout()->addWidget(actuatorWidget);
					actuatorWidget->setParent(ui->widgetCompressedContent);
					QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
					actuatorWidget->setMinimumSize(0,0);
					actuatorWidget->setSizePolicy(sizePolicy);
					actuatorWidget->updateGeometry();
					actuatorWidget->adjustSize();
				}

			}
		} else {
			for(quint32 i = existingNum; i > actualNum; --i) {
				if(mDebug){
					qDebug() << "REMOVING ACTUATOR " << i << " of " << existingNum << " to get to " << actualNum;
				}
				QWidget *w = mWidgets.takeFirst();
				ui->widgetCompressedContent->layout()->removeWidget(w);
				w->setParent(nullptr);
				w->deleteLater();
			}
		}
		ui->widgetCompressedContent->updateGeometry();
		ui->widgetCompressedContent->adjustSize();
	}
}

void ActuatorManagerWidget::configure(QSharedPointer<ControllerHandler> handler)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "configure with controller handler";
	}
	if(mHandler != handler){
		if(!mHandler.isNull()) {
			auto controller = mHandler->controller();
			const auto existingNum = mWidgets.size();
			for(int i = 0; i < existingNum; ++i) {
				auto actuatorWidget = qobject_cast<ActuatorWidget *>(mWidgets[i]);
				if(nullptr != actuatorWidget){
					if(mDebug){
						qDebug() << "Removing old actuator widget" << i;
					}
					actuatorWidget->configure(nullptr, i);
					actuatorWidget->deleteLater();
					mWidgets[i] = nullptr;
					if(nullptr != controller){
						if(!disconnect(actuatorWidget, &ActuatorWidget::moved, controller.data(), &IController::setTargetValue)) {
							qWarning()<<"ERROR: could not disconnect";
						}
						if(!disconnect(actuatorWidget, &ActuatorWidget::limpChanged, controller.data(), static_cast<void (IController::*)(ACTUATOR_INDEX, bool)>(&IController::setLimp))) {
							qWarning()<<"ERROR: could not disconnect";
						}
					}
					
				}
			}
		}
		mHandler = handler;
		if(!mHandler.isNull()) {
			auto controller = mHandler->controller();
			if(!controller.isNull()){
				const auto actualNum = controller->actuatorCount();
				updateWidgetCount( static_cast<quint32>(actualNum) );
				for(int i = 0; i < actualNum; ++i) {
					auto actuatorWidget = qobject_cast<ActuatorWidget *>(mWidgets[i]);
					if (nullptr != actuatorWidget)  {
						actuatorWidget->configure(controller, i);
						if(!connect(actuatorWidget, &ActuatorWidget::moved, controller.data(), &IController::setTargetValue, OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(actuatorWidget, &ActuatorWidget::limpChanged, controller.data(), static_cast<void (IController::*)(ACTUATOR_INDEX, bool)>(&IController::setLimp), OC_CONTYPE )) {
							qWarning()<<"ERROR: could not connect";
						}
					}
				}
			}
		}
	}
}


void ActuatorManagerWidget::toggleLimpAll(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "toggleLimpAll";
	}	
	const auto total_num = mHandler->actuatorCount();
	const auto limp_num = mHandler->actuatorCount(true, false, false, false);
	if(limp_num == 0){
		mHandler->toggleLimpAll(true);
	}
	else if(limp_num == total_num){
		mHandler->toggleLimpAll(false);
	}
}


void ActuatorManagerWidget::toggleEnableAll(){
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "toggleEnableAll";
	}
	const auto total_num = mHandler->actuatorCount();
	const auto enabled_num = mHandler->actuatorCount(false, false, true, false);
	if(enabled_num == 0){
		mHandler->toggleEnableAll(true);
	}
	else if(enabled_num == total_num){
		mHandler->toggleEnableAll(false);
	}
}
