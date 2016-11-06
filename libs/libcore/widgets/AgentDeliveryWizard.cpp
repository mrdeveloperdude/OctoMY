#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "../libutil/utility/Standard.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"
#include "basic/Node.hpp"
#include "random/RNG.hpp"

#include "../libcore/audio/OneOffSpeech.hpp"
#include "zoo/ZooConstants.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>
#include <QMessageBox>

const quint64 AgentDeliveryWizard::MINIMUM_BIRTH_TIME=3000;

AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentDeliveryWizard)
	, mSpinner(nullptr)
	, mSettings(nullptr)
	, mBirthDate(0)
{
	ui->setupUi(this);
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()) {
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	//Regex rules from http://stackoverflow.com/questions/38001256/handling-accented-letters-in-qregularexpressions-in-qt5/38001274#38001274
	//             and http://www.regular-expressions.info/unicode.html
	ui->lineEditName->setValidator(new QRegularExpressionValidator(re, this));
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

	if(!connect(&mBirthTimer, &QTimer::timeout, this, &AgentDeliveryWizard::onBirthComplete, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s) {
	ui->pushButtonOnward->setEnabled(s.length()>=minLetters);
	},OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	ui->widgetBirthCertificate->configure(false,true);
}


void AgentDeliveryWizard::reset()
{
	ui->lineEditName->setText(mNameGenerator.generate());
	ui->comboBoxGender->setCurrentIndex(0);
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}

static QString generateRandomGender()
{
	RNG *rng=RNG::sourceFactory("devu");
	QString gender="Genderless";
	if(nullptr!=rng) {
		QStringList alternatives;
		alternatives<<"Male"<<"Female"<<"Genderless";
		gender=alternatives[rng->generateInt32()%alternatives.size()];
		delete rng;
	}
	return gender;
}


void AgentDeliveryWizard::configure(Node *n)
{
	if(mNode!=n) {
		mNode=n;
		if(nullptr!=mNode) {
			mSettings=&mNode->settings();
			KeyStore &keystore=mNode->keyStore();
			if(!connect(&keystore, &KeyStore::keystoreReady, this, &AgentDeliveryWizard::onBirthComplete, OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
			reset();
		}
	}
}

AgentDeliveryWizard::~AgentDeliveryWizard()
{
	delete ui;
}



void AgentDeliveryWizard::onBirthComplete()
{
	if(nullptr!=mNode) {
		KeyStore &keystore=mNode->keyStore();
		if(keystore.isReady() && !mBirthTimer.isActive()) {
			qDebug()<<"XXX - Birth complete!";
			mBirthTimer.stop();
			mSpinner->stop();
			if(keystore.hasError()) {
				qWarning()<<"XXX - ERROR: Birthdefects detected!";
				qDebug()<<"XXX: DATA AFTER AFILED LOAD WAS: "<<keystore;
				//Go back to try again
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
			} else {
				QString id=keystore.localKey().id();
				//qDebug()<<"XXX - All is good, ID: "<<id;
				//qDebug()<<"XXX: DATA AFTER OK LOAD WAS: "<<keystore;
				mBirthDate=QDateTime::currentMSecsSinceEpoch();
				QVariantMap map;
				map["key"]=keystore.localKey().toVariantMap(true);
				map["name"]=ui->lineEditName->text();
				if(0==ui->comboBoxGender->currentIndex()) {
					ui->comboBoxGender->setCurrentText(generateRandomGender());
				}
				map["gender"]=ui->comboBoxGender->currentText();
				map["type"]=DiscoveryTypeToString(TYPE_AGENT);
				map["role"]=DiscoveryRoleToString(ROLE_AGENT);
				map["birthDate"]=mBirthDate;
				mMyData= QSharedPointer<NodeAssociate> (new NodeAssociate(map));
				mID=mMyData->toPortableID();
				mNode->peers().setParticipant(mMyData);
				mNode->peers().save();
				ui->widgetBirthCertificate->setPortableID(mID);
				ui->widgetNetworkSettings->setPort(ZooConstants::OCTOMY_UDP_DEFAULT_PORT_AGENT);
				ui->stackedWidget->setCurrentWidget(ui->pageDone);
				//"+(mID.gender().toLower()==QStringLiteral("male")?QStringLiteral("Mr. "):(mID.gender().toLower()==QStringLiteral("female")?QStringLiteral("Mrs. "):QStringLiteral("")))+
				//QString text="Hello, my name is "+mID.name()+". I am an octomy agent. What is your bidding master?";
				QString text="My name is "+mID.name()+". I am an octomy agent. What is your bidding master?";
				new OneOffSpeech(mID,text);
			}
		} else {
			//qDebug()<<"XXX - Birth almost complete...";
		}
	} else {
		qWarning()<<"ERROR: No node";
	}
}



void AgentDeliveryWizard::on_pushButtonDone_clicked()
{
	emit done(false);
}

void AgentDeliveryWizard::on_pushButtonPairNow_clicked()
{
	bool cont=true;
	if(!ui->widgetNetworkSettings->verify()) {
		if (QMessageBox::Yes != QMessageBox::question(this, "Warning", "Selected port '" +QString::number(ui->widgetNetworkSettings->port())+ "' is not available. Are you sure you want to continue?", QMessageBox::Yes|QMessageBox::No)) {
			cont=false;
		}
	}
	if(cont) {
		QSharedPointer<NodeAssociate> me=mNode->nodeIdentity();
		if(nullptr!=me) {
			NetworkAddress &local=me->localAddress();
			NetworkAddress cur=ui->widgetNetworkSettings->address();
			local.setIP(cur.ip());
			local.setPort(ui->widgetNetworkSettings->address().port());
		}
		emit done(true);
	}
}

void AgentDeliveryWizard::on_pushButtonRandomName_clicked()
{
	ui->lineEditName->setText(mNameGenerator.generate());
}

void AgentDeliveryWizard::on_pushButtonOnward_clicked()
{
	if(nullptr!=mNode) {
		KeyStore &keystore=mNode->keyStore();
		QString name=ui->lineEditName->text();
		name[0]=name[0].toUpper();
		mID.setName(name);
		QString gender=0>ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():generateRandomGender();
		mID.setGender(gender);
		qDebug()<<"XXX - Started birth for NAME: "<<name<<", GENDER: "<<gender;
		mSpinner->start();
		mBirthTimer.start();
		keystore.clear();
		keystore.bootstrap(false,true);
		ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
	}
}

void AgentDeliveryWizard::on_pushButtonRandomGender_clicked()
{
	ui->comboBoxGender->setCurrentIndex((rand()%(ui->comboBoxGender->count()-1))+1);
}

void AgentDeliveryWizard::on_pushButtonDoneCertificate_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageDone);
}
