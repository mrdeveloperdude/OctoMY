#include "ArduMYActuatorManagerWidget.hpp"
#include "ui_ArduMYActuatorManagerWidget.h"

#include "uptime/New.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

//#include "hardware/controllers/ardumy/ArduMYActuatorWidget.hpp"
//#include "ardumy_arduino/actuator/ArduMYActuatorSet.hpp"
//#include "hardware/controllers/ardumy/ArduMYController.hpp"
#include "hardware/controllers/IController.hpp"

#include <QSpacerItem>
#include <QDebug>

ArduMYActuatorManagerWidget::ArduMYActuatorManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ArduMYActuatorManagerWidget)
	, mController(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(mDebug){
		qDebug() << "CTOR";
	}
}

ArduMYActuatorManagerWidget::~ArduMYActuatorManagerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	if(mDebug){
		qDebug() << "DTOR";
	}
}

void ArduMYActuatorManagerWidget::updateWidgetCount(quint32 num)
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

void ArduMYActuatorManagerWidget::configure(QSharedPointer<IController> controller)
{
	OC_METHODGATE();
	if(mDebug){
		qDebug() << "configure with controller";
	}
	if(nullptr!=mController) {
		const int num=mWidgets.size();
		for(int i=0; i<num; ++i) {
#ifdef OC_USE_FEATURE_ARDUINO
			ArduMYActuatorWidget *si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				if(!disconnect(si,SIGNAL(actuatorMoved(quint32, qreal)), mController, SLOT(onActuatorWidgetMoved(quint32, qreal)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
				if(!disconnect(si,SIGNAL(actuatorLimped(quint32, bool)),mController, SLOT(onActuatorWidgetLimped(quint32, bool)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
				if(!disconnect(si,SIGNAL(actuatorDeleted(quint32)), mController, SLOT(onActuatorWidgetDeleted(quint32)))) {
					qWarning()<<"ERROR: could not disconnect";
				}
			}
#endif
		}
	}
	mController=controller;
	if(nullptr!=mController) {
#ifdef OC_USE_FEATURE_ARDUINO
		ArduMYActuatorSet &actuators=mController->actuators();
		//ArduMYController
		const int num=actuators.size();
		updateWidgetCount( static_cast<quint32>(num) );

		for(int i=0; i<num; ++i) {
			ArduMYActuatorWidget *si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			if (nullptr != si)  {
				ArduMYActuator &a=actuators[i];
				si->configure(&a, static_cast<quint32>(i));
				if(!connect(si,SIGNAL(actuatorMoved(quint32, qreal)), mController, SLOT(onActuatorWidgetMoved(quint32, qreal)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(si,SIGNAL(actuatorLimped(quint32, bool)), mController, SLOT(onActuatorWidgetLimped(quint32, bool)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(si,SIGNAL(actuatorDeleted(quint32)), mController, SLOT(onActuatorWidgetDeleted(quint32)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
			}
		}
#endif
	}
}
