#include "ConnectionWidget.hpp"
#include "ui_ConnectionWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"


#include "agent/AgentConstants.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ConnectionWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", AgentConstants::AGENT_CONNECT_BUTTON_COLOR, AgentConstants::AGENT_DISCONNECT_COLOR);
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


void ConnectionWidget::configure(NetworkAddress &localAddress, NetworkAddress &remoteAddress){
	OC_METHODGATE();
	mLocalAddress=localAddress;
	mRemoteAddress=remoteAddress;

	//Local
	ui->comboBoxLocalAddress->setCurrentText(mLocalAddress.ip().toString());
	ui->lineEditLocalPort->setText(QString::number(mLocalAddress.port()));

	//Target
	ui->lineEditTargetAddress->setText(mRemoteAddress.ip().toString());
	ui->lineEditTargetPort->setText(QString::number(mRemoteAddress.port()));

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

