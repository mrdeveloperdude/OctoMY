#include "AgentPairingWizard.hpp"
#include "ui_AgentPairingWizard.h"

#include "basic/Settings.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#include <QWidget>

AgentPairingWizard::AgentPairingWizard(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgentPairingWizard)
{
	ui->setupUi(this);
	// Hook onward buttons to go to the correct page in stack
	QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
	//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
	for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
		QPushButton*onward=(*it);
		//qDebug()<<" + ONWARD: "<<onward->objectName();
		connect(onward, &QPushButton::clicked,this,[=](bool b){
			// Skip pages that are not relevant to the selection made in "basic" page
			int next=ui->stackedWidget->currentIndex();
			while(true){
				next = (next + 1) % ui->stackedWidget->count();
				QWidget *nextWidget = ui->stackedWidget->widget(next);
				if((!ui->checkBoxFace->isChecked()) && (ui->pageFace==nextWidget)){
					continue;
				}
				if((!ui->checkBoxDebugMode->isChecked()) && (ui->pageDebug==nextWidget)){
					continue;
				}
				if((!ui->checkBoxZooBlog->isChecked()) && (ui->pageZoo==nextWidget)){
					continue;
				}
				break;
			}
			// Align buddy checkboxes
			const QWidget *currentWidget=ui->stackedWidget->currentWidget();
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
			ui->stackedWidget->setCurrentIndex(next);
		});
	}
	reset();
	pm.setRole(ROLE_AGENT);

}

AgentPairingWizard::~AgentPairingWizard()
{
	delete ui;
}




void AgentPairingWizard::save(){
	Settings &s=Settings::getInstance();
	s.setCustomSettingBool("octomy.face",ui->checkBoxFace_2->isChecked());
	s.setCustomSettingBool("octomy.debug",ui->checkBoxDebugMode_2->isChecked());
	s.setCustomSettingBool("octomy.zoo.blog",ui->checkBoxZooBlog_2->isChecked());
	//s.getCustomSettingBool("octomy.face"));
}

void AgentPairingWizard::reset(){
	Settings &s=Settings::getInstance();
	ui->checkBoxFace->setChecked(s.getCustomSettingBool("octomy.face"));
	ui->checkBoxDebugMode->setChecked(s.getCustomSettingBool("octomy.debug"));
	ui->checkBoxZooBlog->setChecked(s.getCustomSettingBool("octomy.zoo.blog"));
	ui->stackedWidget->setCurrentWidget(ui->pageWelcome);
}


void AgentPairingWizard::showEvent(QShowEvent *){
	pm.start();
}

void AgentPairingWizard::hideEvent(QHideEvent *){
	pm.stop();
}

void AgentPairingWizard::on_pushButtonMaybeOnward_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->tableWidgetControls->rowCount()>0?ui->pageNoControls:ui->pageNoControls);
}


void AgentPairingWizard::on_pushButtonTryAgain_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithControls);
}

void AgentPairingWizard::on_pushButtonDone_clicked()
{
	save();
	emit done();
}

void AgentPairingWizard::on_pushButtonYoutubeTutorial_clicked()
{
	QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));
}

void AgentPairingWizard::on_pushButtonCameraPair_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCameraPairing);
}

void AgentPairingWizard::on_pushButtonSkip_clicked()
{
	save();
	emit done();
}
