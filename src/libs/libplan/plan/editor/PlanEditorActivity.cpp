#include "PlanEditorActivity.hpp"
#include "ui_PlanEditorActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


PlanEditorActivity::PlanEditorActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::PlanEditorActivity)
	, mConfigureHelper("PlanEditorActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PlanEditorActivity::~PlanEditorActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PlanEditorActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure(mNode);
	}
}

void PlanEditorActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug()<<"Plan editor activity pushed with " << arguments;
		if(1 == arguments.count()){
			ui->widgetPlanEditor->loadPlan( arguments[0]);
		}
	}
}


void PlanEditorActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug()<<"pop"<< returnActivity << returnArguments;
	}
}
