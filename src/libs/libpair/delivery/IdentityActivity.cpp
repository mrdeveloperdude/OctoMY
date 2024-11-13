#include "IdentityActivity.hpp"
#include "ui_IdentityActivity.h"

#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>

IdentityActivity::IdentityActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::IdentityActivity)
	, mConfigureHelper("IdentityActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
	ui->setupUi(this);
}


IdentityActivity::~IdentityActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}




void IdentityActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
	}
}


void IdentityActivity::done(){
	OC_METHODGATE();
	pop();
}


void IdentityActivity::birthCertificate(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("NetworkIdentityActivity");
	}
}


void IdentityActivity::abortion(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("CameraIdentityActivity");
	}
}


void IdentityActivity::delivery(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("AgentDeliveryActivity");
	}
}

void IdentityActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}


void IdentityActivity::pushImpl(const QStringList arguments)
{
	Q_UNUSED(arguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}

