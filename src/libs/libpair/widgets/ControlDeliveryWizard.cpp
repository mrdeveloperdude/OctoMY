#include "ControlDeliveryWizard.hpp"
#include "ui_ControlDeliveryWizard.h"

#include "app/Settings.hpp"

#include "widgets/WaitingSpinnerWidget.hpp"
#include "node/Node.hpp"
#include "random/RNG.hpp"

#include "utility/time/HumanTime.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

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
	, mCompleteCounter(0)
	, mCompleteOK(false)
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

	if(!connect(&mBirthTimer, &QTimer::timeout, this, [=]() {
	qDebug()<<"Birth timer timed out, calling birth done";
		onBirthComplete(true);
	}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
	}


	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s) {
	ui->pushButtonOnward->setEnabled(s.length()>=minLetters);
	}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
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





void ControlDeliveryWizard::reset()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}



void ControlDeliveryWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mNode!=n) {
		mNode=n;
		mSettings=(!mNode.isNull())?mNode->settings():nullptr;
		reset();
	}
	if(mNode.isNull()) {
		qWarning()<<"WARNING: control delivery configured with null node";
	}
}



void ControlDeliveryWizard::startBirth()
{
	OC_METHODGATE();
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
}

void ControlDeliveryWizard::onBirthComplete(bool ok)
{
	OC_METHODGATE();
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
						map["key"]=key->toVariantMap(true);
						map["name"]=ui->lineEditName->text();
						map["type"]=nodeTypeToString(TYPE_REMOTE);
						map["role"]=nodeRoleToString(ROLE_CONTROL);
						map["birthDate"]=utility::time::msToVariant(mBirthDate);
						mNodeIdentity= QSharedPointer<Associate> (OC_NEW Associate(map));
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
}



void ControlDeliveryWizard::on_pushButtonPairNow_clicked()
{
	OC_METHODGATE();
	emit done(true);
}

void ControlDeliveryWizard::on_pushButtonRandomName_clicked()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
}

void ControlDeliveryWizard::on_pushButtonOnward_clicked()
{
	OC_METHODGATE();
	startBirth();
}
