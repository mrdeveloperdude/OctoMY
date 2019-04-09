#include "PairingListWidget.hpp"
#include "ui_PairingListWidget.h"

PairingListWidget::PairingListWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PairingListWidget)
{
	ui->setupUi(this);
}

PairingListWidget::~PairingListWidget()
{
	delete ui;
}
