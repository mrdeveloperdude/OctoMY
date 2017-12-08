#include "NetworkSettingsWidget.hpp"
#include "ui_NetworkSettingsWidget.h"

#include "utility/Utility.hpp"
#include "basic/LocalAddressList.hpp"

#include <QComboBox>

Q_DECLARE_METATYPE(QHostAddress)


NetworkSettingsWidget::NetworkSettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::NetworkSettingsWidget)
	, mLastPort(0)
	, mLastValidity(false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->stackedWidget->setCurrentWidget(ui->pageView);
	set(false);
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void NetworkSettingsWidget::configure(LocalAddressList &localAddresses)
{
	OC_METHODGATE();
	ui->comboBoxLocalAddress->clear();
	for(QHostAddress adr:localAddresses) {
		if((QAbstractSocket::IPv4Protocol==adr.protocol()) && (!adr.isLoopback()) ) {
			ui->comboBoxLocalAddress->addItem(adr.toString());
		}
	}
	set(localAddresses.currentAddress(), localAddresses.port(), true);
}

bool NetworkSettingsWidget::set(QHostAddress naddr, quint16 nport, bool sendSignal)
{
	OC_METHODGATE();
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	ui->lineEditLocalPort->setText(QString::number(nport));
	return set(sendSignal);
}

bool NetworkSettingsWidget::set(bool sendSignal)
{
	OC_METHODGATE();
	bool portOK=false;
	const auto naddr=address();
	auto nport=port(&portOK);
	if( (mLastAddress==naddr) &&  (mLastPort==nport) ) {
		return mLastValidity;
	}
	const bool addrOK=(ui->comboBoxLocalAddress->findText(naddr.toString())>-1);
	bool ok=false;
	ok= (portOK && addrOK && utility::isAddressOK(naddr.toString(), nport));
	// Retry but this time pass port=0 to let function find available port for us!
	if(!ok) {
		nport=0;
		ok= (portOK && addrOK && utility::isAddressOK(naddr.toString(), nport));
		if(0!=nport) {
			qDebug()<<"Found free port: "<<nport;
			setPort(nport);
		}
	}

	qDebug().noquote().nospace()<<"SET addrOK="<<addrOK<<", portOK="<<portOK<<", nport="<<nport<<", ok="<<ok;
	ui->widgetStatus->setLightOn(true);
	ui->widgetStatus->setLightColor(ok?Qt::green:Qt::red);
	if(ok) {
		mLastAddress=naddr;
		mLastPort=nport;
		mLastValidity=ok;
	}
	ui->pushButtonEdit->setText(mLastAddress.toString()+" : "+QString::number(mLastPort));
	if(sendSignal) {
		qRegisterMetaType<QHostAddress>("QHostAddress");
		emit addressChanged(naddr, nport, ok);
	}
	return ok;
}


bool NetworkSettingsWidget::setAddress(QHostAddress naddr)
{
	OC_METHODGATE();
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	return set();
}



bool NetworkSettingsWidget::setPort(quint16 nport)
{
	OC_METHODGATE();
	ui->lineEditLocalPort->setText(QString::number(nport));
	return set();
}


QHostAddress NetworkSettingsWidget::address() const
{
	OC_METHODGATE();
	return QHostAddress(ui->comboBoxLocalAddress->currentText());
}


quint16 NetworkSettingsWidget::port(bool *ok) const
{
	OC_METHODGATE();
	return quint16(ui->lineEditLocalPort->text().toInt(ok));
}


void NetworkSettingsWidget::on_pushButtonEdit_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageEdit);
}

void NetworkSettingsWidget::on_pushButtonSave_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageView);
}

void NetworkSettingsWidget::on_comboBoxLocalAddress_currentIndexChanged(int)
{
	OC_METHODGATE();
	set();
}

void NetworkSettingsWidget::on_lineEditLocalPort_textChanged(const QString &arg1)
{
	OC_METHODGATE();
	set();
}
