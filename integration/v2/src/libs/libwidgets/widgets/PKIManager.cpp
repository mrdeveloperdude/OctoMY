#include "PKIManager.hpp"
#include "ui_PKIManager.h"

#include "qpolarssl/qpolarsslpki.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QDebug>

PKIManager::PKIManager(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::PKIManager)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

PKIManager::~PKIManager()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void PKIManager::on_pushButtonGenerateKeyPair_clicked()
{
	OC_METHODGATE();
	qpolarssl::Pki pki;
	int ret=0;
	ret=pki.generateKeyPair(2048);
	if(0==ret) {
		QByteArray key=pki.getPEMKey();
		QByteArray pubkey=pki.getPEMPubkey();
		ui->plainTextEditPrivateKey->setPlainText(key);
		ui->plainTextEditPublicKey->setPlainText(pubkey);
	} else {
		qWarning("ERROR Exporting generated key as PEM");
	}

}

void PKIManager::on_pushButtonSaveKeyPair_clicked()
{
	OC_METHODGATE();
	qDebug()<<"TODO: Implement SAVE";
}

void PKIManager::on_pushButtonLoadKeyPair_clicked()
{
	OC_METHODGATE();
	qDebug()<<"TODO: Implement LOAD";
}
