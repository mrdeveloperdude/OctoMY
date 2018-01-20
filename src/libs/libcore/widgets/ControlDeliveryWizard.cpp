#include "ControlDeliveryWizard.hpp"
#include "ui_ControlDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "utility/Standard.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"
#include "node/Node.hpp"
#include "rng/RNG.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>

const quint64 ControlDeliveryWizard::MINIMUM_BIRTH_TIME=3000;

ControlDeliveryWizard::ControlDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ControlDeliveryWizard)
	, mSpinner(nullptr)
	, mSettings(nullptr)
	, mBirthDate(0)
{
	OC_METHODGATE();
	ui->setupUi(this);
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()) {
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	{
		mSpinner=OC_NEW WaitingSpinnerWidget(ui->labelBirthImage, true, false);
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

	if(!connect(&mBirthTimer, &QTimer::timeout, this, &ControlDeliveryWizard::onBirthComplete, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

void ControlDeliveryWizard::reset()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
}



void ControlDeliveryWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mNode!=n) {
		mNode=n;
		mSettings=(!mNode.isNull())?&mNode->settings():nullptr;
		reset();
	}
	if(mNode.isNull()) {
		qWarning()<<"WARNING: control delivery configured with null node";
	}
}

ControlDeliveryWizard::~ControlDeliveryWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mSpinner;
	mSpinner=nullptr;
}



void ControlDeliveryWizard::onBirthComplete()
{
	OC_METHODGATE();
	if(nullptr!=mNode) {
		KeyStore &keystore=mNode->keyStore();
		if(keystore.ready() && !mBirthTimer.isActive()) {
			qDebug()<<"XXX - Birth complete!";
			mBirthTimer.stop();
			mSpinner->stop();
			auto key=keystore.localKey();
			if(nullptr!=key) {
				QString id=key->id();
				//qDebug()<<"XXX - All is good, ID: "<<id;
				//qDebug()<<"XXX: DATA AFTER OK LOAD WAS: "<<keystore;
				mBirthDate=QDateTime::currentMSecsSinceEpoch();
				QVariantMap map;
				map["key"]=key->toVariantMap(true);
				map["type"]=nodeTypeToString(TYPE_REMOTE);
				map["role"]=nodeRoleToString(ROLE_CONTROL);
				map["birthDate"]=QDateTime::fromMSecsSinceEpoch(mBirthDate);
				mNodeIdentity= QSharedPointer<Associate> (OC_NEW Associate(map));
				mNode->setNodeIdentity(mNodeIdentity);
				mID=mNodeIdentity->toPortableID();
				ui->widgetBirthCertificate->setPortableID(mID);
				ui->stackedWidget->setCurrentWidget(ui->pageDone);
			} else {
				qWarning()<<"XXX - ERROR: Birthdefects detected!";
				qDebug()<<"XXX: DATA AFTER FAILED LOAD WAS: "<<keystore;
				//Go back to try again
				ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
			}
		} else {
			//Debug()<<"XXX - Birth almost complete...";
		}
	} else {
		qWarning()<<"ERROR: No node";
	}
}



void ControlDeliveryWizard::on_pushButtonPairNow_clicked()
{
	OC_METHODGATE();
	emit done(true);
}
