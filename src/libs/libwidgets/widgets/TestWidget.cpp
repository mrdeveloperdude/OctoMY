#include "TestWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "ui_TestWidget.h"

TestWidget::TestWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::TestWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

TestWidget::~TestWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
