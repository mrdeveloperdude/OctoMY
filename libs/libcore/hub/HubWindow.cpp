#include "HubWindow.hpp"

#include "ui_HubWindow.h"

#include "basic/Standard.hpp"

#include "comms/Client.hpp"
#include "comms/messages/MessageType.hpp"
#include "hub/Hub.hpp"
#include "ClientWindow.hpp"
#include "utility/Utility.hpp"
#include "models/ClientModel.hpp"

#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"

#include "agent/Agent.hpp"
#include "agent/AgentWindow.hpp"

#include "widgets/hexedit/QHexEdit.hpp"
#include "widgets/hexedit/QHexEditData.hpp"
#include "hw/actuators/HexyTool.hpp"

#include "../libpki/qpolarsslpki.hpp"
#include "../libpki/qpolarsslhash.hpp"

#include "puppet/GaitController.hpp"

#include "utility/ScopedTimer.hpp"

#include "comms/CommsChannel.hpp"
#include "zoo/ZooClient.hpp"

#include "security/Key.hpp"


#include <QScrollBar>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QProcess>



HubWindow::HubWindow(Hub *hub, QWidget *parent) :
	QMainWindow(parent)
  , ui(new Ui::HubWindow)
  , hub(hub)

  , m_gait(0)
{
	setObjectName("HubWindow");
	if(nullptr!=hub){
		restoreGeometry(hub->settings().getCustomSettingByteArray("window.geometry"));
		QSplashScreen *splash=new QSplashScreen(this, QPixmap(":/images/hub_butterfly.svg"), Qt::WindowStaysOnTopHint);
		splash->show();
		summaryTimer.setInterval(100);
		{
			ScopedTimer setupUITimer("hub setupUI");
			ui->setupUi(this);
		}
		ui->widgetPairing->configure(hub);
		/*
	QAbstractItemModel *data = new ClientModel(hub->getComms()->getClients(), this);
	ui->widgetIncommingNodes->configure("Icons","hubwindiow-clients-list");
	ui->widgetIncommingNodes->setModel(data);
*/
		ui->tabWidget->setEnabled(true);
		ui->tabWidget->setCurrentWidget(ui->tabPairing);

		ui->tabWidgetDevelopment->setCurrentIndex(0);
		ui->tabWidgetUtilities->setCurrentIndex(0);

		ui->widgetActiveNodes->configure(hub->settings(),"active_nodes","List");
		ui->widgetIncommingNodes->configure(hub->settings(),"incomming_nodes","List");

		//Listen
		ui->comboBoxLocalAddress->configure(&hub->settings(), "hub-listen-address","Local address");
		ui->lineEditBindPort->configure(&hub->settings(), "","hub-listen-port","The port to listen for incomming netork traffic");

		ui->lineEditRemoteAddress->configure(&hub->settings(), "localhost","hub-listen_address","The address of the remote host");
		ui->lineEditRemotePort->configure(&hub->settings(), "","hub-port","The port of the remote host");
		ui->tryToggleListen->setText("Listen","Preparing...","Listening");
		if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onListenStateChanged(TryToggleState)),OC_CONTYPE)){
			qDebug()<<"could not connect";
		}
		if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)){
			qDebug()<<"could not connect";
		}


		const QCommandLineParser &opts=hub->options();
		if(opts.isSet("local-port")){
			ui->lineEditBindPort->setText(opts.value("local-port"));
			qDebug()<<"OVERRIDING LOCAL PORT WITH VALUE FROM CMDLINE: "<<opts.value("local-port");
		}
		if(opts.isSet("remote-port")){
			ui->lineEditRemotePort->setText(opts.value("remote-port"));
			qDebug()<<"OVERRIDING REMOTE PORT WITH VALUE FROM CMDLINE: "<<opts.value("remote-port");
		}
		ui->lineEditBindPort->setValidator( new QIntValidator(0, 65535, this) );
		ui->lineEditRemotePort->setValidator( new QIntValidator(0, 65535, this) );

		hub->comms()->hookSignals(*this);

		QByteArray ba("HELLO WORLD");
		QHexEditData* hexdata = QHexEditData::fromMemory(ba);

		ui->hexEditor->setData(hexdata);

		m_gait=new GaitController<qreal> ();
		if(0!=m_gait){
			ui->widgetGait->setGait(*m_gait);
		}


		appendLog("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure("hub.plan");

		//updateClientsList();
		appendLog("READY");
		QTimer::singleShot(1500,splash,SLOT(deleteLater()));
		splash=nullptr;
	}
}

