#include "ConnectionWidget.hpp"
#include "ui_ConnectionWidget.h"

#include "../libutil/utility/Standard.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ConnectionWidget)
{
	ui->setupUi(this);
	ui->tryToggleListen->configure("Connect","Connecting...","Connected");
	setEditsEnabled(false);
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onConnectStateChanged(TryToggleState)),OC_CONTYPE)){
		qWarning()<<"ERROR: could not connect";
	}
}

ConnectionWidget::~ConnectionWidget()
{
	if(!disconnect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onConnectStateChanged(TryToggleState)))){
		qWarning()<<"ERROR: could not disconnect";
	}
	delete ui;
}


void ConnectionWidget::configure(NetworkAddress &localAddress, NetworkAddress &remoteAddress){
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
	setEditsEnabled(OFF==s);
	emit connectStateChanged(s);
}

void ConnectionWidget::setEditsEnabled(bool e){
	ui->lineEditLocalPort->setEnabled(e);
	ui->lineEditTargetAddress->setEnabled(e);
	ui->lineEditTargetPort->setEnabled(e);
	ui->comboBoxLocalAddress->setEnabled(e);
}

quint16 ConnectionWidget::getLocalPort(){
	return ui->lineEditLocalPort->text().toInt();
}

QHostAddress ConnectionWidget::getLocalAddress(){
	return QHostAddress(ui->comboBoxLocalAddress->currentText());
}

quint16 ConnectionWidget::getTargetPort(){
	return ui->lineEditTargetPort->text().toInt();
}


QHostAddress ConnectionWidget::getTargetAddress(){
	return QHostAddress(ui->lineEditTargetAddress->text());
}

TryToggleState ConnectionWidget::connectState()
{
	return ui->tryToggleListen->state();
}



void ConnectionWidget::hookSignals(QObject &ob){
	Q_UNUSED(ob);
}


void ConnectionWidget::unHookSignals(QObject &ob){
	Q_UNUSED(ob);

}


void ConnectionWidget::setConnectState(TryToggleState s){
	ui->tryToggleListen->setState(s);

}

