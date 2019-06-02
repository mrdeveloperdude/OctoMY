#include "TrackedMtlobeWidget.hpp"
#include "ui_TrackedMtlobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


TrackedMtlobeWidget::TrackedMtlobeWidget(QWidget *parent) :
	MtlobeWidget(parent),
	ui(OC_NEW Ui::TrackedMtlobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

TrackedMtlobeWidget::~TrackedMtlobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
