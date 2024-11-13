#include "LeggedLobeWidget.hpp"
#include "ui_LeggedLobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

LeggedLobeWidget::LeggedLobeWidget(QWidget *parent)
	: LobeWidget(parent)
	, ui(OC_NEW Ui::LeggedLobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

LeggedLobeWidget::~LeggedLobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void LeggedLobeWidget::on_pushButtonDelete_clicked()
{
	OC_METHODGATE();
	emit LobeDeleted(0);
}