HubWindow::~HubWindow()
{
	if(nullptr!=hub){
		hub->settings().setCustomSettingByteArray("window.geometry", saveGeometry());
		hub->comms()->unHookSignals(*this);
	}
	delete ui;
	delete m_gait;
}



/////////////////////////////




void HubWindow::onSummaryTimer()
{
	CommsChannel *comms=hub->comms();
	if(0==comms){
		ui->plainTextEditSummary->setPlainText("N/A");
	}
	else{

	}
}

void HubWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}

void HubWindow::onListenStateChanged(TryToggleState s)
{
	ui->lineEditBindPort->setEnabled(OFF==s);
	ui->comboBoxLocalAddress->setEnabled(OFF==s);
	ui->pushButtonSendData->setEnabled(ON==s);
	const bool on=ON==s;
	if(!on){
		ui->tabWidget->setCurrentIndex(0);
	}
	//ui->tabWidget->setEnabled(on);
	on?summaryTimer.start():summaryTimer.stop();
	appendLog("New listening state: "+ToggleStateToSTring(s));
	if(TRYING==s){
		QHostInfo::lookupHost("localhost",this, SLOT(onLocalHostLookupComplete(QHostInfo)));
	}
	else if(OFF==s){
		CommsChannel *comms=hub->comms();
		if(0!=comms){
			comms->stop();
		}
	}
}



void HubWindow::onLocalHostLookupComplete(QHostInfo hi)
{
	for(QHostAddress adr:hi.addresses()){
		if(adr.isNull()){
			ui->logScroll->appendLog("Skipping invalid address during local host lookup: "+adr.toString());
		}
		else{
			CommsChannel *comms=hub->comms();
			if(0!=comms){
				qDebug()<<"HUB comms start for " << adr.toString()<<":" << ui->lineEditBindPort->text();
				comms->start(NetworkAddress(adr, ui->lineEditBindPort->text().toInt()));
				ui->tryToggleListen->setState(ON);
			}
			return;
		}
	}
	appendLog("No valid addresses found during host lookup, aborting bind");
	ui->tryToggleListen->setState(OFF);
}


void HubWindow::onRemoteHostLookupComplete(QHostInfo hi)
{
	qDebug()<<"## onRemoteHostLookupComplete";
	for(QHostAddress adr:hi.addresses()){
		if(adr.isNull()){
			ui->logScroll->appendLog("Skipping invalid address during remote host lookup: "+adr.toString());
		}
		else{
			const int l=ui->horizontalSliderSendCount->value();
			const int sz=int(500*ui->horizontalSliderSendSize->value())/100;
			for(int i=0;i<l;++i){
				QByteArray ba=("PING "+QString::number(i)+"/"+QString::number(l)).toUtf8()+" ";
				const int sz2=sz-ba.size();
				for(int i=0;i<sz2;++i){
					ba.append("X");
				}
				//				quint64 port=ui->lineEditRemotePort->text().toInt();
				//			CommsChannel *comms=hub->getComms();
				//TODO: convert this to use couriers

				//	comms->sendPackage(ba,adr,port);
			}
			appendLog( "SENDING "+QString::number(l)+" DATA PACKETS OF SIZE "+QString::number(sz)+" to "+ui->lineEditRemoteAddress->text()+ "("+adr.toString()+"):"+ui->lineEditRemotePort->text());
			return;
		}
	}
	ui->logScroll->appendLog("No valid addresses found during remote host lookup, aborting send");
}


