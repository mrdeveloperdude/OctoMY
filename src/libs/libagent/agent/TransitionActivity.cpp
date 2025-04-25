#include "TransitionActivity.hpp"
#include "ui_TransitionActivity.h"
#include "uptime/MethodGate.hpp"

TransitionActivity::TransitionActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::TransitionActivity)
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
	qDebug()<<"TRANSITION "<< arguments;
	if(arguments.size()>0){
	}
}
