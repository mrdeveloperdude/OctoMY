#include "VoiceActivity.hpp"
#include "ui_VoiceActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


VoiceActivity::VoiceActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::VoiceActivity)
	, mConfigureHelper("VoiceActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


VoiceActivity::~VoiceActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void VoiceActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP VOICE");
		ui->widgetSpeechControl->configure(n);
	}
}

void VoiceActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void VoiceActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
