#include "RateCalculatorWidget.hpp"
#include "ui_RateCalculatorWidget.h"

#include "utility/time/RateCalculator.hpp"
#include "utility/string/String.hpp"

#include "uptime/MethodGate.hpp"

RateCalculatorWidget::RateCalculatorWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RateCalculatorWidget)
	, mConfigureHelper("RateCalculatorWidget", true, false, true, true, false)
{
	ui->setupUi(this);
	OC_METHODGATE();
}


RateCalculatorWidget::~RateCalculatorWidget()
{
	OC_METHODGATE();
	delete ui;
}


void RateCalculatorWidget::configure(const RateCalculator *rc)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mRateCalculator=rc;
		if(nullptr!=mRateCalculator) {
			update();
			ui->labelName->setText(mRateCalculator->mName);
		} else {
			ui->labelName->setText("N/A");
			ui->labelTotalBytes->setText("N/A");
			ui->labelTotalCount->setText("N/A");
		}
		ui->labelRateBytes->setText("N/A");
		ui->labelRateCount->setText("N/A");
	}
}


void RateCalculatorWidget::update()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr!=mRateCalculator) {
			ui->labelTotalBytes->setText(QString("%1 (%2)").arg(mRateCalculator->mBytes).arg(utility::string::humanReadableSize(mRateCalculator->mBytes)));
			ui->labelTotalCount->setText(QString("%1").arg(mRateCalculator->mCount));
		}
	}
}
