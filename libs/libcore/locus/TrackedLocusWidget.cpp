#include "TrackedLocusWidget.hpp"
#include "ui_TrackedLocusWidget.h"

TrackedLocusWidget::TrackedLocusWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TrackedLocusWidget)
{
	ui->setupUi(this);
}

TrackedLocusWidget::~TrackedLocusWidget()
{
	delete ui;
}
