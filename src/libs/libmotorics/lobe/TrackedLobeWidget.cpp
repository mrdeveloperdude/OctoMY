#include "TrackedLobeWidget.hpp"
#include "ui_TrackedLobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


TrackedLobeWidget::TrackedLobeWidget(QWidget *parent) :
	LobeWidget(parent),
	ui(OC_NEW Ui::TrackedLobeWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

TrackedLobeWidget::~TrackedLobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
