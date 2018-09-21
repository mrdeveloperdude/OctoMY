#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "utility/Standard.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"
#include "node/Node.hpp"
#include "rng/RNG.hpp"

#include "audio/OneOffSpeech.hpp"
#include "zoo/ZooConstants.hpp"
#include "utility/Utility.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>
#include <QMessageBox>

const quint64 AgentDeliveryWizard::MINIMUM_BIRTH_TIME=3000;

AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::AgentDeliveryWizard)
	, mSpinner(nullptr)
	, mSettings(nullptr)
	, mNode(nullptr)
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
	//Regex rules from http://stackoverflow.com/questions/38001256/handling-accented-letters-in-qregularexpressions-in-qt5/38001274#38001274
	//             and http://www.regular-expressions.info/unicode.html
	ui->lineEditName->setValidator(OC_NEW QRegularExpressionValidator(re, this));
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
	qDebug()<<"birth timer timed out, calling birth done";
		onBirthComplete(true);
	}, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s) {
	ui->pushButtonOnward->setEnabled(s.length()>=minLetters);
	},OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	ui->widgetBirthCertificate->configure(false,true);
}


AgentDeliveryWizard::~AgentDeliveryWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mSpinner;
	mSpinner=nullptr;
}


void AgentDeliveryWizard::reset()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
	ui->comboBoxGender->setCurrentIndex(0);
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}

static QString generateRandomGender()
{
	OC_FUNCTIONGATE();
	RNG *rng=RNG::sourceFactory("devu");
	QString gender="Genderless";
	if(nullptr!=rng) {
		QStringList alternatives;
		alternatives<<"Male"<<"Female"<<"Genderless";
		gender=alternatives[rng->generateInt32()%alternatives.size()];
		delete rng;
		rng=nullptr;
	}
	return gender;
}


void AgentDeliveryWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mNode!=n) {
		mNode=n;
		mSettings=(!mNode.isNull())?&mNode->settings():nullptr;
		reset();
	}
	if(mNode.isNull()) {
		qWarning()<<"WARNING: agent delivery configured with null node";
	}
}

void AgentDeliveryWizard::startBirth()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		QString name=ui->lineEditName->text();
		name[0]=name[0].toUpper();
		mID.setName(name);
		QString gender=0>ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():generateRandomGender();
		mID.setGender(gender);
		qDebug()<<"XXX - Started birth for NAME: "<<name<<", GENDER: "<<gender;
		mSpinner->start();
		mCompleteCounter=0;
		mCompleteOK=false;
		mBirthTimer.start();
		ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
		QTimer::singleShot(0, Qt::VeryCoarseTimer, [=]() {
			KeyStore &keystore=mNode->keyStore();
			keystore.clear();
			keystore.save();
			qDebug()<<"Synchronizing keystore START";
			keystore.synchronize([this](ASEvent<QVariantMap> &se) {
				const bool ok=se.isSuccessfull();
				qDebug()<<"synchronized keystore, calling birth done";
				onBirthComplete(ok);
			});
			qDebug()<<"Synchronizing keystore END";
		});
	}
}




void AgentDeliveryWizard::onBirthComplete(bool ok)
{
	OC_METHODGATE();
	qDebug()<<"onBirthComplete got "<<ok;
	QMutexLocker timeoutLock(&mTimeoutMutex);
	if(!mNode.isNull()) {
		KeyStore &keystore=mNode->keyStore();
		qDebug()<<"XXX - keystore.ready()="<<keystore.ready()<<", ok="<<ok;
		mCompleteOK|=ok;
		mCompleteCounter++;
		if(mCompleteCounter<2) {
			qDebug()<<"XXX - Birth almost complete...";
		} else {

			qDebug()<<"XXX - Birth complete!";
			mBirthTimer.stop();
			mSpinner->stop();
			const bool dok=keystore.ready();
			if(!dok || !ok ) {
				qWarning()<<"XXX - ERROR: Birthdefects detected: datastore.ready="<<dok<<", ok="<<ok;
				qDebug()<<"XXX: DATA AFTER AFILED LOAD WAS: "<<keystore;
				//Go back to try again
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
			} else {
				mBirthDate=utility::currentMsecsSinceEpoch<quint64>();
				QVariantMap map;
				auto key=keystore.localKey();
				if(!key.isNull()) {
					map["key"]=key->toVariantMap(true);
					map["name"]=ui->lineEditName->text();
					if(0==ui->comboBoxGender->currentIndex()) {
						ui->comboBoxGender->setCurrentText(generateRandomGender());
					}
					map["gender"]=ui->comboBoxGender->currentText();
					map["type"]=nodeTypeToString(TYPE_AGENT);
					map["role"]=nodeRoleToString(ROLE_AGENT);
					map["birthDate"]=utility::msToVariant(mBirthDate);
					mNodeIdentity= QSharedPointer<Associate> (OC_NEW Associate(map));
					mNode->setNodeIdentity(mNodeIdentity);
					mID=mNodeIdentity->toPortableID();
					ui->widgetBirthCertificate->setPortableID(mID);
					ui->stackedWidget->setCurrentWidget(ui->pageDone);
					//"+(mID.gender().toLower()==QStringLiteral("male")?QStringLiteral("Mr. "):(mID.gender().toLower()==QStringLiteral("female")?QStringLiteral("Mrs. "):QStringLiteral("")))+
					QString text="My name is "+mID.name()+". I am an octomy agent. How do you do!";
					OC_NEW OneOffSpeech(mID,text);
				} else {
					qWarning()<<"ERROR: No key";
				}
			}
		}
	} else {
		qWarning()<<"ERROR: No node";
	}
	qDebug()<<"onBirthComplete over with completeCounter="<<mCompleteCounter<<" and page="<<ui->stackedWidget->currentWidget()->objectName();
}


void AgentDeliveryWizard::on_pushButtonPairNow_clicked()
{
	OC_METHODGATE();
	emit done(true);
}

void AgentDeliveryWizard::on_pushButtonRandomName_clicked()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
}

void AgentDeliveryWizard::on_pushButtonOnward_clicked()
{
	OC_METHODGATE();
	startBirth();
}

void AgentDeliveryWizard::on_pushButtonRandomGender_clicked()
{
	OC_METHODGATE();
	ui->comboBoxGender->setCurrentIndex((rand()%(ui->comboBoxGender->count()-1))+1);
}
