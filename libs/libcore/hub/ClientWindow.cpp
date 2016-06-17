#include "ClientWindow.hpp"
#include "ui_ClientWindow.h"
#include "basic/Standard.hpp"

ClientWindow::ClientWindow(Client *client,QWidget *parent) :
	QWidget(parent)
  , ui(new Ui::ClientWindow)
  , client(client)
{
	ui->setupUi(this);
	summaryTimer.setInterval(100);
	if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)){
		qDebug()<<"could not connect";
	}
	summaryTimer.start();

}

ClientWindow::~ClientWindow(){
	delete ui;
}




void ClientWindow::onSummaryTimer(){
	if(0==client){
		ui->labelConnectionStatus->setText("N/A");
	}
	else{
		ui->labelConnectionStatus->setText(client->getSummary());
	}
}
