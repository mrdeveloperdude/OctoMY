#include "CommsCarrierTestWidget.hpp"
#include "ui_CommsCarrierTestWidget.h"

#include <QDebug>

CommsCarrierTestWidget::CommsCarrierTestWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CommsCarrierTestWidget)
{
	ui->setupUi(this);
}


CommsCarrierTestWidget::~CommsCarrierTestWidget()
{
	delete ui;
}

void CommsCarrierTestWidget::log(const QString &sender,const QString &message){
	const auto statement = QString("[%1]: %2").arg(sender).arg(message);
	qDebug()<<"LOG: "<< statement;
	ui->listWidget->addItem(statement);
}

void CommsCarrierTestWidget::configureA(){
	emit configureAImpl();
}

void CommsCarrierTestWidget::configureB(){
	emit configureBImpl();
}

void CommsCarrierTestWidget::connectA(bool con){
	emit connectAImpl(con);
}


void CommsCarrierTestWidget::connectB(bool con){
	emit connectBImpl(con);
}

void CommsCarrierTestWidget::activateA(bool con){
	emit activateAImpl(con);
}


void CommsCarrierTestWidget::activateB(bool con){
	emit activateBImpl(con);
}


void CommsCarrierTestWidget::intervalChangeA(int interval){
	emit intervalChangeAImpl(interval);
}

void CommsCarrierTestWidget::intervalChangeB(int interval){
	emit intervalChangeBImpl(interval);
}
