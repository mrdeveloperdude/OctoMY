#include "AgentDeliveryWizard.hpp"
#include "ui_AgentDeliveryWizard.h"

#include "basic/Settings.hpp"
#include "widgets/WaitingSpinnerWidget.hpp"

#include <QDebug>


AgentDeliveryWizard::AgentDeliveryWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentDeliveryWizard)
	, spinner(nullptr)
	, settings(nullptr)
{
	ui->setupUi(this);
	spinner=new WaitingSpinnerWidget(ui->labelBirthImage, true, false);
	SpinnerStyle style;
	style.setColor(QColor("white"));
	style.setRelatveSize(true);
	style.setNumberOfLines(24);
	style.setLineLength(10);
	style.setInnerRadius(40);
	style.setLineWidth(3);
	spinner->setStyle(style);
	birthTimer.setInterval(4000); //Minimum birth time gives this moment some depth in case keygen should finish quickly.
	birthTimer.setSingleShot(true);

	if(!connect(&birthTimer, SIGNAL(timeout()), this,SLOT(onBirthComplete()))){
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
					QWidget *nextWidget = ui->stackedWidget->widget(next);
					/*
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
				/*
				if(currentWidget==ui->pageBasics){
					ui->checkBoxFace_2->setChecked(ui->checkBoxFace->isChecked());
					ui->checkBoxZooBlog_2->setChecked(ui->checkBoxZooBlog->isChecked());
					ui->checkBoxDebugMode_2->setChecked(ui->checkBoxDebugMode->isChecked());
				}
				else if(currentWidget==ui->pageFace){
					ui->checkBoxFace->setChecked(ui->checkBoxFace_2->isChecked());
				}
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
				ui->stackedWidget->setCurrentWidget(ui->pageBirthDone);
				if(nullptr!=settings){
					settings->setCustomSettingBool("octomy.delivered",true);
				}
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
