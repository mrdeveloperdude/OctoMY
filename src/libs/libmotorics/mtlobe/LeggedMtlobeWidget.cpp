#include "LeggedMtlobeWidget.hpp"
#include "ui_LeggedMtlobeWidget.h"

LeggedMtlobeWidget::LeggedMtlobeWidget(QWidget *parent)
	: MtlobeWidget(parent)
	, ui(new Ui::LeggedMtlobeWidget)
{
	ui->setupUi(this);
}

LeggedMtlobeWidget::~LeggedMtlobeWidget()
{
	delete ui;
	ui=nullptr;
}

void LeggedMtlobeWidget::on_pushButtonDelete_clicked()
{
	emit mtlobeDeleted(0);
}
