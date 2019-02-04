#include "LeggedMtlobeWidget.hpp"
#include "ui_LeggedMtlobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

LeggedMtlobeWidget::LeggedMtlobeWidget(QWidget *parent)
	: MtlobeWidget(parent)
	, ui(OC_NEW Ui::LeggedMtlobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

LeggedMtlobeWidget::~LeggedMtlobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void LeggedMtlobeWidget::on_pushButtonDelete_clicked()
{
	OC_METHODGATE();
	emit mtlobeDeleted(0);
}
