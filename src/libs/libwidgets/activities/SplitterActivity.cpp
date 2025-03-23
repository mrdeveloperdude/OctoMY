#include "SplitterActivity.hpp"
#include "ui_SplitterActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


SplitterActivity::SplitterActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::SplitterActivity)
	, mConfigureHelper("SplitterActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


SplitterActivity::~SplitterActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void SplitterActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP SPLITTER");
	}
}

void SplitterActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void SplitterActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
