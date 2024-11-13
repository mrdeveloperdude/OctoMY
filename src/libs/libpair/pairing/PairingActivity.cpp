#include "PairingActivity.hpp"
#include "ui_PairingActivity.h"

#include "PairingEditButtonDelegate.hpp"
#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>

PairingActivity::PairingActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::PairingActivity)
	, mConfigureHelper("PairingActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PairingActivity::~PairingActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}




void PairingActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
	}
}


void PairingActivity::done(){
	OC_METHODGATE();
	pop();
}


void PairingActivity::networkPairing(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("NetworkPairingActivity");
	}
}


void PairingActivity::cameraPairing(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("CameraPairingActivity");
	}
}


void PairingActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}


void PairingActivity::pushImpl(const QStringList arguments)
{
	Q_UNUSED(arguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}

