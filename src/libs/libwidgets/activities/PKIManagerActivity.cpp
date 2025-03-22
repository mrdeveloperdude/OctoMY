#include "PKIManagerActivity.hpp"
#include "ui_PKIManagerActivity.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "uptime/MethodGate.hpp"

PKIManagerActivity::PKIManagerActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::PKIManagerActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PKIManagerActivity::~PKIManagerActivity()
{
	OC_METHODGATE();
	delete ui;
}


void PKIManagerActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void PKIManagerActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
