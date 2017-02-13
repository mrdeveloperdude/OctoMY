#include "ActuatorManagerWidget.hpp"
#include "ui_ActuatorManagerWidget.h"

#include "../libutil/utility/Standard.hpp"
#include "hw/actuators/ArduMYActuatorWidget.hpp"
#include "../../libs/arduino/ArduMYActuatorSet.hpp"


#include <QSpacerItem>

ActuatorManagerWidget::ActuatorManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ActuatorManagerWidget)
	, mActuators(nullptr)
{
	ui->setupUi(this);
}

ActuatorManagerWidget::~ActuatorManagerWidget()
{
	delete ui;
}

void ActuatorManagerWidget::updateWidgetCount(quint32 num)
{
	quint32 count=mWidgets.count();
	qDebug()<<"---- COUNT: "<<count<<" NUM: "<<num;
	// Trivial reject, no change
	if(num==count) {
		return;
	} else {
		if(num>count) {
			quint32 end=num;
			num-=count;
			for(quint32 i=0; i<num; ++i) {
				qDebug()<<"ADDING ITeMN "<<i<<" of "<<num<<" to get to "<<end;
				ArduMYActuatorWidget *si=new ArduMYActuatorWidget();
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
				qDebug()<<"REMOVING ITeMN "<<i<<" of "<<count<<" to get to "<<num;
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

void ActuatorManagerWidget::configure(ArduMYActuatorSet *actuators)
{
	mActuators=actuators;
	quint32 num=0;
	if(nullptr!=mActuators) {
		num=mActuators->size();
	}
	updateWidgetCount(num);
	if(nullptr!=mActuators) {
		for(quint32 i=0; i<num; ++i) {
			ArduMYActuatorWidget *si=qobject_cast<ArduMYActuatorWidget *>(mWidgets[i]);
			ArduMYActuator &a=(*mActuators)[i];
			if (nullptr != si)  {
				si->configure(&a);
				/*
				if(!connect(si,SIGNAL(servoMoved(quint32, qreal)),this,SLOT(onServoMoved(quint32, qreal)),OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(si,SIGNAL(servoKilled(quint32)),this,SLOT(onServoLimped(quint32)),OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				*/
			}
		}
	}
}



void ActuatorManagerWidget::onServoMoved(quint32 id, qreal val)
{

}

void ActuatorManagerWidget::onServoLimped(quint32 id)
{

}
