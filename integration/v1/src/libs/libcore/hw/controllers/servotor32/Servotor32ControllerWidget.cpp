#include "Servotor32ControllerWidget.hpp"
#include "Servotor32Controller.hpp"
#include "ui_Servotor32ControllerWidget.h"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "Servotor32ActuatorWidget.hpp"
#include "agent/AgentConstants.hpp"
#include <QSpacerItem>

#include <QtMath>

Servotor32ControllerWidget::Servotor32ControllerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::Servotor32ControllerWidget)
	, mController(nullptr)
{
	ui->setupUi(this);
	ui->tryToggleConnect->configure("Connect","Connecting...","Connected", "Disconnecting...", AgentConstants::AGENT_CONNECT_BUTTON_COLOR, AgentConstants::AGENT_DISCONNECT_COLOR);

	if(!connect(ui->tryToggleConnect, SIGNAL(stateChanged(const TryToggleState, const TryToggleState)), this, SLOT(onConnectChanged(const TryToggleState, const TryToggleState))),OC_CONTYPE) {
		qWarning()<<"ERROR: could not connect";
	}

	if(!connect(ui->widgetLimbIK, SIGNAL(IKUpadted()), this, SLOT(onLimbIKUpdated())),OC_CONTYPE) {
		qWarning()<<"ERROR: could not connect";
	}


	QSpacerItem *vs = OC_NEW QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayoutServos->addItem(vs);
	ui->scrollAreaServos->setEnabled(false);
	ui->widgetGait->setGait(gait);

	gaitTimer.setTimerType(Qt::PreciseTimer);
	gaitTimer.setInterval(40);
	if(!connect(&gaitTimer,SIGNAL(timeout()),this,SLOT(onUpdateGaitTimer()))) {
		qWarning()<<"ERROR: Could not connect";
	}
	gait.setDirection(0.5,0.5);

	ui->tabWidget->setCurrentWidget(ui->tabGeneral);
	utility::pack(ui->tabWidget);

}

void Servotor32ControllerWidget::onUpdateGaitTimer()
{
	//qDebug()<<"----------------------UPDATE GAIT";
	gait.update();
	const qreal PI2=M_PI*-2;
	const int map[]= {7,6,5,11,10,9,15,14,13,16,17,18,20,21,22,24,25,26};
	int i=0;
	quint32 flags=0;
	for(int leg=0; leg<6; ++leg) {
		const quint32 coxID=map[i++];
		const quint32 femID=map[i++];
		const quint32 tibID=map[i++];
		flags |= (1<<coxID)|(1<<femID)|(1<<tibID);
		qreal cox=0,fem=0,tib=0;
		gait.getLimbVars(leg,cox,fem,tib);
		pos[coxID]=tib/PI2;
		pos[femID]=fem/PI2;
		pos[tibID]=cox/PI2;
	}
	Pose pose;
	pose.setValues(pos, sizeof(pos)/sizeof(qreal));
	if(nullptr!=mController) {
		mController->move(pose);
	}
}

Servotor32ControllerWidget::~Servotor32ControllerWidget()
{
	delete ui;
	ui=nullptr;
}

void Servotor32ControllerWidget::onConnectChanged(const TryToggleState last, const TryToggleState current)
{
	if(nullptr!=mController) {
		ui->scrollAreaServos->setEnabled(ON==current);
		switch(current) {
		case(OFF): {
			mController->limpAll();
			if(nullptr!=mController) {
				mController->setConnected(false);
			}
			setUILock(false);
		}
		break;
		case(GOING_ON): {
			setUILock(true);
			mController->setConnected(true);
		}
		break;
		case(ON): {
			mController->limpAll();
			setUILock(false);
		}
		break;
		}
	}
}

void Servotor32ControllerWidget::onLimbIKUpdated()
{
	/* duplicate of onUpdateGaitTimer?
	const int map[]= {7,6,5,11,10,9,15,14,13,16,17,18,20,21,22,24,25,26};
	quint32 i=0;
	const quint32 coxID=map[i++];
	const quint32 femID=map[i++];
	const quint32 tibID=map[i++];
	quint32 flags = (1<<coxID)|(1<<femID)|(1<<tibID);
	qreal cox=0,fem=0,tib=0;
	ui->widgetLimbIK->getLimbVars(cox,fem,tib);
	pos[coxID]=tib;
	pos[femID]=fem;
	pos[tibID]=cox;
	serial->move(pos,flags);
	*/
}


