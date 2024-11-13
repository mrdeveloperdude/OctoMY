#include "Servotor32ControllerWidget.hpp"
#include "Servotor32Controller.hpp"
#include "components/NoticeLabel.hpp"
#include "ui_Servotor32ControllerWidget.h"

#include "Servotor32ActuatorWidget.hpp"
#include "app/Constants.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/ui/Ui.hpp"
//#include "agent/AgentConstants.hpp"

#include <QSpacerItem>

#include <QtMath>
#include <qscrollbar.h>

Servotor32ControllerWidget::Servotor32ControllerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::Servotor32ControllerWidget)
	, mController(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleConnect->configure("Connect", "Connecting...", "Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	
	setHookConnectButton(true);
	setHookSerialSettings(true);
	QSpacerItem *vs = OC_NEW QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayoutServos->addItem(vs);
	ui->scrollAreaServos->setEnabled(false);
	ui->tabWidget->setCurrentWidget(ui->tabGeneral);
	utility::ui::pack(ui->tabWidget);
}


Servotor32ControllerWidget::~Servotor32ControllerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void Servotor32ControllerWidget::onConnectChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	Q_UNUSED(last);
	if(nullptr!=mController) {
		ui->scrollAreaServos->setEnabled(ON==current);
		switch(current) {
			case(OFF): {
				if(nullptr!=mController) {
					if(mController->isConnected()){
						mController->toggleLimpAll(true);
					}
					mController->setConnected(false);
				}
				setUILock(false);
			}
			break;
			case(GOING_ON): {
				setUILock(true);
				if(nullptr!=mController) {
					mController->setConnected(true);
				}
			}
			break;
			case(ON): {
				if(nullptr!=mController) {
					if(mController->isConnected()){
						mController->toggleLimpAll(true);
					}
				}
				setUILock(false);
			}
			break;
			default:
			case(GOING_OFF):
				break;
		}
	}
}


void Servotor32ControllerWidget::onSettingsChanged()
{
	OC_METHODGATE();
	if(nullptr != mController) {
		SerialSettings s;
		s.fromMap(mController->configuration());
		ui->tabSerial->setSettings(s);
	}
}


void Servotor32ControllerWidget::onSerialSettingsChanged()
{
	OC_METHODGATE();
	if(nullptr != mController) {
		auto s = mController->configuration();
		auto ss = ui->tabSerial->settings();
		s["serial"] = ss.toMap();
		mController->setConfiguration(s);
	}
}


void Servotor32ControllerWidget::onConnectionChanged()
{
	OC_METHODGATE();
	if(nullptr != mController) {
		const bool e = mController->isConnected();
		ui->tryToggleConnect->setState(e?ON:OFF);
	}
}

void Servotor32ControllerWidget::onErrorOccurred(const QString &error){
	OC_METHODGATE();
	appendNotice(error);
}


void Servotor32ControllerWidget::onServoMoved(quint32 id, qreal val)
{
	OC_METHODGATE();
	qDebug()<<"Registered actuator move"<<id<<val;
	if(id >= Servotor32Controller::SERVO_COUNT) {
		return;
	}
	mPos[id]=(val*2.0)-1.0;
	const quint32 bit=1<<id;
	// TODO: Fix this?
	Q_UNUSED(bit);
	//serial->move(pos,bit);
}


void Servotor32ControllerWidget::onServoLimped(quint32 id)
{
	qDebug()<<"Registered actuator limp"<<id;
	OC_METHODGATE();
	if(nullptr != mController) {
		mController->setLimp(id, true);
	}
}


