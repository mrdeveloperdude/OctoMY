#include "CarrierDebugWidget.hpp"
#include "ui_CarrierDebugWidget.h"

CarrierDebugWidget::CarrierDebugWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CarrierDebugWidget)
{
	ui->setupUi(this);
}

CarrierDebugWidget::~CarrierDebugWidget()
{
	delete ui;
}
