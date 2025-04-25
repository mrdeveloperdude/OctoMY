#include "MessageActivity.hpp"
#include "ui_MessageActivity.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "uptime/MethodGate.hpp"

MessageActivity::MessageActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::MessageActivity)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


MessageActivity::~MessageActivity()
{
	OC_METHODGATE();
	delete ui;
}


void MessageActivity::end(bool status){
	OC_METHODGATE();
	qWarning()<< "Message for '"<< mID <<"' ended with status "<< status;
	pop(QStringList() << mID << (status?"true":"false") );
}

void MessageActivity::confirmed(){
	OC_METHODGATE();
	end(true);
}


void MessageActivity::canceled(){
	OC_METHODGATE();
	end(false);
}


static bool isTruthy(const QString &string){
	return "true" == string;
}


void MessageActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	QString title;
	QString bodyMD;
	QString iconPath;
	bool askConfirmation{false};
	const auto sz = arguments.size();
	if(sz > 0){
		mID = arguments[0];
	}
	if(sz > 1){
		title = arguments[1];
	}
	if(sz > 2){
		bodyMD = arguments[2];
	}
	if(sz > 3){
		iconPath = arguments[3];
	}
	if(sz > 4){
		askConfirmation = isTruthy(arguments[4]);
	}
	ui->labelTitle->setText(title);
	ui->labelBodyMD->setTextFormat(Qt::MarkdownText);
	ui->labelBodyMD->setText(bodyMD);
	ui->labelIcon->setPixmap(QPixmap(iconPath));
	ui->pushButtonCancel->setVisible(askConfirmation);
	ui->pushButtonOK->setFocus();
}


void MessageActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
}
