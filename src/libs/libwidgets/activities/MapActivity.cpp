#include "MapActivity.hpp"
#include "ui_MapActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


MapActivity::MapActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::MapActivity)
	, mConfigureHelper("MapActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


MapActivity::~MapActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void MapActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//ui->widgetMap->conf
	}
}

void MapActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void MapActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
