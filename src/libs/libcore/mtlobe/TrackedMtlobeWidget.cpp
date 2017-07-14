#include "TrackedMtlobeWidget.hpp"
#include "ui_TrackedMtlobeWidget.h"

TrackedMtlobeWidget::TrackedMtlobeWidget(QWidget *parent) :
	MtlobeWidget(parent),
	ui(new Ui::TrackedMtlobeWidget)
{
	ui->setupUi(this);
}

TrackedMtlobeWidget::~TrackedMtlobeWidget()
{
	delete ui;
}