void Servotor32ControllerWidget::configure(Servotor32Controller *c)
{
	OC_METHODGATE();
	if(c!=mController){
		if(nullptr != mController) {
			mController->setConnected(false);
			setHookController(false);
			auto s = mController->configuration();
			SerialSettings ss;
			ss.fromMap(s["serial"].toMap());
			ui->tabSerial->configure(true, ss);
			for(int i=0;i<32;++i) {
				auto si = OC_NEW Servotor32ActuatorWidget(ui->scrollAreaWidgetContents);
				si->configure(nullptr, i);
			}
		}
		mController = c;
		if(nullptr != mController) {
			setHookController(true);
			setHookActuators(true);
		}
	}
}


void Servotor32ControllerWidget::setHookActuators(bool on){
	OC_METHODGATE();
	if(on){
		auto sis = ui->scrollAreaWidgetContents->findChildren<Servotor32ActuatorWidget *>();
		for(Servotor32ActuatorWidget *si:sis) {
			// TODO: Fix this?
			Q_UNUSED(si);
			//		si->configure(settings,i++);
		}
	}
}

void Servotor32ControllerWidget::setHookController(bool on){
	OC_METHODGATE();
	if(on){
		if(!connect(mController, &Servotor32Controller::settingsChanged, this, &Servotor32ControllerWidget::onSettingsChanged)) {
			qWarning() << "ERROR: could not connect";
		}
		if(!connect(mController, &Servotor32Controller::errorOccurred, this, &Servotor32ControllerWidget::onErrorOccurred)) {
			qWarning() << "ERROR: could not connect";
		}
		
		if(!connect(mController, &Servotor32Controller::connectionChanged, this, &Servotor32ControllerWidget::onConnectionChanged)) {
			qWarning() << "ERROR: could not connect";
		}
	}
	else{
		if(!disconnect(mController, &Servotor32Controller::settingsChanged, this, &Servotor32ControllerWidget::onSettingsChanged)) {
			qWarning() << "ERROR: could not disconnect";
		}
		if(!disconnect(mController, &Servotor32Controller::errorOccurred, this, &Servotor32ControllerWidget::onErrorOccurred)) {
			qWarning() << "ERROR: could not disconnect";
		}
		
		if(!disconnect(mController, &Servotor32Controller::connectionChanged, this, &Servotor32ControllerWidget::onConnectionChanged)) {
			qWarning() << "ERROR: could not disconnect";
		}
	}
}

void Servotor32ControllerWidget::setHookConnectButton(bool wantConnection){
	OC_METHODGATE();
	if(wantConnection){
		if(!connect(ui->tryToggleConnect, &TryToggle::stateChanged, this, &Servotor32ControllerWidget::onConnectChanged)) {
			qWarning() << "ERROR: could not connect";
		}
	}
	else{
		if(!disconnect(ui->tryToggleConnect, &TryToggle::stateChanged, this, &Servotor32ControllerWidget::onConnectChanged)) {
			qWarning() << "ERROR: could not disconnect";
		}
	}
}


void Servotor32ControllerWidget::setHookSerialSettings(bool wantSerial){
	OC_METHODGATE();
	if(wantSerial){
		if(!connect(ui->tabSerial, &SerialSettingsWidget::settingsChanged, this, &Servotor32ControllerWidget::onSerialSettingsChanged)) {
			qWarning() << "ERROR: could not connect";
		}
	}
	else{
		if(!disconnect(ui->tabSerial, &SerialSettingsWidget::settingsChanged, this, &Servotor32ControllerWidget::onSerialSettingsChanged)) {
			qWarning() << "ERROR: could not disconnect";
		}
	}
}


void Servotor32ControllerWidget::appendNotice(const QString &notice)
{
	OC_METHODGATE();
	ui->widgetNoticeList->appendNotice(notice);
}


void Servotor32ControllerWidget::setUILock(bool lock)
{
	OC_METHODGATE();
	const bool enabled=!lock;
	ui->tabWidget->setEnabled(enabled);
	ui->pushButtonLimpAll->setEnabled(enabled);
}


void Servotor32ControllerWidget::limpAll()
{
	OC_METHODGATE();
	if(nullptr!=mController) {
		mController->toggleLimpAll(true);
	}
}

