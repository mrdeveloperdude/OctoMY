#include "ActuatorWidget.hpp"
#include "ui_ActuatorWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "hardware/controllers/IController.hpp"

#include <QDebug>


ActuatorWidget::ActuatorWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ActuatorWidget)
{
	OC_METHODGATE();
	setObjectName("ActuatorWidget");
	ui->setupUi(this);
	ui->doubleSpinBoxHighTrim->hide();
	ui->doubleSpinBoxLowTrim->hide();
	ui->doubleSpinBoxHighTrim->setValue(mLowTrim);
	ui->doubleSpinBoxLowTrim->setValue(mHighTrim);
	ui->editableLabelName->setTextFormat(Qt::PlainText);
	toggleEnabled(ui->checkBoxEnabled->isChecked());
	ui->stackedWidgetViews->setCurrentWidget(ui->pageTarget);
}


ActuatorWidget::~ActuatorWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void ActuatorWidget::configure(QSharedPointer<IController> controller, ACTUATOR_INDEX index)
{
	OC_METHODGATE();
	if(mController != controller){
		if(!mController.isNull()){
			
		}
		mController = controller;
		if(!mController.isNull()){
			mIndex = index;
			ACTUATOR_VALUE position{0.5};
			if(!mController.isNull()){
				ui->editableLabelName->setText(mController->actuatorName(mIndex));
				position = mController->actuatorTargetValue(mIndex);
				if(!connect(this, &ActuatorWidget::moved, mController.data(), &IController::setTargetValue)){
					qWarning() << "Could not connect move";
				}
				/*
				if(!connect(this, &ActuatorWidget::limpChanged, mController.data(), &IController::setLimp)){
					qWarning() << "Could not connect limp";
				}
*/
			}
			ui->decimalEntryServoPosition->configure(0.0, 1.0, position);
			ui->labelIndex->setText(QString::number(mIndex + 1));
		}
	}
}

bool ActuatorWidget::isEnabled(){
	OC_METHODGATE();
	return ui->checkBoxEnabled->isChecked();
}


void ActuatorWidget::lowTrimUpdated(ACTUATOR_VALUE lowTrim){
	OC_METHODGATE();
	mLowTrim = lowTrim;
}


void ActuatorWidget::highTrimUpdated(ACTUATOR_VALUE highTrim){
	OC_METHODGATE();
	mHighTrim = highTrim;
}


void ActuatorWidget::center()
{
	OC_METHODGATE();
	const auto mi = ui->decimalEntryServoPosition->minimum();
	const auto ma = ui->decimalEntryServoPosition->maximum();
	ui->decimalEntryServoPosition->setValue(mi + (ma - mi) * 0.5);
}


void ActuatorWidget::move()
{
	OC_METHODGATE();
	if(isEnabled()){
		const auto reversed = ui->checkBoxReversed->isChecked();
		auto val = ui->decimalEntryServoPosition->value();
		if(reversed){
			val = 1.0 - val;
		}
		const auto actualPosition = mLowTrim + val * (mHighTrim + 1.0f - mLowTrim);
		emit moved(mIndex, actualPosition);
	}
}


void ActuatorWidget::toggleEnabled(bool enabled)
{
	OC_METHODGATE();
	ui->doubleSpinBoxHighTrim->setVisible(enabled);
	ui->doubleSpinBoxLowTrim->setVisible(enabled);
	ui->decimalEntryServoPosition->setEnabled(enabled);
	ui->checkBoxReversed->setEnabled(enabled);
	ui->pushButtonCenter->setEnabled(enabled);
	ui->pushButtonLimp->setEnabled(enabled);
	ui->pushButtonSetDefault->setEnabled(enabled);
}


void ActuatorWidget::toggleReversed(bool limp)
{
	OC_METHODGATE();
	emit reversedChanged(mIndex, limp);
}


void ActuatorWidget::toggleLimp(bool limp)
{
	OC_METHODGATE();
	emit limpChanged(mIndex, limp);
}


void ActuatorWidget::toggleSettings(bool on){
	OC_METHODGATE();
	ui->stackedWidgetViews->setCurrentWidget(on?ui->pageSettings:ui->pageTarget);
}