void Servotor32ControllerWidget::onHexySettingsChanged()
{
	if(nullptr!=mController) {
		const bool e=mController->isConnected();
		ui->tryToggleConnect->setState(e?ON:OFF);
		setUILock(false);
	}
}



void Servotor32ControllerWidget::onHexyConenctionChanged()
{
	if(nullptr!=mController) {
		const bool e=mController->isConnected();
		ui->tryToggleConnect->setState(e?ON:OFF);
		setUILock(false);
	}
}





void Servotor32ControllerWidget::onServoMoved(quint32 id, qreal val)
{
	if(id>= HexySerial::SERVO_COUNT) {
		return;
	}
	pos[id]=(val*2.0)-1.0;
	const quint32 bit=1<<id;
	//serial->move(pos,bit);
}

void Servotor32ControllerWidget::onServoKilled(quint32 id)
{
	if( id>= HexySerial::SERVO_COUNT) {
		return;
	}
	const quint32 bit=1<<id;
	qDebug()<<"KILL "<<id << " (for )=bit "<<bit<<")";
	//serial->kill(bit);
}



void Servotor32ControllerWidget::configure(Servotor32Controller *c)
{
	if(nullptr!=mController) {
		mController->setConnected(false);
		if(!disconnect(mController,SIGNAL(settingsChanged()), this, SLOT(onHexySettingsChanged()))) {
			qWarning()<<"ERROR: could not connect";
		}

		if(!disconnect(mController,SIGNAL(connectionChanged()), this, SLOT(onHexyConenctionChanged()))) {
			qWarning()<<"ERROR: could not connect";
		}
	}
	mController=c;
	if(nullptr!=mController) {
		if(!connect(mController,SIGNAL(settingsChanged()), this, SLOT(onHexySettingsChanged())),OC_CONTYPE) {
			qWarning()<<"ERROR: could not connect";
		}

		if(!connect(mController,SIGNAL(connectionChanged()), this, SLOT(onHexyConenctionChanged())),OC_CONTYPE) {
			qWarning()<<"ERROR: could not connect";
		}
		auto sis=ui->scrollAreaWidgetContents->findChildren<Servotor32ActuatorWidget *>();
		for(Servotor32ActuatorWidget *si:sis) {
			//		si->configure(settings,i++);
		}
	}
}


void Servotor32ControllerWidget::setUILock(bool lock)
{
	const bool enabled=!lock;
	ui->tabWidget->setEnabled(enabled);
	ui->dialFeedrate->setEnabled(enabled);
	ui->pushButtonDisableAll->setEnabled(enabled);
	ui->pushButtonGait->setEnabled(enabled);
}

/*
void Servotor32ControllerWidget::killAll()
{
	//qDebug()<<"KILL ALL";
	if(serial->isConnected()) {
		serial->kill();
		QList<Servotor32ActuatorWidget *> si=ui->scrollAreaWidgetContents->findChildren<Servotor32ActuatorWidget *>();
		for(QList<Servotor32ActuatorWidget*>::iterator it=si.begin(),e=si.end(); it!=e; ++it) {
			Servotor32ActuatorWidget *s=*it;
			if(nullptr != s) {
				QSignalBlocker sb(s);
				s->disableServo();
			}
		}
	}
}
*/
void Servotor32ControllerWidget::on_pushButtonDisableAll_clicked()
{
	if(nullptr!=mController) {
		mController->limpAll();
	}
}

void Servotor32ControllerWidget::on_pushButtonGait_toggled(bool checked)
{
	if(checked) {
		gaitTimer.start();
	} else {
		gaitTimer.stop();
	}
}

void Servotor32ControllerWidget::on_dialFeedrate_valueChanged(int value)
{
	qreal ma=ui->dialFeedrate->maximum();
	qreal mi=ui->dialFeedrate->minimum();
	qreal va=ui->dialFeedrate->value();
	qreal range=(ma-mi)/2;
	qreal feedrate=((va-mi)/range);
	gait.setFeedrate(feedrate);
}
