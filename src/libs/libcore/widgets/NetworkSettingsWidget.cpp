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
	configure();
	set(false);
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	delete ui;
	ui=nullptr;
}


void NetworkSettingsWidget::configure()
{
	ui->comboBoxLocalAddress->clear();
	QList<QHostAddress> localAddresses=utility::allLocalNetworkAddresses();
	for(QHostAddress adr:localAddresses) {
		if((QAbstractSocket::IPv4Protocol==adr.protocol()) && (!adr.isLoopback()) ) {
			ui->comboBoxLocalAddress->addItem(adr.toString());
		}
	}
	ui->comboBoxLocalAddress->setCurrentText(QHostAddress(QHostAddress::LocalHost).toString());
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
	bool ok=false;
	ok= (portOK && (ui->comboBoxLocalAddress->findText(naddr.toString())>-1) && utility::isAddressOK(naddr.toString(), nport));
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
