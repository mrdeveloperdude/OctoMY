#include "PairingMenuActivity.hpp"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

PairingMenuActivity::PairingMenuActivity(QWidget *parent)
	: MenuActivity(parent, "PairingMenu")
	, mConfigureHelper("PairingMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
}


PairingMenuActivity::~PairingMenuActivity()
{
	OC_METHODGATE();
}


void PairingMenuActivity::configure(QSharedPointer<Node> n)
{
	Q_UNUSED(n);
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		MenuActivity::configure();
		addButton(tr("Network"),     ":/icons/pairing/lan.svg",  tr("Go to network pairing"), &PairingMenuActivity::cameraPairing, this);
		addButton(tr("Camera"),      ":/icons/pairing/snap.svg", tr("Go to camera pairing"), &PairingMenuActivity::cameraPairing, this);
		addSpacer();
		addButton(tr("Done"),        ":/icons/app/yes.svg",      tr("Go back"), &PairingMenuActivity::done, this);
	}
}


void PairingMenuActivity::done(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		pop();
	}
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

