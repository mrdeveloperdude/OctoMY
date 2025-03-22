#include "TriggerManagerActivity.hpp"
#include "ui_TriggerManagerActivity.h"

#include "uptime/MethodGate.hpp"
#include "trigger/executor/TriggerExecutor.hpp"

#include <QJsonDocument>
#include <QJsonObject>


TriggerManagerActivity::TriggerManagerActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::TriggerManagerActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


TriggerManagerActivity::~TriggerManagerActivity()
{
	OC_METHODGATE();
	delete ui;
}

void TriggerManagerActivity::configure(){
	const auto ts = QSharedPointer<TriggerExecutor>::create();
	ts->configure();
	ui->widgetTriggerManager->configure(ts);
}

void TriggerManagerActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void TriggerManagerActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
