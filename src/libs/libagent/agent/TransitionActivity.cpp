#include "TransitionActivity.hpp"
#include "ui_Quitting.h"
#include "uptime/MethodGate.hpp"

TransitionActivity::TransitionActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::Quitting)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


TransitionActivity::~TransitionActivity()
{
	OC_METHODGATE();
	delete ui;
}




void TransitionActivity::popImpl(const QString &returnActivity, const QStringList returnArguments){
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}


void TransitionActivity::pushImpl(const QStringList arguments){
	OC_METHODGATE();
	if(arguments.size()>0){
		
	}
}
