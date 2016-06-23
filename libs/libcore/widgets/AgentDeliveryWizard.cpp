#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"

#include <QDebug>
#include <QRegularExpressionValidator>
#include <QRegularExpression>


AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentDeliveryWizard)
	, spinner(nullptr)
	, settings(nullptr)
{
	ui->setupUi(this);
	const quint32 minLetters=3;
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
	birthTimer.setInterval(4000); //Minimum birth time gives this moment some depth in case keygen should finish quickly.
	birthTimer.setSingleShot(true);

	if(!connect(&birthTimer, SIGNAL(timeout()), this,SLOT(onBirthComplete()))){
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->lineEditName, &QLineEdit::textEdited, this, [=](QString s){
				ui->pushButtonOnward->setEnabled(s.length()>=minLetters);
})){
		qWarning()<<"ERROR: Could not connect";
	}
}




void AgentDeliveryWizard::save(){
	if(nullptr!=settings){
		settings->setCustomSettingBool("octomy.face",ui->checkBoxFace_2->isChecked());
		//	s.setCustomSettingBool("octomy.debug.log",ui->checkBoxDebugMode_2->isChecked());
		settings->setCustomSettingBool("octomy.zoo.blog",ui->checkBoxZooBlog_2->isChecked());
		//s.getCustomSettingBool("octomy.face"));
	}
}

void AgentDeliveryWizard::reset(){

	/*
	ui->checkBoxFace->setChecked(s.getCustomSettingBool("octomy.face"));
	ui->checkBoxDebugMode->setChecked(s.getCustomSettingBool("octomy.debug.log"));
	ui->checkBoxZooBlog->setChecked(s.getCustomSettingBool("octomy.zoo.blog"));
	*/
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);

}
#include "random/RNG.hpp"
QString generateRandomGender(){
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

#include "basic/Node.hpp"
void AgentDeliveryWizard::configure(Node *n){
	node=n;
	if(nullptr!=node){
		settings=&node->getSettings();
		KeyStore &keystore=node->getKeyStore();
		KeyStore *keystorep=&keystore;

		if(!connect(&keystore, SIGNAL(keystoreReady()), this, SLOT(onBirthComplete()))){
			qWarning()<<"ERROR: Could not connect";
		}

		// Hook onward buttons to go to the correct page in stack
		QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
		//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
		for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
			QPushButton*onward=(*it);
			//qDebug()<<" + ONWARD: "<<onward->objectName();
			connect(onward, &QPushButton::clicked,this,[this, keystorep](bool b){
				// Skip pages that are not relevant to the selection made in "basic" page
				int next=ui->stackedWidget->currentIndex();
				while(true){
					next = (next + 1) % ui->stackedWidget->count();
					/*				QWidget *nextWidget = ui->stackedWidget->widget(next);
					if((!ui->checkBoxFace->isChecked()) && (ui->pageFace==nextWidget)){
						continue;
					}
					if((!ui->checkBoxDebugMode->isChecked()) && (ui->pageDebug==nextWidget)){
						continue;
					}
					if((!ui->checkBoxZooBlog->isChecked()) && (ui->pageZoo==nextWidget)){
						continue;
					}
					*/
					break;
				}
				// Align buddy checkboxes
				const QWidget *currentWidget=ui->stackedWidget->currentWidget();
				const QWidget *nextWidget = ui->stackedWidget->widget(next);
				qDebug()<<"ONWARD TRANSITION FROM "<<currentWidget->objectName()<<" TO "<<nextWidget->objectName();

				if(nextWidget==ui->pageBirthInProgress){
					QString name=ui->lineEditName->text();
					name[0]=name[0].toUpper();
					ui->labelName->setText(name);
					QString gender=0>ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():generateRandomGender();
					ui->labelGender->setText(gender);
					qDebug()<<"NAME: "<<name<<", GENDER: "<<gender;
				}
				/*
				else if(currentWidget==ui->pageZoo){
					ui->checkBoxZooBlog->setChecked(ui->checkBoxZooBlog_2->isChecked());
				}
				else if(currentWidget==ui->pageDebug){
					ui->checkBoxDebugMode->setChecked(ui->checkBoxDebugMode_2->isChecked());
				}
	*/
				if(nextWidget==ui->pageBirthInProgress){
					qDebug()<<"XXX - Started birth...";
					spinner->start();
					birthTimer.start();
					if(!keystorep->isReady()){
						qDebug()<<"XXX - NOT READY, BOOTSTRAPPING...";
						keystorep->bootstrap();
					}
				}
				ui->stackedWidget->setCurrentIndex(next);
			});
		}

		ui->lineEditName->setText(ng.generate());

		reset();
	}
}

AgentDeliveryWizard::~AgentDeliveryWizard()
{
	delete ui;
}

void AgentDeliveryWizard::on_pushButtonDone_clicked()
{
	emit done(false);
}

void AgentDeliveryWizard::on_pushButtonPairNow_clicked()
{
	emit done(true);
}


void AgentDeliveryWizard::onBirthComplete(){
	if(nullptr!=node){
		KeyStore &keystore=node->getKeyStore();
		if((keystore.isReady() || keystore.isError()) && !birthTimer.isActive()){
			qDebug()<<"XXX - Birth complete!";
			birthTimer.stop();
			spinner->stop();

			if(keystore.isError()){
				qWarning()<<"XXX - ERROR: Birthdefects detected!";
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
			}
			else{
				QString id=keystore.getLocalID();
				qDebug()<<"ID: "<<id;
				ui->labelID->setText(id);
				ui->stackedWidget->setCurrentWidget(ui->pageBirthDone);
			}
		}
		else{
			qDebug()<<"XXX - Birth almost complete...";
		}
	}
}

void AgentDeliveryWizard::on_pushButtonRandomName_clicked()
{
	ui->lineEditName->setText(ng.generate());
}
