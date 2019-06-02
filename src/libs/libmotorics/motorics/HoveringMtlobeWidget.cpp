#include "HoveringMtlobeWidget.hpp"
#include "ui_HoveringMtlobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

HoveringMtlobeWidget::HoveringMtlobeWidget(QWidget *parent) :
	MtlobeWidget(parent),
	ui(OC_NEW Ui::HoveringMtlobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

HoveringMtlobeWidget::~HoveringMtlobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
