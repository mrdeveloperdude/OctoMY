#include "HoveringMtlobeWidget.hpp"
#include "ui_HoveringMtlobeWidget.h"



HoveringMtlobeWidget::HoveringMtlobeWidget(QWidget *parent) :
	MtlobeWidget(parent),
	ui(OC_NEW Ui::HoveringMtlobeWidget)
{
	ui->setupUi(this);
}

HoveringMtlobeWidget::~HoveringMtlobeWidget()
{
	delete ui;
	ui=nullptr;
}
