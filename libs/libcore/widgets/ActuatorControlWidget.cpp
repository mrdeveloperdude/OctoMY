#include "ActuatorControlWidget.hpp"
#include "ui_ActuatorControlWidget.h"

#include "../libutil/utility/Standard.hpp"
#include "hw/actuators/ServoInput.hpp"
#include "basic/Settings.hpp"

ActuatorControlWidget::ActuatorControlWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ActuatorControlWidget)
	, settings(nullptr)
{
	ui->setupUi(this);

}

ActuatorControlWidget::~ActuatorControlWidget()
{
	delete ui;
}


void ActuatorControlWidget::configure(quint32 num)
{
	for(quint32 i=0; i<num; ++i) {
		ServoInput *si=new ServoInput();
		if(0!=si) {
			si->configure(settings,i);
			if(!connect(si,SIGNAL(servoMoved(quint32, qreal)),this,SLOT(onServoMoved(quint32, qreal)),OC_CONTYPE)) {
				qWarning()<<"ERROR: could not connect";
			}
			if(!connect(si,SIGNAL(servoKilled(quint32)),this,SLOT(onServoKilled(quint32)),OC_CONTYPE)) {
				qWarning()<<"ERROR: could not connect";
			}
			ui->scrollAreaWidgetContents->layout()->addWidget(si);
		}
	}

	QSpacerItem *vs = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayoutServos->addItem(vs);
}



void ActuatorControlWidget::onServoMoved(quint32 id, qreal val)
{

}

void ActuatorControlWidget::onServoKilled(quint32 id)
{

}
