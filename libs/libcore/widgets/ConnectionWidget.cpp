#include "ConnectionWidget.hpp"
#include "ui_ConnectionWidget.h"

#include "basic/Standard.hpp"

ConnectionWidget::ConnectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConnectionWidget)
{
	ui->setupUi(this);
	ui->tryToggleListen->setText("Connect","Connecting...","Connected");
	setEditsEnabled(false);
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onConnectStateChanged(TryToggleState)),WWCONTYPE)){
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


void ConnectionWidget::configure(QString base){
	//Local
	ui->comboBoxLocalAddress->configure(base+"-listen-address","Local address");
	ui->lineEditLocalPort->configure("",base+"-listen-port","Local port");

	//Target
	ui->lineEditTargetAddress->configure("",base+"-target-address","Target address");
	ui->lineEditTargetPort->configure("",base+"-target-port", "Target port");


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



void ConnectionWidget::hookSignals(QObject &ob){
	Q_UNUSED(ob);
}


void ConnectionWidget::unHookSignals(QObject &ob){
	Q_UNUSED(ob);

}


void ConnectionWidget::setConnectState(TryToggleState s){
	ui->tryToggleListen->setState(s);

}

