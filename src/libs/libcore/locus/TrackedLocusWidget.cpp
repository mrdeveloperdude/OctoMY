#include "TrackedLocusWidget.hpp"
#include "ui_TrackedLocusWidget.h"

TrackedLocusWidget::TrackedLocusWidget(QWidget *parent) :
	LocusWidget(parent),
	ui(new Ui::TrackedLocusWidget)
{
	ui->setupUi(this);
}

TrackedLocusWidget::~TrackedLocusWidget()
{
	delete ui;
}
