#include "HUDActivity.hpp"
#include "ui_HUDActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


HUDActivity::HUDActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::HUDActivity)
	, mConfigureHelper("HUDActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


HUDActivity::~HUDActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void HUDActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP HUD");
	}
}

void HUDActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void HUDActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
