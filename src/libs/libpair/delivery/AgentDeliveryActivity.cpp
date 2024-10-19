#include "AgentDeliveryActivity.hpp"
#include "ui_AgentDeliveryActivity.h"

#include "app/Settings.hpp"
#include "components/WaitingSpinnerWidget.hpp"
#include "node/Node.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


AgentDeliveryActivity::AgentDeliveryActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::AgentDeliveryActivity)
	, mSpinner(nullptr)
	, mBirthDate(0)
	, mCompleteCounter(0)
	, mCompleteOK(false)
	, mConfigureHelper("AgentDeliveryActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->labelBirthImage->setStyleSheet("background-color: transparent;");
}


void AgentDeliveryActivity::configureSpinner()	{
	mSpinner = OC_NEW WaitingSpinnerWidget(ui->labelBirthImage, true, false);
	SpinnerStyle style;
	style.setColor(QColor("white"));
	style.setRelatveSize(true);
	style.setNumberOfLines(24);
	style.setLineLength(10);
	style.setInnerRadius(40);
	style.setLineWidth(3);
	mSpinner->setStyle(style);
}


AgentDeliveryActivity::~AgentDeliveryActivity()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
	delete mSpinner;
	mSpinner = nullptr;
}


void AgentDeliveryActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			mBirthControl.configure(mNode);
			if(!connect(&mBirthControl, &BirthControl::birthProgress, this, &AgentDeliveryActivity::onBirthProgress)) {
				qWarning()<<"ERROR: Could not connect onBirthProgress";
			}
			if(!connect(&mBirthControl, &BirthControl::birthComplete, this, &AgentDeliveryActivity::onBirthComplete)) {
				qWarning()<<"ERROR: Could not connect onBirthComplete";
			}
			configureSpinner();
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: agent delivery configured without node";
		}
	}
}


void AgentDeliveryActivity::reset()
{
	OC_METHODGATE();
}


void AgentDeliveryActivity::startBirth(const QString &name, const QString &gender)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			qDebug() << "XXX - Started birth of agent with NAME: " << name << ", GENDER: " << gender;
			mSpinner->start();
			mBirthControl.birth(name, gender);
		}
	}
}


void AgentDeliveryActivity::onBirthProgress(const QString &step, int index, int total){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "AgentDeliveryActivity:onBirthProgress " << step << index << total;
	}
}


void AgentDeliveryActivity::onBirthComplete(bool ok, const QString &message)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "AgentDeliveryActivity:onBirthProgress " << ok << message;
		mSpinner->stop();
		QString id;
		if(!mNode.isNull()){
			auto nodeIdentity = mNode->nodeIdentity();
			if(!nodeIdentity.isNull()){
				id = nodeIdentity->id();
			}
		}
		push("AgentSipAndSeeActivity", QStringList() << id);
	}
}


void AgentDeliveryActivity::deliveryDone()
{
	OC_METHODGATE();
	pop(QStringList() << "true");
}


void AgentDeliveryActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if("AgentBaptismActivity" == returnActivity){
		// After baptism completes, go on to delivery
		if (returnArguments.size() >= 2) {
			QString name = returnArguments.at(0);
			QString gender = returnArguments.at(1);
			startBirth(name, gender);
		}
		else{
			qWarning()<<"No name and/or gender at delivery";
			pop();
		}
	}
	else{
		pop();
	}
}


void AgentDeliveryActivity::pushImpl(const QStringList arguments) {
	OC_METHODGATE();
	Q_UNUSED(arguments);
	reset();
	// Start with baptism (selecting name and gender)
	push("AgentBaptismActivity", arguments);
}
