#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "basic/Standard.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"
#include "basic/Node.hpp"
#include "random/RNG.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDateTime>

const quint64 AgentDeliveryWizard::MINIMUM_BIRTH_TIME=3000;

AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentDeliveryWizard)
	, spinner(nullptr)
	, settings(nullptr)
{
	ui->setupUi(this);
	const qint32 minLetters=3;
	QRegularExpression re("\\p{Ll}{"+QString::number(minLetters)+",20}");
	if(!re.isValid()){
		qWarning()<<"ERROR: Name validator regex was invalid: "<<re.errorString();
	}
	//Regex rules from http://stackoverflow.com/questions/38001256/handling-accented-letters-in-qregularexpressions-in-qt5/38001274#38001274
	//             and http://www.regular-expressions.info/unicode.html
	ui->lineEditName->setValidator(new QRegularExpressionValidator(re, this));
	{
		spinner=new WaitingSpinnerWidget(ui->labelBirthImage, true, false);
		SpinnerStyle style;
		style.setColor(QColor("white"));
		style.setRelatveSize(true);
		style.setNumberOfLines(24);
		style.setLineLength(10);
		style.setInnerRadius(40);
		style.setLineWidth(3);
		spinner->setStyle(style);
	}
	birthTimer.setInterval(MINIMUM_BIRTH_TIME); //Minimum birth time gives this moment some depth in case keygen should finish quickly.
	birthTimer.setSingleShot(true);

	if(!connect(&birthTimer, &QTimer::timeout, this, &AgentDeliveryWizard::onBirthComplete, OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s){
				ui->pushButtonOnward->setEnabled(s.length()>=minLetters);
},OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
}




void AgentDeliveryWizard::save(){
	if(nullptr!=settings){
	}
}

void AgentDeliveryWizard::reset(){
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}

static QString generateRandomGender(){
	RNG *rng=RNG::sourceFactory("devu");
	QString gender="Genderless";
	if(nullptr!=rng){
		QStringList alternatives;
		alternatives<<"Male"<<"Female"<<"Genderless";
		gender=alternatives[rng->generateInt32()%alternatives.size()];
		delete rng;
	}
	return gender;
}


void AgentDeliveryWizard::configure(Node *n){
	if(node!=n){
		node=n;
		if(nullptr!=node){
			settings=&node->settings();
			KeyStore &keystore=node->keyStore();
			if(!connect(&keystore, &KeyStore::keystoreReady, this, &AgentDeliveryWizard::onBirthComplete, OC_CONTYPE)){
				qWarning()<<"ERROR: Could not connect";
			}
			ui->lineEditName->setText(ng.generate());
			reset();
		}
	}
}

AgentDeliveryWizard::~AgentDeliveryWizard()
{
	delete ui;
}



void AgentDeliveryWizard::onBirthComplete(){
	if(nullptr!=node){
		KeyStore &keystore=node->keyStore();
		if((keystore.isReady() || keystore.hasError()) && !birthTimer.isActive()){
			qDebug()<<"XXX - Birth complete!";
			birthTimer.stop();
			spinner->stop();
			if(keystore.hasError()){
				qWarning()<<"XXX - ERROR: Birthdefects detected!";
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
			}
			else{
				QString id=keystore.localKey().id();
				qDebug()<<"XXX - All is good, ID: "<<id;
				mID.setID(id);
				mID.setType(TYPE_AGENT);
				mID.setBirthDate(QDateTime::currentMSecsSinceEpoch());
				ui->widgetBirthCertificate->setPortableID(mID);
				ui->stackedWidget->setCurrentWidget(ui->pageDone);
			}
		}
		else{
			qDebug()<<"XXX - Birth almost complete...";
		}
	}
}



void AgentDeliveryWizard::on_pushButtonDone_clicked()
{
	emit done(false);
}

void AgentDeliveryWizard::on_pushButtonPairNow_clicked()
{
	emit done(true);
}

void AgentDeliveryWizard::on_pushButtonRandomName_clicked()
{
	ui->lineEditName->setText(ng.generate());
}

void AgentDeliveryWizard::on_pushButtonOnward_clicked()
{
	if(nullptr!=node){
		KeyStore &keystore=node->keyStore();
		QString name=ui->lineEditName->text();
		name[0]=name[0].toUpper();
		mID.setName(name);
		QString gender=0>ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():generateRandomGender();
		mID.setGender(gender);
		qDebug()<<"XXX - Started birth for NAME: "<<name<<", GENDER: "<<gender;
		spinner->start();
		birthTimer.start();
		keystore.clear();
		keystore.bootstrap(false,true);
		ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
	}
}
