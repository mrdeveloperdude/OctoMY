#include "PairingWidget.hpp"
#include "ui_PairingWidget.h"

PairingWidget::PairingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PairingWidget)
{
	ui->setupUi(this);
	ui->labelSecurityWarning->setVisible(!ui->checkBoxSecurity->isChecked());
	ui->stackedWidget->setCurrentWidget(ui->pagePairingMethod);
}

PairingWidget::~PairingWidget()
{
	delete ui;
}




/////////////////////////////////////////////////////////////////////////



void PairingWidget::on_checkBoxSecurity_toggled(bool checked)
{
	ui->labelSecurityWarning->setVisible(!checked);
}

void PairingWidget::on_pushButtonBack_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairingMethod);
}

void PairingWidget::on_pushButtonPairLAN_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageScan);
}

void PairingWidget::on_pushButtonPairCamera_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCamera);
}

void PairingWidget::on_pushButtonBack_2_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairingMethod);
}

void PairingWidget::on_pushButtonPairZOO_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageUnderConstruction);
}

void PairingWidget::on_pushButtonPairBluetooth_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageUnderConstruction);
}

void PairingWidget::on_pushButtonPairNFC_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageUnderConstruction);
}

void PairingWidget::on_pushButtonBack_3_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairingMethod);
}

void PairingWidget::on_pushButtonBack_4_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageScan);
}


void PairingWidget::on_listWidgetScanned_doubleClicked(const QModelIndex &index)
{
	ui->stackedWidget->setCurrentWidget(ui->pagePair);
	ui->widgetIdenticon->setIdenticonData(0x1337);
	ui->plainTextEditIdentity->setPlainText("Name: Golem\nID: 92345703ertklerfgnjwerht9\nVersion: OctoMY™ Remote 0.1beta\n");
}


void PairingWidget::on_listWidgetPaired_doubleClicked(const QModelIndex &index)
{
	ui->stackedWidget->setCurrentWidget(ui->pagePair);
	ui->widgetIdenticon->setIdenticonData(0x234623461338);
	ui->plainTextEditIdentity->setPlainText("Name: Devol\nID: 5sd34f4s5fg5g5h23sdgsdfhehj\nVersion: OctoMY™ Agent 0.12beta\n");
}
