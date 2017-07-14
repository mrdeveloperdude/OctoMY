#include "HoveringMtlobeWidget.hpp"
#include "ui_HoveringMtlobeWidget.h"



HoveringMtlobeWidget::HoveringMtlobeWidget(QWidget *parent) :
	MtlobeWidget(parent),
	ui(new Ui::HoveringMtlobeWidget)
{
	ui->setupUi(this);
}

HoveringMtlobeWidget::~HoveringMtlobeWidget()
{
	delete ui;
}
