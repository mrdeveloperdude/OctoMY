#include "ControlPairingWidget.hpp"
#include "ui_ControlPairingWidget.h"

#include "basic/Settings.hpp"

#include <QDesktopServices>
#include <QUrl>

ControlPairingWidget::ControlPairingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ControlPairingWidget)
{
	ui->setupUi(this);

	//Hook onward buttons to go to next page in stack
	QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
	//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
	for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
		QPushButton*onward=(*it);
		//qDebug()<<" + ONWARD: "<<onward->objectName();
		connect(onward, &QPushButton::clicked,this,[=](bool b){
			int  next=(ui->stackedWidget->currentIndex() + 1 ) % ui->stackedWidget->count();
			ui->stackedWidget->setCurrentIndex(next);
		});
	}
	reset();
}

ControlPairingWidget::~ControlPairingWidget()
{
	delete ui;
}



void ControlPairingWidget::save(){
	//Settings &s=Settings::getInstance();
	//s.setCustomSettingBool("octomy.face",ui->checkBoxFace->isChecked());
	//s.getCustomSettingBool("octomy.face"));
}

void ControlPairingWidget::reset(){
	ui->stackedWidget->setCurrentWidget(ui->pageWelcome);
}

void ControlPairingWidget::on_pushButtonDone_clicked()
{
	save();
	emit done();
}

void ControlPairingWidget::on_pushButtonYoutubeTutorial_clicked()
{
	QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));
}

void ControlPairingWidget::on_pushButtonSkip_clicked()
{
	save();
	emit done();
}

void ControlPairingWidget::on_pushButtonMaybeOnward_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->tableWidgetControls->rowCount()>0?ui->pageDone:ui->pageNoAgents);
}
