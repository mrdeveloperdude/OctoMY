#include "ControlDeliveryWizard.hpp"
#include "ui_ControlDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "../libutil/utility/Standard.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"
#include "basic/Node.hpp"
#include "random/RNG.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>

const quint64 ControlDeliveryWizard::MINIMUM_BIRTH_TIME=3000;

ControlDeliveryWizard::ControlDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ControlDeliveryWizard)
	, mSpinner(nullptr)
	, mSettings(nullptr)
	, mBirthDate(0)
{
	ui->setupUi(this);
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()){
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	{
		mSpinner=new WaitingSpinnerWidget(ui->labelBirthImage, true, false);
		SpinnerStyle style;
		style.setColor(QColor("white"));
		style.setRelatveSize(true);
		style.setNumberOfLines(24);
		style.setLineLength(10);
		style.setInnerRadius(40);
		style.setLineWidth(3);
		mSpinner->setStyle(style);
	}
	mBirthTimer.setInterval(MINIMUM_BIRTH_TIME); //Minimum birth time gives this moment some depth in case keygen should finish quickly.
	mBirthTimer.setSingleShot(true);

	if(!connect(&mBirthTimer, &QTimer::timeout, this, &ControlDeliveryWizard::onBirthComplete, OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
}




void ControlDeliveryWizard::save(){
}

void ControlDeliveryWizard::reset(){
	ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
}



void ControlDeliveryWizard::configure(Node *n){
	if(mNode!=n){
		mNode=n;
		if(nullptr!=mNode){
			mSettings=&mNode->settings();
			KeyStore &keystore=mNode->keyStore();
			if(!connect(&keystore, &KeyStore::storeReady, this, &ControlDeliveryWizard::onBirthComplete, OC_CONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
			reset();
		}
	}
}

ControlDeliveryWizard::~ControlDeliveryWizard()
{
	delete ui;
}



void ControlDeliveryWizard::onBirthComplete(){
	if(nullptr!=mNode){
		KeyStore &keystore=mNode->keyStore();
		if(keystore.isReady() && !mBirthTimer.isActive()){
			qDebug()<<"XXX - Birth complete!";
			mBirthTimer.stop();
			mSpinner->stop();
			if(keystore.hasError()){
				qWarning()<<"XXX - ERROR: Birthdefects detected!";
				qDebug()<<"XXX: DATA AFTER FAILED LOAD WAS: "<<keystore;
				//Go back to try again
				ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
			}
			else{
				QString id=keystore.localKey().id();
				//qDebug()<<"XXX - All is good, ID: "<<id;
				//qDebug()<<"XXX: DATA AFTER OK LOAD WAS: "<<keystore;
				mBirthDate=QDateTime::currentMSecsSinceEpoch();
				QVariantMap map;
				map["key"]=keystore.localKey().toVariantMap(true);
				map["type"]=DiscoveryTypeToString(TYPE_REMOTE);
				map["role"]=DiscoveryRoleToString(ROLE_CONTROL);
				map["birthDate"]=mBirthDate;
				mMyData= QSharedPointer<NodeAssociate> (new NodeAssociate(map));
				mID=mMyData->toPortableID();
				mNode->peers().setParticipant(mMyData);
				mNode->peers().save();
				ui->widgetBirthCertificate->setPortableID(mID);
				ui->stackedWidget->setCurrentWidget(ui->pageDone);
			}
		}
		else{
			//Debug()<<"XXX - Birth almost complete...";
		}
	}
	else{
		qWarning()<<"ERROR: No node";
	}
}



void ControlDeliveryWizard::on_pushButtonPairNow_clicked()
{
	emit done(true);
}
