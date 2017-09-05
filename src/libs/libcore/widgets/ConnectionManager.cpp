#include "ConnectionManager.hpp"
#include "ui_ConnectionManager.h"

#include "utility/Standard.hpp"

ConnectionManager::ConnectionManager(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::ConnectionManager)
{
	ui->setupUi(this);
}

ConnectionManager::~ConnectionManager()
{
	delete ui;
	ui=nullptr;
}

void ConnectionManager::on_pushButtonBack_clicked()
{
	emit done();
}

void ConnectionManager::on_pushButtonPairing_clicked()
{
	emit startPairing();
}
