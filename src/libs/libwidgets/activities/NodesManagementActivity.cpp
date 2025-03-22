#include "NodesManagementActivity.hpp"
#include "ui_NodesManagementActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


NodesManagementActivity::NodesManagementActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::NodesManagementActivity)
	, mConfigureHelper("NodesManagementActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


NodesManagementActivity::~NodesManagementActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void NodesManagementActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		ui->widgetNodesManagement->configure(s);
	}
}

void NodesManagementActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void NodesManagementActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
