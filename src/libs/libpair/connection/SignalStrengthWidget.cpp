#include "SignalStrengthWidget.hpp"
#include "ui_SignalStrengthWidget.h"

SignalStrengthWidget::SignalStrengthWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SignalStrengthWidget)
{
	ui->setupUi(this);
}

SignalStrengthWidget::~SignalStrengthWidget()
{
	delete ui;
}
