#include "ConnectionWidget.hpp"
#include "ui_ConnectionWidget.h"

#include "app/Constants.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "comms/address/CarrierAddress.hpp"
#include "comms/address/CarrierAddressUDP.hpp"
//#include "agent/AgentConstants.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ConnectionWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	setEditsEnabled(false);
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onConnectStateChanged(TryToggleState)),OC_CONTYPE)){
		qWarning()<<"ERROR: could not connect";
	}
}

ConnectionWidget::~ConnectionWidget()
{
	OC_METHODGATE();
	if(!disconnect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onConnectStateChanged(TryToggleState)))){
		qWarning()<<"ERROR: could not disconnect";
	}
	delete ui;
	ui=nullptr;
}


void ConnectionWidget::configure(QSharedPointer<CarrierAddress> localAddress, QSharedPointer<CarrierAddress> remoteAddress){
	OC_METHODGATE();
	mLocalAddress = localAddress;
	mRemoteAddress = remoteAddress;
	// TODO: Update widget to handle all address types
	auto localAddressUDP = qSharedPointerDynamicCast<CarrierAddressUDP>(mLocalAddress);
	if(!localAddressUDP.isNull()){
		//Local
		ui->comboBoxLocalAddress->setCurrentText(localAddressUDP->ip().toString());
		ui->lineEditLocalPort->setText(QString::number(localAddressUDP->port()));
	}
	auto remoteAddressUDP = qSharedPointerDynamicCast<CarrierAddressUDP>(mRemoteAddress);
	if(!remoteAddressUDP.isNull()){
		//Target
		ui->lineEditTargetAddress->setText(remoteAddressUDP->ip().toString());
		ui->lineEditTargetPort->setText(QString::number(remoteAddressUDP->port()));
	}
	setEditsEnabled(true);
}


void ConnectionWidget::onConnectStateChanged(TryToggleState s){
	OC_METHODGATE();
	setEditsEnabled(OFF==s);
	emit connectStateChanged(s);
}

void ConnectionWidget::setEditsEnabled(bool e){
	OC_METHODGATE();
	ui->lineEditLocalPort->setEnabled(e);
	ui->lineEditTargetAddress->setEnabled(e);
	ui->lineEditTargetPort->setEnabled(e);
	ui->comboBoxLocalAddress->setEnabled(e);
}

quint16 ConnectionWidget::getLocalPort(){
	OC_METHODGATE();
	return static_cast<quint16>(ui->lineEditLocalPort->text().toShort());
}

QHostAddress ConnectionWidget::getLocalAddress(){
	OC_METHODGATE();
	return QHostAddress(ui->comboBoxLocalAddress->currentText());
}

quint16 ConnectionWidget::getTargetPort(){
	OC_METHODGATE();
	return static_cast<quint16>(ui->lineEditTargetPort->text().toShort());
}

QHostAddress ConnectionWidget::getTargetAddress(){
	OC_METHODGATE();
	return QHostAddress(ui->lineEditTargetAddress->text());
}

TryToggleState ConnectionWidget::connectState()
{
	OC_METHODGATE();
	return ui->tryToggleListen->state();
}

// TODO: Merge into single setHookSignal
void ConnectionWidget::hookSignals(QObject &ob){
	OC_METHODGATE();
	Q_UNUSED(ob);
}

// TODO: Merge into single setHookSignal
void ConnectionWidget::unHookSignals(QObject &ob){
	OC_METHODGATE();
	Q_UNUSED(ob);
}


void ConnectionWidget::setConnectState(TryToggleState s){
	OC_METHODGATE();
	ui->tryToggleListen->setState(s);
}

