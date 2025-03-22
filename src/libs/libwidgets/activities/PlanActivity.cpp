#include "PlanActivity.hpp"
#include "ui_PlanActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


PlanActivity::PlanActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::PlanActivity)
	, mConfigureHelper("PlanActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PlanActivity::~PlanActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PlanActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure("hub.plan");
	}
}

void PlanActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void PlanActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
