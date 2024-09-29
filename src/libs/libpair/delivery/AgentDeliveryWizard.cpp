#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "app/Settings.hpp"
#include "components/WaitingSpinnerWidget.hpp"
#include "name/GenderGenerator.hpp"
#include "node/Node.hpp"
#include "security/PortableID.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "voice/VoiceManager.hpp"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::AgentDeliveryWizard)
	, mConfigureHelper("AgentDeliveryWizard", true, false, false, true, false)
	, mSpinner(nullptr)
	, mBirthDate(0)
	, mCompleteCounter(0)
	, mCompleteOK(false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	configureValidators();
	configureSpinner();
	ui->widgetBirthCertificate->configure(false,true);
}


void AgentDeliveryWizard::configureSpinner()	{
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


void AgentDeliveryWizard::configureValidators(){
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()) {
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	//Regex rules from http://stackoverflow.com/questions/38001256/handling-accented-letters-in-qregularexpressions-in-qt5/38001274#38001274
	//             and http://www.regular-expressions.info/unicode.html
	ui->lineEditName->setValidator(OC_NEW QRegularExpressionValidator(re, this));
	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [this](QString s) {
			ui->pushButtonStartDelivery->setEnabled(s.length() >= minLetters);
		}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


AgentDeliveryWizard::~AgentDeliveryWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
	delete mSpinner;
	mSpinner=nullptr;
}


void AgentDeliveryWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mNode != n) {
			mNode = n;
			mBirthControl.configure(mNode);
			if(!connect(&mBirthControl, &BirthControl::birthProgress, this, &AgentDeliveryWizard::onBirthProgress), OC_CONTYPE_NON_UNIQUE) {
				qWarning()<<"ERROR: Could not connect";
			}
			if(!connect(&mBirthControl, &BirthControl::birthComplete, this, &AgentDeliveryWizard::onBirthComplete), OC_CONTYPE_NON_UNIQUE) {
				qWarning()<<"ERROR: Could not connect";
			}
			
			reset();
		}
		if(mNode.isNull()) {
			qWarning()<<"WARNING: agent delivery configured with null node";
		}
	}
}


void AgentDeliveryWizard::reset()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
	ui->comboBoxGender->setCurrentIndex(0);
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}


Settings *AgentDeliveryWizard::settings()
{
	OC_METHODGATE();
	return (!mNode.isNull())?mNode->settings().data():nullptr;
}


void AgentDeliveryWizard::startBirth()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			QString name = ui->lineEditName->text();
			name[0] = name[0].toUpper();
			GenderGenerator gg;
			QString gender = 0 > ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():gg.generate();
			qDebug() << "XXX - Started birth of agent with NAME: " << name << ", GENDER: " << gender;
			mSpinner->start();
			ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
			mBirthControl.birth(name, gender);
		}
	}
}


void AgentDeliveryWizard::onBirthProgress(const QString &step, int index, int total){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "AgentDeliveryWizard:onBirthProgress " << step << index << total;
	}
}


void AgentDeliveryWizard::announceBirth(const PortableID &id){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QString text = "My name is " + id.name() + ". I am an octomy "+nodeTypeToString(id.type())+". How do you do!";
		if(mUseVoice){
			VoiceManager::speak(id, text);
		}
	}
}


void AgentDeliveryWizard::onBirthComplete(bool ok, const QString &message)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "AgentDeliveryWizard:onBirthProgress " << ok << message;
		mSpinner->stop();
		ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
		PortableID id;
		if(!mNode.isNull()){
			auto nodeIdentity = mNode->nodeIdentity();
			if(!nodeIdentity.isNull()){
				id = nodeIdentity->toPortableID();
			}
		}
		ui->comboBoxGender->setCurrentText(id.gender());
		ui->widgetBirthCertificate->setPortableID(id);
		ui->stackedWidget->setCurrentWidget(ui->pageDone);
		announceBirth(id);
	}
}


void AgentDeliveryWizard::randomizeName()
{
	OC_METHODGATE();
	ui->lineEditName->setText(mNameGenerator.generate());
}


void AgentDeliveryWizard::randomizeGender()
{
	OC_METHODGATE();
	ui->comboBoxGender->setCurrentIndex((rand()%(ui->comboBoxGender->count()-1))+1);
}


void AgentDeliveryWizard::startDelivery()
{
	OC_METHODGATE();
	startBirth();
}


void AgentDeliveryWizard::deliveryDone()
{
	OC_METHODGATE();
	emit done(true);
}
