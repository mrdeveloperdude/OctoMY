#include "ArduMYControllerWidget.hpp"
#include "ui_ArduMYControllerWidget.h"
#include "../libutil/utility/Standard.hpp"
#include "ArduMYController.hpp"

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
	ui->widgetActuatorControl->configure(5);
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

void ArduMYControllerWidget::on_spinBoxActuatorCount_valueChanged(int num)
{
	ui->widgetActuatorControl->configure(num);
}
