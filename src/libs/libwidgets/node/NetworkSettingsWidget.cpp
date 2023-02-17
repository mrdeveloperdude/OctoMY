#include "NetworkSettingsWidget.hpp"
#include "ui_NetworkSettingsWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/network/Network.hpp"

#include "address/LocalAddressList.hpp"

#include <QComboBox>
#include <QMessageBox>
#include <QWidget>
#include <QIntValidator>

Q_DECLARE_METATYPE(QHostAddress)


NetworkSettingsWidget::NetworkSettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::NetworkSettingsWidget)
	, mLastPort(0)
	, mLastPortOK(false)
	, mLastAddressOK(false)
	, mMuteSignals(false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->stackedWidget->setCurrentWidget(ui->pageView);
	ui->lineEditLocalPort->setValidator( new QIntValidator(0, 0xFFFF, this) );
	verifyAndSet(false);
	qRegisterMetaType<QHostAddress>("QHostAddress");
}

NetworkSettingsWidget::~NetworkSettingsWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void NetworkSettingsWidget::configure(QSharedPointer<LocalAddressList> localAddresses)
{
	OC_METHODGATE();
	mMuteSignals=true;
	ui->comboBoxLocalAddress->clear();
	qDebug()<<"Configuring address list";
	if(!localAddresses.isNull()) {
		for(QHostAddress &adr:*localAddresses) {
			if(isAddrToBeAdded(adr)) {
				ui->comboBoxLocalAddress->addItem(adr.toString());
				qDebug()<<" + Adding "<<adr.toString();
			}
			else {
				qDebug()<<" + Skipping "<<adr.toString();
			}
		}
		
		setHostAddress(localAddresses->currentAddress(), localAddresses->port(), true, false);
		auto ct = ui->comboBoxLocalAddress->count();
		qDebug()<<"THERE ARE" <<ct<<"ITEMS IN COMBOBOX:";
		for(int i=0;i<ct;++i){
			qDebug()<<" + " <<ui->comboBoxLocalAddress->itemText(i);
		}
		
	}
	else{
		qWarning()<<"ERROR: No local addresses";
	}
	mMuteSignals=false;
}

bool NetworkSettingsWidget::setHostAddress(QHostAddress naddr, quint16 nport, bool verify, bool sendSignal)
{
	OC_METHODGATE();
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	ui->lineEditLocalPort->setText(QString::number(nport));
	bool ok=false;
	if(verify) {
		ok=verifyAndSet(sendSignal);
	}
	return ok;
}

bool NetworkSettingsWidget::isAddrToBeAdded(QHostAddress adr)
{
	OC_METHODGATE();
	return ((QAbstractSocket::IPv4Protocol==adr.protocol()) && (!adr.isLoopback()) );
}


bool NetworkSettingsWidget::isAddrOK(QHostAddress naddr)
{
	OC_METHODGATE();
	return (ui->comboBoxLocalAddress->findText(naddr.toString())>-1);
}

bool NetworkSettingsWidget::isPortOK(QHostAddress naddr, quint16 nport)
{
	OC_METHODGATE();
	return utility::network::checkUDPAddress(naddr, nport);
}


