#include "ActuatorManagerWidget.hpp"
#include "ui_ActuatorManagerWidget.h"

#include "utility/Standard.hpp"
#include "hw/controllers/ardumy/ArduMYActuatorWidget.hpp"
#include "ardumy/ArduMYActuatorSet.hpp"
#include "hw/controllers/ardumy/ArduMYController.hpp"


#include <QSpacerItem>

ActuatorManagerWidget::ActuatorManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ActuatorManagerWidget)
	, mController(nullptr)
{
	ui->setupUi(this);
}

ActuatorManagerWidget::~ActuatorManagerWidget()
{
	delete ui;
	ui=nullptr;
}

void ActuatorManagerWidget::updateWidgetCount(quint32 num)
{
	quint32 count=mWidgets.count();
	qDebug()<<"---- UPDATING WIDGET COUNT: FROM "<<count<<" TO "<<num;
	// Trivial reject, no change
	if(num==count) {
		return;
	} else {
		if(num>count) {
			quint32 end=num;
			num-=count;
			for(quint32 i=0; i<num; ++i) {
				qDebug()<<"ADDING ARDUMY ACTUATOR "<<(i+1)<<" of "<<num<<" to get to "<<end;
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
			}
		} else {
			for(quint32 i=count; i>num; --i) {
				qDebug()<<"REMOVING ARDUMY ACTUATOR "<<i<<" of "<<count<<" to get to "<<num;
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

void ActuatorManagerWidget::configure(ArduMYController *controller)
{

	if(nullptr!=mController) {
		if(!disconnect(mController,SIGNAL(actuatorConfigurationChanged()),this,SLOT(controllerSettingsChanged()))) {
			qWarning()<<"ERROR: could not disconnect";
		}
		const quint32 num=mWidgets.size();
		for(quint32 i=0; i<num; ++i) {
			ArduMYActuatorWidget *si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				if(!disconnect(si,SIGNAL(actuatorMoved(quint32, qreal)),mController,SLOT(onActuatorWidgetMoved(quint32, qreal)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
				if(!disconnect(si,SIGNAL(actuatorLimped(quint32, bool)),mController,SLOT(onActuatorWidgetLimped(quint32, bool)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
				if(!disconnect(si,SIGNAL(actuatorDeleted(quint32)),mController,SLOT(onActuatorWidgetDeleted(quint32)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
			}
		}
	}
	mController=controller;
	if(nullptr!=mController) {
		if(!connect(mController,SIGNAL(actuatorConfigurationChanged()),this,SLOT(controllerSettingsChanged()),OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
		ArduMYActuatorSet &actuators=mController->actuators();
		//ArduMYController
		quint32 num=actuators.size();
		updateWidgetCount(num);

		for(quint32 i=0; i<num; ++i) {
			ArduMYActuatorWidget *si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				ArduMYActuator &a=actuators[i];
				si->configure(&a, i);
				if(!connect(si,SIGNAL(actuatorMoved(quint32, qreal)),mController,SLOT(onActuatorWidgetMoved(quint32, qreal)),OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(si,SIGNAL(actuatorLimped(quint32, bool)),mController,SLOT(onActuatorWidgetLimped(quint32, bool)),OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(si,SIGNAL(actuatorDeleted(quint32)),mController,SLOT(onActuatorWidgetDeleted(quint32)),OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
			}
		}

	}
}



void ActuatorManagerWidget::controllerSettingsChanged()
{
	// Reload config the best we can
	configure(mController);
}
