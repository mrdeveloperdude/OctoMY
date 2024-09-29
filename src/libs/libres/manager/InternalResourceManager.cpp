#include "InternalResourceManager.hpp"
#include "ui_InternalResourceManager.h"

InternalResourceManager::InternalResourceManager(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::InternalResourceManager)
{
	ui->setupUi(this);
}

InternalResourceManager::~InternalResourceManager()
{
	delete ui;
}
