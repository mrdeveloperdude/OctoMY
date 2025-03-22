#include "GaitActivity.hpp"
#include "ui_GaitActivity.h"

#include "uptime/MethodGate.hpp"
#include <QJsonDocument>
#include <QJsonObject>


GaitActivity::GaitActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::GaitActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


GaitActivity::~GaitActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mGait;
	mGait=nullptr;
}

void GaitActivity::configure(){
	mGait = OC_NEW GaitController<qreal>();
	if(nullptr!=mGait) {
		ui->widgetGait->setGait(*mGait);
	}
	
}

void GaitActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void GaitActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
