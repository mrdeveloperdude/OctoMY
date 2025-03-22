#include "ComputeActivity.hpp"
#include "ui_ComputeActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


ComputeActivity::ComputeActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ComputeActivity)
	, mConfigureHelper("ComputeActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ComputeActivity::~ComputeActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ComputeActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP COMPUTE");
		
	}
}

void ComputeActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void ComputeActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
