#include "WidgetIllustrationActivity.hpp"
#include "ui_WidgetIllustrationActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


WidgetIllustrationActivity::WidgetIllustrationActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::WidgetIllustrationActivity)
	, mConfigureHelper("WidgetIllustrationActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


WidgetIllustrationActivity::~WidgetIllustrationActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void WidgetIllustrationActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP WIDGET ILLUSTRATION");
		ui->widgetIllustration->configure();
	}
}

void WidgetIllustrationActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void WidgetIllustrationActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
