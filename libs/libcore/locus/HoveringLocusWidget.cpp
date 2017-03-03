#include "HoveringLocusWidget.hpp"
#include "ui_HoveringLocusWidget.h"

HoveringLocusWidget::HoveringLocusWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::HoveringLocusWidget)
{
	ui->setupUi(this);
}

HoveringLocusWidget::~HoveringLocusWidget()
{
	delete ui;
}
