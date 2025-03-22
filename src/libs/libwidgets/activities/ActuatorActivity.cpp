#include "ActuatorActivity.hpp"
#include "ui_ActuatorActivity.h"

#include "node/Node.hpp"
#include "pose/PoseMapping.hpp"
#include "uptime/MethodGate.hpp"


ActuatorActivity::ActuatorActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ActuatorActivity)
	, mConfigureHelper("ActuatorActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ActuatorActivity::~ActuatorActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ActuatorActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP ACTUATORS");
		//TODO: WOW we need to update this
		//		ui->widgetActuatorControl->configure(5);
		
		auto pm = QSharedPointer<PoseMapping>::create(5);
		ui->widgetPoseMapping->configure(pm);
	}
}

void ActuatorActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void ActuatorActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}
