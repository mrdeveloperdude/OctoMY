#include "IdentityActivity.hpp"
#include "ui_IdentityActivity.h"

#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "security/PortableID.hpp"


IdentityActivity::IdentityActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::IdentityActivity)
	, mConfigureHelper("IdentityActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

IdentityActivity::~IdentityActivity()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}



void IdentityActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		// NOTE: activity must be nullptr here or else we will wind into an infinite loop
		ui->widgetMyCertificate->configure(nullptr, true, false);
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
	qDebug() << "PUSH IDENTITY ACTIVITY: " << arguments;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		PortableID pid;
		if(arguments.size() > 0){
			pid.fromPortableString(arguments[0]);
		}
		else{
			if(!mNode.isNull()){
				auto nodeIdentity = mNode->nodeIdentity();
				if(!nodeIdentity.isNull()){
					pid = nodeIdentity->toPortableID();
				}
			}
		}
		qDebug() << "PUSH IDENTITY ACTIVITY ID: " << pid;
		ui->widgetMyCertificate->setPortableID(pid);
	}
}

