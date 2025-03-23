#include "ConstructActivity.hpp"
#include "ui_ConstructActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


ConstructActivity::ConstructActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ConstructActivity)
	, mConfigureHelper("ConstructActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ConstructActivity::~ConstructActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ConstructActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP CONSTRUCT");
		//ui->widgetConstruct->
		
	}
}

void ConstructActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void ConstructActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
