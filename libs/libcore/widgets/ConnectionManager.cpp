#include "ConnectionManager.hpp"
#include "ui_ConnectionManager.h"

ConnectionManager::ConnectionManager(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConnectionManager)
{
	ui->setupUi(this);
}

ConnectionManager::~ConnectionManager()
{
	delete ui;
}

void ConnectionManager::on_pushButtonBack_clicked()
{
	emit done();
}

void ConnectionManager::on_pushButtonPairing_clicked()
{
	emit startPairing();
}
