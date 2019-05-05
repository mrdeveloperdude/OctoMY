#include "CommsDebugWidget.hpp"
#include "ui_CommsDebugWidget.h"

CommsDebugWidget::CommsDebugWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CommsDebugWidget)
{
	ui->setupUi(this);
}

CommsDebugWidget::~CommsDebugWidget()
{
	delete ui;
}
