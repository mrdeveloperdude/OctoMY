#include "CameraActivity.hpp"
#include "ui_CameraActivity.h"


#include "uptime/MethodGate.hpp"


CameraActivity::CameraActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::CameraActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


CameraActivity::~CameraActivity()
{
	OC_METHODGATE();
	delete ui;
}

void CameraActivity::configure(){
	OC_METHODGATE();
}

void CameraActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void CameraActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
