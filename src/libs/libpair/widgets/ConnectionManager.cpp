#include "ConnectionManager.hpp"
#include "ui_ConnectionManager.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

ConnectionManager::ConnectionManager(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::ConnectionManager)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

ConnectionManager::~ConnectionManager()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ConnectionManager::on_pushButtonBack_clicked()
{
	OC_METHODGATE();
	emit done();
}

void ConnectionManager::on_pushButtonPairing_clicked()
{
	OC_METHODGATE();
	emit startPairing();
}
