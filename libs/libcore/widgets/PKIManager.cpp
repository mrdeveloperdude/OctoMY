#include "PKIManager.hpp"
#include "ui_PKIManager.h"

#include "../libpki/qpolarsslpki.hpp"

#include <QDebug>

PKIManager::PKIManager(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PKIManager)
{
	ui->setupUi(this);
}

PKIManager::~PKIManager()
{
	delete ui;
}



void PKIManager::on_pushButtonGenerateKeyPair_clicked(){
	qpolarssl::Pki pki;
	int ret=0;
	ret=pki.generateKeyPair(2048);
	if(0==ret){
		QByteArray key=pki.getPEMKey();
		QByteArray pubkey=pki.getPEMPubkey();
		ui->plainTextEditPrivateKey->setPlainText(key);
		ui->plainTextEditPublicKey->setPlainText(pubkey);
	}
	else{
		qWarning("ERROR Exporting generated key as PEM");
	}

}

void PKIManager::on_pushButtonSaveKeyPair_clicked()
{
	qDebug()<<"TODO: Implement SAVE";
}

void PKIManager::on_pushButtonLoadKeyPair_clicked()
{
	qDebug()<<"TODO: Implement LOAD";
}
