#include "Message.hpp"
#include "ui_Message.h"

Message::Message(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::Message)
{
	ui->setupUi(this);
}

Message::~Message()
{
	delete ui;
}


void Message::setMesasge(const QString &message){
	ui->labelMessage->setText(message);
}
