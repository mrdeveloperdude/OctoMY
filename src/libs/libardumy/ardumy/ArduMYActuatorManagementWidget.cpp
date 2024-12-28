#include "ArduMYActuatorManagementWidget.hpp"
#include "ardumy/ArduMYController.hpp"
#include "ui_ArduMYActuatorManagementWidget.h"

#include "uptime/New.hpp"
#include "uptime/MethodGate.hpp"

#ifdef OC_USE_FEATURE_ARDUINO

#include "ardumy/ArduMYActuatorWidget.hpp"
#include "ardumy_arduino/actuator/ArduMYActuatorSet.hpp"

#endif

#include "hardware/controllers/IController.hpp"

#include <QSpacerItem>
#include <QDebug>

ArduMYActuatorManagementWidget::ArduMYActuatorManagementWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ArduMYActuatorManagementWidget)
	, mController(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(mDebug){
		qDebug() << "CTOR";
	}
}

ArduMYActuatorManagementWidget::~ArduMYActuatorManagementWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	if(mDebug){
		qDebug() << "DTOR";
	}
}

void ArduMYActuatorManagementWidget::updateWidgetCount(quint32 num)
{
	OC_METHODGATE();
	quint32 count=static_cast<quint32>(mWidgets.count());
	if(mDebug){
		qDebug()<<"---- UPDATING WIDGET COUNT: FROM "<<count<<" TO "<<num;
	}
	// Trivial reject, no change
	if(num==count) {
		return;
	} else {
		if(num>count) {
			quint32 end=num;
			num-=count;
			for(quint32 i=0; i<num; ++i) {
				if(mDebug){
					qDebug()<<"ADDING ARDUMY ACTUATOR "<<(i+1)<<" of "<<num<<" to get to "<<end;
				}
#ifdef OC_USE_FEATURE_ARDUINO
				ArduMYActuatorWidget *si=OC_NEW ArduMYActuatorWidget();
				if(nullptr!=si) {
					mWidgets.push_back(si);
					ui->widgetCompressedContent->layout()->addWidget(si);
					si->setParent(ui->widgetCompressedContent);
					QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
					si->setMinimumSize(0,0);
					si->setSizePolicy(sizePolicy);
					si->updateGeometry();
					si->adjustSize();
				}
#endif
			}
		} else {
			for(quint32 i=count; i>num; --i) {
				if(mDebug){
					qDebug()<<"REMOVING ARDUMY ACTUATOR "<<i<<" of "<<count<<" to get to "<<num;
				}
				QWidget *w=mWidgets.takeFirst();
				ui->widgetCompressedContent->layout()->removeWidget(w);
				w->setParent(nullptr);
				w->deleteLater();
			}
		}
		ui->widgetCompressedContent->updateGeometry();
		ui->widgetCompressedContent->adjustSize();
	}
}

void ArduMYActuatorManagementWidget::configure(QSharedPointer<IController> controller)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "configure with controller";
	}
	if(nullptr!=mController) {
		const int num=mWidgets.size();
		for(int i=0; i<num; ++i) {
#ifdef OC_USE_FEATURE_ARDUINO
			auto si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				if (!disconnect(si, &ArduMYActuatorWidget::actuatorMoved, mController.data(), &IController::setTargetValue)) {
					qWarning() << "ERROR: could not disconnect actuatorMoved";
				}
				/*
				if (!disconnect(si, &ArduMYActuatorWidget::actuatorLimped, mController.data(), &IController::setLimp)) {
					qWarning() << "ERROR: could not disconnect actuatorLimped";
				}
*/
			}
#endif
		}
	}
	mController = qSharedPointerCast<ArduMYController>(controller);
	if(nullptr!=mController) {
#ifdef OC_USE_FEATURE_ARDUINO
		
		
		ArduMYActuatorSet &actuators=mController->actuators();
		//ArduMYController
		const int num=actuators.size();
		updateWidgetCount( static_cast<quint32>(num) );

		for(int i=0; i<num; ++i) {
			auto si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				ArduMYActuator &a=actuators[i];
				si->configure(&a, static_cast<quint32>(i));
				if (!connect(si, &ArduMYActuatorWidget::actuatorMoved, mController.data(), &IController::setTargetValue)) {
					qWarning() << "ERROR: could not connect actuatorMoved";
				}
				/*
				if (!connect(si, &ArduMYActuatorWidget::actuatorLimped, mController.data(), &IController::setLimp)) {
					qWarning() << "ERROR: could not connect actuatorLimped";
				}*/
			}
		}
#endif
	}
}

