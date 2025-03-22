#include "IdentityMenuActivity.hpp"

#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

IdentityMenuActivity::IdentityMenuActivity(QWidget *parent)
	: MenuActivity(parent, "IdentityMenu")
	, mConfigureHelper("IdentityMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
}


IdentityMenuActivity::~IdentityMenuActivity()
{
	OC_METHODGATE();
}


void IdentityMenuActivity::configure(QSharedPointer<Node> n)
{
	Q_UNUSED(n);
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		addButton(tr("Birth Certificate"), ":/icons/delivery/certificate.svg",  tr("Show birth certifictate"), &IdentityMenuActivity::birthCertificate, this);
		addButton(tr("Delivery"),          ":/icons/delivery/delivery.svg",     tr("Deliver new node"), &IdentityMenuActivity::delivery, this);
		addButton(tr("Abort"),             ":/icons/general/kill.svg",          tr("Abort node"), &IdentityMenuActivity::delivery, this);
		addSpacer();
		addButton(tr("Done"),              ":/icons/app/yes.svg",               tr("Go back"), &IdentityMenuActivity::done, this);
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
