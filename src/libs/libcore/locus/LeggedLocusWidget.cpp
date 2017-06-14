#include "LeggedLocusWidget.hpp"
#include "ui_LeggedLocusWidget.h"

LeggedLocusWidget::LeggedLocusWidget(QWidget *parent)
	: LocusWidget(parent)
	, ui(new Ui::LeggedLocusWidget)
{
	ui->setupUi(this);
}

LeggedLocusWidget::~LeggedLocusWidget()
{
	delete ui;
}

void LeggedLocusWidget::on_pushButtonDelete_clicked()
{
	emit locusDeleted();
}
