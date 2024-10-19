#include "ControlSipAndSeeActivity.hpp"
#include "node/Node.hpp"
#include "ui_ControlSipAndSeeActivity.h"

#include "security/PortableID.hpp"
#include "uptime/MethodGate.hpp"

ControlSipAndSeeActivity::ControlSipAndSeeActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControlSipAndSeeActivity)
	, mConfigureHelper("ControlSipAndSeeActivity", true, false, false, true, false)
{
	ui->setupUi(this);
}

ControlSipAndSeeActivity::~ControlSipAndSeeActivity()
{
	delete ui;
}


void ControlSipAndSeeActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			ui->widgetBirthCertificate->configure(false,true);
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: control delivery configured without node";
		}
	}
}


void ControlSipAndSeeActivity::reset()
{
	OC_METHODGATE();
}


void ControlSipAndSeeActivity::done()
{
	OC_METHODGATE();
	pop();
}


void ControlSipAndSeeActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}


void ControlSipAndSeeActivity::pushImpl(const QStringList arguments) {
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
}
