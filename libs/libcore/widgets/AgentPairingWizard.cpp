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
	reset();
	discovery.setRole(ROLE_AGENT);

	// Hook onward buttons to go to the correct page in stack
	QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
	//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
	for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
		QPushButton*onward=(*it);
		//qDebug()<<" + ONWARD: "<<onward->objectName();
		connect(onward, &QPushButton::clicked,this,[=](bool b){
			// Skip pages that are not relevant to the selection made in "basic" page
			int next = (ui->stackedWidget->currentIndex() + 1) % ui->stackedWidget->count();
			ui->stackedWidget->setCurrentIndex(next);
		});
	}


}

AgentPairingWizard::~AgentPairingWizard()
{
	delete ui;
}

void AgentPairingWizard::reset(){
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithControls);
}


void AgentPairingWizard::showEvent(QShowEvent *){
	//discovery.start();
}

void AgentPairingWizard::hideEvent(QHideEvent *){
	discovery.stop();
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
	//pm.stop();//Not necessary, well handled by hide event handler above
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

void AgentPairingWizard::on_pushButtonDiscoverNow_clicked()
{
	discovery.discover();
}
