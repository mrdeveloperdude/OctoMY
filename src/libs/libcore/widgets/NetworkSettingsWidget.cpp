#include "NetworkSettingsWidget.hpp"
#include "ui_NetworkSettingsWidget.h"

#include "utility/Utility.hpp"
#include "basic/LocalAddressList.hpp"

#include <QComboBox>



NetworkSettingsWidget::NetworkSettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::NetworkSettingsWidget)
	, mLastPort(0)
	, mLastValidity(false)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentWidget(ui->pageView);
	set(false);
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	ui=nullptr;
}


void NetworkSettingsWidget::configure(LocalAddressList &localAddresses)
{
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
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	ui->lineEditLocalPort->setText(QString::number(nport));
	return set(sendSignal);
}

bool NetworkSettingsWidget::set(bool sendSignal)
{
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

	qDebug()<<"SET portOK="<<portOK<<", addrOK="<<addrOK<<", nport="<<nport<<", ok="<<ok;
	ui->widgetStatus->setLightOn(true);
	ui->widgetStatus->setLightColor(ok?Qt::green:Qt::red);
	if(ok) {
		mLastAddress=naddr;
		mLastPort=nport;
		mLastValidity=ok;
	}
	ui->pushButtonEdit->setText(mLastAddress.toString()+" : "+QString::number(mLastPort));
	if(sendSignal) {
		emit addressChanged(naddr, nport, ok);
	}
	return ok;
}


bool NetworkSettingsWidget::setAddress(QHostAddress naddr)
{
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	return set();
}



bool NetworkSettingsWidget::setPort(quint16 nport)
{
	ui->lineEditLocalPort->setText(QString::number(nport));
	return set();
}


QHostAddress NetworkSettingsWidget::address() const
{
	return QHostAddress(ui->comboBoxLocalAddress->currentText());
}


quint16 NetworkSettingsWidget::port(bool *ok) const
{
	return quint16(ui->lineEditLocalPort->text().toInt(ok));
}


void NetworkSettingsWidget::on_pushButtonEdit_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageEdit);
}

void NetworkSettingsWidget::on_pushButtonSave_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageView);
}

void NetworkSettingsWidget::on_comboBoxLocalAddress_currentIndexChanged(int)
{
	set();
}

void NetworkSettingsWidget::on_lineEditLocalPort_textChanged(const QString &arg1)
{
	set();
}
