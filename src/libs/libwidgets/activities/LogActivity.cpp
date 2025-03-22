#include "LogActivity.hpp"
#include "ui_LogActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


LogActivity::LogActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::LogActivity)
	, mConfigureHelper("LogActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


LogActivity::~LogActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void LogActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		ui->logScroll->configure(n);
	}
}

void LogActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void LogActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
