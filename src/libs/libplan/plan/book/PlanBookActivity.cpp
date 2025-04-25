#include "PlanBookActivity.hpp"
#include "ui_PlanBookActivity.h"

#include "node/Node.hpp"
#include "plan/Plan.hpp"
#include "uptime/MethodGate.hpp"


PlanBookActivity::PlanBookActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::PlanBookActivity)
	, mConfigureHelper("PlanBookActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PlanBookActivity::~PlanBookActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PlanBookActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		//log("SETTING UP PLAN BOOK");
		ui->widgetPlans->configure(mNode);
		connect(ui->widgetPlans, &PlanBookWidget::planSelected, this, &PlanBookActivity::planSelected);
	}
}

void PlanBookActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug()<<"push"<< arguments;
	}
}


void PlanBookActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug()<<"pop"<< returnActivity << returnArguments;
	}
}


void PlanBookActivity::planSelected(QSharedPointer<Plan> plan){
	OC_METHODGATE();
	if(plan){
		push("PlanEditorActivity", QStringList() << plan->id());
	}
	else{
		qWarning() << "No plan selected";
	}
}
