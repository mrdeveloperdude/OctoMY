#include "ControlDeliveryActivity.hpp"
#include "ui_ControlDeliveryActivity.h"

#include "app/Settings.hpp"
#include "node/Node.hpp"
#include "random/RNG.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/time/HumanTime.hpp"
#include "components/WaitingSpinnerWidget.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>

const quint64 ControlDeliveryActivity::MINIMUM_BIRTH_TIME=3000;

ControlDeliveryActivity::ControlDeliveryActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::ControlDeliveryActivity)
	, mSpinner(nullptr)
	, mBirthDate(0)
	, mCompleteCounter(0)
	, mCompleteOK(false)
	, mConfigureHelper("ControlDeliveryActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->labelBirthImage->setStyleSheet("background-color: transparent;");
}




void ControlDeliveryActivity::configureSpinner()	{
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




ControlDeliveryActivity::~ControlDeliveryActivity()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
	delete mSpinner;
	mSpinner = nullptr;
}


void ControlDeliveryActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode!=n) {
			mNode=n;
			mBirthControl.configure(mNode);
			if(!connect(&mBirthControl, &BirthControl::birthProgress, this, &ControlDeliveryActivity::onBirthProgress)) {
				qWarning()<<"ERROR: Could not connect onBirthProgress";
			}
			if(!connect(&mBirthControl, &BirthControl::birthComplete, this, &ControlDeliveryActivity::onBirthComplete)) {
				qWarning()<<"ERROR: Could not connect onBirthComplete";
			}
			configureSpinner();
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: control delivery configured with null node";
		}
	}
}


void ControlDeliveryActivity::reset()
{
	OC_METHODGATE();
}


void ControlDeliveryActivity::startBirth(const QString &name)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			qDebug() << "XXX - Started birth of control with NAME: " << name;
			mSpinner->start();
			mBirthControl.birth(name);
		}
	}
	
	
	
	/// MIGRATE TO SIP AND SEE???
	/// 
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// 
	///
	/*
	if(!mNode.isNull()) {
		QString name=ui->lineEditName->text();
		name[0]=name[0].toUpper();
		mID.setName(name);
		qDebug()<<"XXX - Started birth of control with NAME: "<<name;
		mSpinner->start();
		mCompleteCounter=0;
		mCompleteOK=false;
		mBirthTimer.start();
		ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
		QTimer::singleShot(0, Qt::VeryCoarseTimer, [=]() {
			QSharedPointer<KeyStore> keystore=mNode->keyStore();
			if(!keystore.isNull()) {
				keystore->clear();
				keystore->save();
				qDebug()<<"Synchronizing keystore START";
				keystore->synchronize([this](ASEvent<QVariantMap> &se) {
					const bool ok=se.isSuccessfull();
					qDebug()<<"synchronized keystore, calling birth done with OK="<<ok;
					onBirthComplete(ok);
				});
				qDebug()<<"Synchronizing keystore END";
			} else {
				qDebug()<<"ERROR: No keystore";
			}
		});
	}
	*/
}


void ControlDeliveryActivity::onBirthProgress(const QString &step, int index, int total){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "ControlDeliveryActivity:onBirthProgress " << step << index << total;
	}
}


void ControlDeliveryActivity::onBirthComplete(bool ok, const QString &message)
{
	OC_METHODGATE();
	
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "ControlDeliveryActivity:onBirthProgress " << ok << message;
		mSpinner->stop();
		QString id;
		if(!mNode.isNull()){
			auto nodeIdentity = mNode->nodeIdentity();
			if(!nodeIdentity.isNull()){
				id = nodeIdentity->id();
			}
		}
		push("ControlSipAndSeeActivity", QStringList() << id);
	}
	
	
	
	/// MIGRATE TO SIP AND SEE???
	/// 
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// // /// // // / // /
	/// 
	/// 
	/*
	if(!mNode.isNull()) {
		QSharedPointer<KeyStore> keystore=mNode->keyStore();
		if(!keystore.isNull()) {
			mCompleteOK|=ok;
			mCompleteCounter++;
			if(mCompleteCounter<2) {
				qDebug()<<"XXX - Birth almost complete...";
			} else {
				qDebug()<<"XXX - Birth complete!";
				mBirthTimer.stop();
				mSpinner->stop();
				const bool dok=keystore->ready();
				if(!dok || !ok ) {
					qWarning()<<"XXX - ERROR: Birthdefects detected: datastore.ready="<<dok<<", ok="<<ok;
					qDebug()<<"XXX: DATA AFTER AFILED LOAD WAS: "<<keystore;
					//Go back to try again
					ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
				} else {
					mBirthDate=utility::time::currentMsecsSinceEpoch<quint64>();
					QVariantMap map;
					auto key=keystore->localKey();
					if(!key.isNull()) {
						map["key"] = key->toVariantMap(true);
						map["name"] = ui->lineEditName->text();
						map["type"] = nodeTypeToString(TYPE_REMOTE);
						map["role"] = nodeRoleToString(ROLE_CONTROL);
						map["birthDate"] = utility::time::msToVariant(mBirthDate);
						mNodeIdentity = QSharedPointer<Associate>::create(map);
						mNode->setNodeIdentity(mNodeIdentity);
						mID=mNodeIdentity->toPortableID();
						ui->widgetBirthCertificate->setPortableID(mID);
						ui->stackedWidget->setCurrentWidget(ui->pageDone);
					} else {
						qWarning()<<"No key";
					}
				}
			}
		} else {
			qDebug()<<"ERROR: No keystore";
		}
	} else {
		qWarning()<<"ERROR: No node";
	}
	qDebug()<<"onBirthComplete over with completeCounter="<<mCompleteCounter<<" and page="<<ui->stackedWidget->currentWidget()->objectName();
	*/
}




void ControlDeliveryActivity::deliveryDone()
{
	OC_METHODGATE();
	pop(QStringList() << "true");
}




void ControlDeliveryActivity::popImpl(const QString &returnActivity, const QStringList returnArguments) {
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if("ControlBaptismActivity" == returnActivity){
		// After baptism completes, go on to delivery
		if (returnArguments.size() >= 1) {
			QString name = returnArguments.at(0);
			startBirth(name);
		}
		else{
			qWarning()<<"No name at delivery";
			pop();
		}
	}
	else{
		pop();
	}
}


void ControlDeliveryActivity::pushImpl(const QStringList arguments) {
	OC_METHODGATE();
	Q_UNUSED(arguments);
	reset();
	// Start with baptism (selecting name)
	push("ControlBaptismActivity", arguments);
}