bool NetworkSettingsWidget::verifyAndSet(bool sendSignal, bool doCorrection)
{
	OC_METHODGATE();
	QHostAddress naddr=(address());
	bool addrOK=isAddrOK(naddr);
	quint16 nport=(port());
	bool portOK=isPortOK(naddr, nport);
	//qDebug().noquote().nospace()<<"VERIFY & SET BEFORE naddr="<<naddr<<"("<<addrOK<<"), nport="<<nport<<"("<<portOK<<") sendSignal="<<sendSignal<<", doCorrection="<<doCorrection;
	if(doCorrection) {
		if(!addrOK) {
			// Correct bad address by selecting first entry in combobox (combobox is assumed to be all GOOD addresses)
			if(ui->comboBoxLocalAddress->count()>0) {
				naddr=QHostAddress(ui->comboBoxLocalAddress->itemText(0));
				//qDebug()<<"Found free address: "<<naddr;
				setAddress(naddr, false, false);
				naddr=address();
				addrOK=isAddrOK(naddr);
				//qDebug()<<"Free address: "<<naddr<<"("<<addrOK<<")";
			}
		}
		if(addrOK && !portOK) {
			// Correct bad port by finding an available one for us!
			nport = utility::network::freeUDPPortForAddress(naddr);
			if(0 != nport) {
				//qDebug()<<"Found free port: "<<nport;
				setPort(nport, false, false);
				nport=port();
				portOK=isPortOK(naddr, nport);
				//qDebug()<<"Free port: "<<nport<<"("<<portOK<<")";
			}
		}
	}
	//qDebug().noquote().nospace()<<"VERIFY & SET AFTER  naddr="<<naddr<<"("<<addrOK<<"), nport="<<nport<<"("<<portOK<<") sendSignal="<<sendSignal<<", doCorrection="<<doCorrection;
	const bool ok=(addrOK && portOK);
	ui->widgetStatus->setLightColor(ok?LightWidget::sDefaultOKColor:LightWidget::sDefaultErrorColor);
	ui->widgetStatus->setLightOn(true);
	// Remeber the new good stuff
	if(portOK) {
		mLastPort=nport;
		mLastPortOK=portOK;
	}
	if(addrOK) {
		mLastAddress=naddr;
		mLastAddressOK=addrOK;
	}
	// Correct to last known good if everything else fails
	if(doCorrection) {
		if(!addrOK && mLastAddressOK) {
			setAddress(mLastAddress, false, false);
		}
		if(!portOK && mLastPortOK) {
			setPort(mLastPort, false, false);
		}
	}
	ui->pushButtonEdit->setText(mLastAddress.toString()+" : "+QString::number(mLastPort));
	if(sendSignal && (naddr != mLastAddress || nport != mLastPort)) {
		emit addressChanged(mLastAddress, mLastPort, ok);
	}
	return ok;
}


bool NetworkSettingsWidget::setAddress(QHostAddress naddr, bool verify, bool sendSignal)
{
	OC_METHODGATE();
	ui->comboBoxLocalAddress->setCurrentText(naddr.toString());
	bool ok=false;
	if(verify) {
		ok=verifyAndSet(sendSignal);
	}
	return ok;
}



bool NetworkSettingsWidget::setPort(quint16 nport, bool verify, bool sendSignal)
{
	OC_METHODGATE();
	ui->lineEditLocalPort->setText(QString::number(nport));
	bool ok=false;
	if(verify) {
		ok=verifyAndSet(sendSignal);
	}
	return ok;
}


QHostAddress NetworkSettingsWidget::address() const
{
	OC_METHODGATE();
	return QHostAddress(ui->comboBoxLocalAddress->currentText());
}


quint16 NetworkSettingsWidget::port() const
{
	OC_METHODGATE();
	bool ok=false;
	quint16 nport(ui->lineEditLocalPort->text().toUShort(&ok));
	if(!ok) {
		// Signal the port is bad
		nport=0;
	}
	return nport;
}


void NetworkSettingsWidget::on_pushButtonEdit_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageEdit);
}

void NetworkSettingsWidget::on_pushButtonSave_clicked()
{
	OC_METHODGATE();
	bool go=false;
	const bool ok=verifyAndSet(false, false);
	
	if(!ok) {
		QMessageBox::StandardButton reply = QMessageBox::question(this, "The settings are invalid", "Would you like to automatically correct them?", QMessageBox::No|QMessageBox::Yes);
		if (QMessageBox::Yes==reply) {
			//qDebug()<<"OK";
			go=verifyAndSet(true, true);
		}
	} else {
		go=true;
	}
	if(go) {
		//qDebug()<<"Back to civ";
		ui->stackedWidget->setCurrentWidget(ui->pageView);
	}
}

void NetworkSettingsWidget::on_lineEditLocalPort_textChanged(const QString &)
{
	OC_METHODGATE();
	if(!mMuteSignals) {
		verifyAndSet(false, false);
	}
}

void NetworkSettingsWidget::on_lineEditLocalPort_editingFinished()
{
	OC_METHODGATE();
	if(!mMuteSignals) {
		verifyAndSet(false, false);
	}
}


void NetworkSettingsWidget::on_comboBoxLocalAddress_currentIndexChanged(int index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	qDebug()<<"CHANGED TO "<<ui->comboBoxLocalAddress->currentText();
	if(!mMuteSignals) {
		verifyAndSet();
	}
}
