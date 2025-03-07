#include "IdentityMenuActivity.hpp"
#include "ui_IdentityMenuActivity.h"

#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>

IdentityMenuActivity::IdentityMenuActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::IdentityMenuActivity)
	, mConfigureHelper("IdentityMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
	ui->setupUi(this);
}


IdentityMenuActivity::~IdentityMenuActivity()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}




void IdentityMenuActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
	}
}


void IdentityMenuActivity::done(){
	OC_METHODGATE();
	pop();
}


void IdentityMenuActivity::birthCertificate(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("NetworkIdentityActivity");
	}
}


void IdentityMenuActivity::abortion(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("CameraIdentityActivity");
	}
}


void IdentityMenuActivity::delivery(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("AgentDeliveryActivity");
	}
}

void IdentityMenuActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}


void IdentityMenuActivity::pushImpl(const QStringList arguments)
{
	Q_UNUSED(arguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}

