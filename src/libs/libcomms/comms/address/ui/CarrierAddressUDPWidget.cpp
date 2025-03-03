#include "CarrierAddressUDPWidget.hpp"
#include "ui_CarrierAddressUDPWidget.h"

CarrierAddressUDPWidget::CarrierAddressUDPWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CarrierAddressUDPWidget)
{
	ui->setupUi(this);
}

CarrierAddressUDPWidget::~CarrierAddressUDPWidget()
{
	delete ui;
}
