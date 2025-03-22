#include "MarkdownActivity.hpp"
#include "ui_MarkdownActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


MarkdownActivity::MarkdownActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::MarkdownActivity)
	, mConfigureHelper("MarkdownActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


MarkdownActivity::~MarkdownActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void MarkdownActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		ui->widgetEditor->configure("hub.md");
	}
}

void MarkdownActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void MarkdownActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