void HubWindow::onError(QString msg)
{
	if("Unable to send a message"==msg){

	}
	else{
		appendLog("GOT ERROR: "+msg);
	}
}



void HubWindow::onClientAdded(Client *c)
{
	if(0!=c){
		appendLog("CLIENT ADDED: "+c->getSummary());
		ui->widgetIncommingNodes->update();
	}
}


void HubWindow::on_pushButtonSendData_clicked()
{
	CommsChannel *comms=hub->comms();
	if(nullptr!=comms){
		//QHostInfo::lookupHost(ui->lineEditRemoteAddress->text(),this, SLOT(onRemoteHostLookupComplete(QHostInfo)));
		QByteArray ba;
		ba="HELLO WORLD";
		quint64 w=comms->sendRawData(ba,ClientSignature(hub->keyStore().localKey().id(),NetworkAddress(QHostAddress(ui->lineEditRemoteAddress->text()),ui->lineEditRemotePort->text().toInt())));
		qDebug()<<"Wrote "<<w<<" bytes raw helloworld packet";
	}
	else{
		appendLog("NOT READY TO SEND DATA");
	}
}





void HubWindow::startProcess(QString base)
{
	const QString program = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath()+"/../"+base+"/"+base;
	qDebug()<<"Starting process: "<<program;
	QStringList arguments;
	//arguments << "-style" << "fusion";
	QProcess *proc = new QProcess(this);
	proc->setProcessChannelMode(QProcess::ForwardedChannels);
	proc->start(program, arguments);
	//TODO: make async
	proc->waitForStarted();
	if(QProcess::Running!=proc->state()){
		qDebug()<<"ERROR could not start: "<<proc->errorString();
		proc->deleteLater();
		proc=0;
	}
	else{
		qDebug()<<"A-OK";
	}

}

void HubWindow::on_comboBoxAddLocal_currentIndexChanged(const QString &arg1){
	if("Remote"==arg1){
		//TODO: synthesize arguments object that points to parent hub
		startProcess("remote");

	}
	else if("Agent"==arg1){
		//TODO: synthesize arguments that points to parent hub
		startProcess("agent");
	}
	ui->comboBoxAddLocal->setCurrentIndex(0);
}

void HubWindow::on_tabWidget_currentChanged(int)
{

}


void HubWindow::onConnectionStatusChanged(bool s)
{
	qDebug()<<"connection state changed: "<<s;

}


void HubWindow::on_pushButtonTest_clicked()
{
	qDebug()<<"TEST BUTTON PRESSED";
}



void HubWindow::on_lineEditQR_textChanged(const QString &text)
{
	ui->widgetQR->setQRData(text);
}

void HubWindow::on_pushButtonUDPPunch_clicked()
{
	if(nullptr!=hub){
		ZooClient *zoo=hub->zooClient();
		if(nullptr!=zoo){
			zoo->punchUDP("12345");
		}
	}
}

void HubWindow::on_pushButtonRandomIdenticonID_clicked()
{
	OC_METHODGATE();
	QString id=Key::hash(QString::number(qrand()));
	ui->lineEditIdenticonID->setText(id);
}

void HubWindow::on_lineEditIdenticonID_textChanged(const QString &arg1)
{
	OC_METHODGATE();
	QString id=ui->lineEditIdenticonID->text();
	PortableID pid;
	pid.setName("Dummy Identicon");
	pid.setID(id);
	pid.setType(TYPE_AGENT);
	ui->widgetIdenticonAgent->setPortableID(pid);
	pid.setType(TYPE_REMOTE);
	ui->widgetIdenticonRemote->setPortableID(pid);
	pid.setType(TYPE_HUB);
	ui->widgetIdenticonHub->setPortableID(pid);
}
