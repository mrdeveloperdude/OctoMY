#include "HoveringLobeWidget.hpp"
#include "ui_HoveringLobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

HoveringLobeWidget::HoveringLobeWidget(QWidget *parent) :
	LobeWidget(parent),
	ui(OC_NEW Ui::HoveringLobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

HoveringLobeWidget::~HoveringLobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
