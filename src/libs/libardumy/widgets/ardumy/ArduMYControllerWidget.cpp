#include "ArduMYControllerWidget.hpp"
#include "ui_ArduMYControllerWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "ArduMYController.hpp"
#include "widgets/NameMappingWidget.hpp"
#include "agent/AgentConstants.hpp"
#include "hardware/serial/SerialSettings.hpp"

#include "widgets/ardumy/arduino/ArduinoPinFilter.hpp"

#include "widgets/ardumy/arduino/ArduinoPinFacilitator.hpp"

#include <QMetaObject>

ArduMYControllerWidget::ArduMYControllerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ArduMYControllerWidget)
	, mController(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);

	// NOTE: Default ArduMY serial settings
	SerialSettings defaults;
	defaults.setIntBaudRate(115200);
	defaults.dataBits=QSerialPort::Data8;

	ui->widgetSerialSettings->configure(false, defaults);

	ui->tryToggleConnect->configure("Connect","Connecting...","Connected", "Disconnecting...", AgentConstants::AGENT_CONNECT_BUTTON_COLOR, AgentConstants::AGENT_DISCONNECT_COLOR);

	if(!connect(ui->widgetSerialSettings, &SerialSettingsWidget::settingsChanged, this, &ArduMYControllerWidget::onSerialSettingsChanged, OC_CONTYPE) ) {
		qWarning()<<"ERROR: Could not connect";
	}


	if(!connect(ui->tryToggleConnect, SIGNAL(stateChanged(const TryToggleState, const TryToggleState)), this, SLOT(onTryConnectChanged(const TryToggleState, const TryToggleState)), OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}

	ui->widgetActuatorManager->configure(nullptr);
	ui->tabWidget->setCurrentWidget(ui->tabStatus);

	/* TODO: Implement the full Arduino pin mappnig utopia thingy with filters, pins, detecting used pins, suggesting free pins etc.
	ArduinoPinFacilitator apf;

	ui->widgetPinThing_1->configure(ArduinoPinFilter(PIN_DIGITAL_INPUT, &apf));
	ui->widgetPinThing_2->configure(ArduinoPinFilter(PIN_DIGITAL_OUTPUT, &apf));
	ui->widgetPinThing_3->configure(ArduinoPinFilter(PIN_ANALOG_INPUT, &apf));
	ui->widgetPinThing_4->configure(ArduinoPinFilter(PIN_PWM_OUTPUT, &apf));

	ui->widgetPinThing_5->configure(ArduinoPinFilter(PIN_DIGITAL_INPUT, &apf));
	ui->widgetPinThing_6->configure(ArduinoPinFilter(PIN_DIGITAL_OUTPUT, &apf));
	*/

	showQuickGuide(false);

}

ArduMYControllerWidget::~ArduMYControllerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ArduMYControllerWidget::configure(ArduMYController *controller)
{
	OC_METHODGATE();
	if(nullptr!=mController ) {
		mController->setConnected(false);
		if(!disconnect(mController, SIGNAL(connectionChanged()), this, SLOT(onConnectionChanged())) ) {
			qWarning()<<"ERROR: could not disconnect";
		}
	}
	mController=controller;
	if(nullptr!=mController ) {
		if(!connect(mController, SIGNAL(connectionChanged()), this, SLOT(onConnectionChanged()),OC_CONTYPE) ) {
			qWarning()<<"ERROR: could not connect";
		}
		setSerialSettings(mController->serialSettings());
		onSerialSettingsChanged();
		reconnectActuatorWidgets();
	}
}


void ArduMYControllerWidget::setSerialSettings(SerialSettings &s)
{
	OC_METHODGATE();
	qDebug()<<"ARDUMY CONTROLLER WIDGET PASSES SERIAL SETTINGS ON TO SERTIAL WSETTINGS WIDGET: "<<serialSettingsToString(s);
	ui->widgetSerialSettings->setSettings(s);
}


void ArduMYControllerWidget::setUILock(bool lock)
{
	OC_METHODGATE();
	const bool enabled=!lock;
	ui->tabWidget->setEnabled(enabled);
}


void ArduMYControllerWidget::reconnectActuatorWidgets()
{
	OC_METHODGATE();
	ui->widgetActuatorManager->configure(mController);
}


void ArduMYControllerWidget::onSerialSettingsChanged()
{
	OC_METHODGATE();
	if(nullptr!= mController) {
		const bool wasConnected=mController->isConnected();
		if(wasConnected) {
			mController->setConnected(false);
		}
		mController->setSerialConfig(ui->widgetSerialSettings->settings());
		if(wasConnected) {
			mController->setConnected(true);
		}
	}
}


void ArduMYControllerWidget::onTryConnectChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
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


void ArduMYControllerWidget::onConnectionChanged()
{
	OC_METHODGATE();
	if(nullptr!=mController) {
		qDebug()<<"CONNECTION WAS :"<<mController->isConnected();
		ui->tryToggleConnect->setState(mController->isConnected()?ON:OFF, true);
	}
}



//////////////////////////////////////
void ArduMYControllerWidget::on_pushButtonConfigureSerial_clicked()

{
	OC_METHODGATE();
	ui->tabWidget->setCurrentWidget(ui->tabSerial);
}

void ArduMYControllerWidget::on_pushButtonConfigureActuators_clicked()
{
	OC_METHODGATE();
	ui->tabWidget->setCurrentWidget(ui->tabActuators);
}


void ArduMYControllerWidget::on_comboBoxAddActuator_currentIndexChanged(int index)
{
	OC_METHODGATE();
	if(nullptr==mController) {
		qWarning()<<"ERROR: No controller";
		return;
	}
	const auto idx=ui->comboBoxAddActuator->currentIndex();
	auto ct=mController->actuatorCount();
	ArduMYActuatorType type=ArduMYActuatorType::TYPE_COUNT;
	ArduMYActuatorValueRepresentation representation =ArduMYActuatorValueRepresentation::VALREP_WORD;

	switch(idx) {

	case(0): {
		// We are already here, no action needed
		return;
	}
	break;
	case(1): {
		type=RC_SERVO;
	}
	break;
	case(2): {
		type=STEP_MOTOR;
	}
	break;
	case(3): {
		type=DC_MOTOR;
	}
	break;
	case(4): {
		type=RELAY;
		representation=VALREP_BIT;
	}
	break;
	default:
		break;
	}
	if(TYPE_COUNT!=type) {
		ArduMYActuator *actuator=mController->addActuator();
		if(nullptr!=actuator) {
			actuator->config.type=type;
			actuator->config.representation=representation;
			// TODO: Should this be set here?
			//actuator->config.nickName
		}

	}
	// Prepare add-widget combobox widget for next use
	ui->comboBoxAddActuator->setCurrentIndex(0);
}

void ArduMYControllerWidget::on_pushButtonSync_clicked()
{
	OC_METHODGATE();
	if(nullptr!=mController) {
		qDebug()<<"ARDUMY SET SYNC DUE";
		ArduMYActuatorSet &set=mController->actuators();
		set.setSyncDue(true);
	}
}

void ArduMYControllerWidget::on_pushButtonShowQuickGuide_toggled(bool checked)
{
	OC_METHODGATE();
	showQuickGuide(checked);
}


void ArduMYControllerWidget::showQuickGuide(bool show)
{
	OC_METHODGATE();
	ui->labelQuickGuide->setVisible(show);
	ui->pushButtonShowQuickGuide->setText(show?"Hide Quick Guide":"Show Quick Guide");
}
