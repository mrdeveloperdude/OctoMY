#include "TestWidget.hpp"

#include "utility/Standard.hpp"

#include "ui_TestWidget.h"

TestWidget::TestWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::TestWidget)
{
	ui->setupUi(this);
}

TestWidget::~TestWidget()
{
	delete ui;
	ui=nullptr;
}
