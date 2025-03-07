#include "AgentSipAndSeeActivity.hpp"
#include "ui_AgentSipAndSeeActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "voice/VoiceManager.hpp"

AgentSipAndSeeActivity::AgentSipAndSeeActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::AgentSipAndSeeActivity)
	, mConfigureHelper("AgentSipAndSeeActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


AgentSipAndSeeActivity::~AgentSipAndSeeActivity()
{
	OC_METHODGATE();
	delete ui;
}


void AgentSipAndSeeActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			ui->widgetBirthCertificate->configure(this, false, true);
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: agent delivery configured without node";
		}
	}
}


void AgentSipAndSeeActivity::reset()
{
	OC_METHODGATE();
}


void AgentSipAndSeeActivity::done()
{
	OC_METHODGATE();
	pop();
}


void AgentSipAndSeeActivity::announceBirth(const PortableID &id){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		auto text = QString("My name is %1 I am an octomy %2. How do you do!").arg(id.name()).arg(nodeTypeToString(id.type()));
		if(mUseVoice){
			VoiceManager::speak(id, text);
		}
	}
}


void AgentSipAndSeeActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}


void AgentSipAndSeeActivity::pushImpl(const QStringList arguments) {
	OC_METHODGATE();
	Q_UNUSED(arguments);
	
	PortableID id;
	if(!mNode.isNull()){
		auto nodeIdentity = mNode->nodeIdentity();
		if(!nodeIdentity.isNull()){
			id = nodeIdentity->toPortableID();
		}
	}
	ui->widgetBirthCertificate->setPortableID(id);
	announceBirth(id);
}
