#include "PairingMenuActivity.hpp"
#include "ui_PairingMenuActivity.h"

#include "list/PairingEditButtonDelegate.hpp"
#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>

PairingMenuActivity::PairingMenuActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::PairingMenuActivity)
	, mConfigureHelper("PairingMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PairingMenuActivity::~PairingMenuActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}




void PairingMenuActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
	}
}


void PairingMenuActivity::done(){
	OC_METHODGATE();
	pop();
}


void PairingMenuActivity::networkPairing(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("NetworkPairingActivity");
	}
}


void PairingMenuActivity::cameraPairing(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		swap("CameraPairingActivity");
	}
}


void PairingMenuActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}


void PairingMenuActivity::pushImpl(const QStringList arguments)
{
	Q_UNUSED(arguments);
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}

