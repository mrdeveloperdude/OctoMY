#include "ArduMYControllerWidget.hpp"
#include "ui_ArduMYControllerWidget.h"
#include "../libutil/utility/Standard.hpp"
#include "ArduMYController.hpp"
#include "widgets/NameMappingWidget.hpp"

#include "widgets/ArduinoPinFilter.hpp"

#include "widgets/ArduinoPinFacilitator.hpp"

#include <QMetaObject>

ArduMYControllerWidget::ArduMYControllerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ArduMYControllerWidget)
	, mController(nullptr)
{
	ui->setupUi(this);
	SerialSettings defaults;
	ui->widgetSerialSettings->configure(true, defaults);
	if(!connect(ui->widgetSerialSettings, &SerialSettingsWidget::settingsChanged, [=]() {
	if(nullptr!= mController) {
			mController->setConnected(false);
			mController->setSerialConfig(ui->widgetSerialSettings->settings());
			mController->setConnected(true);
		}
		ui->tabWidget->setCurrentWidget(ui->tabGeneral);
	})) {
		qWarning()<<"ERROR: Could not connect";
	}
	ui->widgetActuatorManager->configure(nullptr);
	ui->tabWidget->setCurrentIndex(0);

	/*
	ArduinoPinFacilitator apf;

	ui->widgetPinThing_1->configure(ArduinoPinFilter(PIN_DIGITAL_INPUT, &apf));
	ui->widgetPinThing_2->configure(ArduinoPinFilter(PIN_DIGITAL_OUTPUT, &apf));
	ui->widgetPinThing_3->configure(ArduinoPinFilter(PIN_ANALOG_INPUT, &apf));
	ui->widgetPinThing_4->configure(ArduinoPinFilter(PIN_PWM_OUTPUT, &apf));

	ui->widgetPinThing_5->configure(ArduinoPinFilter(PIN_DIGITAL_INPUT, &apf));
	ui->widgetPinThing_6->configure(ArduinoPinFilter(PIN_DIGITAL_OUTPUT, &apf));
	*/


}

ArduMYControllerWidget::~ArduMYControllerWidget()
{
	delete ui;
}

void ArduMYControllerWidget::configure(ArduMYController *controller)
{
	mController=controller;
}

void ArduMYControllerWidget::on_pushButtonConfigureSerial_clicked()
{
	ui->tabWidget->setCurrentWidget(ui->tabSerial);
}

void ArduMYControllerWidget::on_pushButtonConfigureActuators_clicked()
{
	ui->tabWidget->setCurrentWidget(ui->tabActuators);
}


void ArduMYControllerWidget::on_comboBoxAddActuator_currentIndexChanged(int index)
{
	if(nullptr==mController) {
		qWarning()<<"ERROR: No controller";
		return;
	}
	const auto idx=ui->comboBoxAddActuator->currentIndex();
	auto ct=mController->actuatorCount();
	ArduMYActuatorType type=ArduMYActuatorType::TYPE_COUNT;
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
	}
	break;
	default:
		break;
	}
	if(TYPE_COUNT!=type) {

		ArduMYActuator *actuator=mController->addActuator();
		if(nullptr!=actuator) {
			actuator->config.type=type;
		}

	}

	auto nct=mController->actuatorCount();
	if(nct!=ct) {
		ui->widgetActuatorManager->configure(&mController->actuators());
	}
	// Prepare widget for next use
	ui->comboBoxAddActuator->setCurrentIndex(0);

}
