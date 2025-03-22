#include "InternalResourcesActivity.hpp"
#include "ui_InternalResourcesActivity.h"

#include "manager/InternalResourceManager.hpp"

#include "uptime/MethodGate.hpp"


InternalResourcesActivity::InternalResourcesActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::InternalResourcesActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


InternalResourcesActivity::~InternalResourcesActivity()
{
	OC_METHODGATE();
	delete ui;
}

void InternalResourcesActivity::configure(){
	OC_METHODGATE();
}

void InternalResourcesActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void InternalResourcesActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
