#include "CameraPairingActivity.hpp"
#include "ui_CameraPairingActivity.h"
#include "uptime/MethodGate.hpp"

CameraPairingActivity::CameraPairingActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::CameraPairingActivity)
{
	ui->setupUi(this);
}

CameraPairingActivity::~CameraPairingActivity()
{
	delete ui;
}



void CameraPairingActivity::configure(QSharedPointer<Agent> agent)
{
	OC_METHODGATE();
	mAgent = agent;
}
