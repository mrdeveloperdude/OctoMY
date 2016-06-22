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
  , hexy(0)
  , m_gait(0)
{
	setObjectName("HubWindow");
	if(nullptr!=hub){
		restoreGeometry(hub->getSettings().getCustomSettingByteArray("window.geometry"));
		QSplashScreen *splash=new QSplashScreen(this, QPixmap(":/images/hub_butterfly.svg"), Qt::WindowStaysOnTopHint);
		splash->show();
		summaryTimer.setInterval(100);
		hexyTimer.setSingleShot(true);
		hexyTimer.setTimerType(Qt::PreciseTimer);
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




		ui->widgetActiveNodes->configure(hub->getSettings(),"active_nodes","List");
		ui->widgetIncommingNodes->configure(hub->getSettings(),"incomming_nodes","List");

		//Listen
		ui->comboBoxLocalAddress->configure(&hub->getSettings(), "hub-listen-address","Local address");
		ui->lineEditBindPort->configure(&hub->getSettings(), "","hub-listen-port","The port to listen for incomming netork traffic");

		ui->lineEditRemoteAddress->configure(&hub->getSettings(), "localhost","hub-listen_address","The address of the remote host");
		ui->lineEditRemotePort->configure(&hub->getSettings(), "","hub-port","The port of the remote host");
		ui->tryToggleListen->setText("Listen","Preparing...","Listening");
		if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onListenStateChanged(TryToggleState)),OC_CONTYPE)){
			qDebug()<<"could not connect";
		}
		if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)){
			qDebug()<<"could not connect";
		}

		if(!connect(&hexyTimer,SIGNAL(timeout()),this,SLOT(onHexyTimer()))){
			qDebug()<<"could not connect";
		}
		QCommandLineParser &opts=hub->getOptions();
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

		hub->getComms()->hookSignals(*this);

		QByteArray ba("HELLO WORLD");
		QHexEditData* hexdata = QHexEditData::fromMemory(ba);

		ui->hexEditor->setData(hexdata);

		HexyTool *ht=ui->widgetHexyTool;
		if(0==hexy){
			hexy=new HexySerial;
			if(0!=hexy){
				connect( hexy, SIGNAL(settingsChanged()), this, SLOT(onHexySettingsChanged()));
			}
		}


		m_gait=new GaitController<qreal> ();
		if(0!=m_gait){
			ui->widgetGait->setGait(*m_gait);
		}


		appendLog("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure("hub.plan");

		//updateClientsList();
		appendLog("READY");
		QTimer::singleShot(2500,splash,SLOT(deleteLater()));
		splash=nullptr;
	}
}

HubWindow::~HubWindow() {
	if(nullptr!=hub){
		hub->getSettings().setCustomSettingByteArray("window.geometry", saveGeometry());
		hub->getComms()->unHookSignals(*this);
	}
	delete ui;
	delete m_gait;
}



/////////////////////////////




void HubWindow::onSummaryTimer(){
	CommsChannel *comms=hub->getComms();
	if(0==comms){
		ui->plainTextEditSummary->setPlainText("N/A");
	}
	else{

	}
}

void HubWindow::appendLog(const QString& text){
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}

void HubWindow::onListenStateChanged(TryToggleState s){
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
		CommsChannel *comms=hub->getComms();
		if(0!=comms){
			comms->stop();
		}
	}
}



void HubWindow::onLocalHostLookupComplete(QHostInfo hi){
	for(QHostAddress adr:hi.addresses()){
		if(adr.isNull()){
			ui->logScroll->appendLog("Skipping invalid address during local host lookup: "+adr.toString());
		}
		else{
			CommsChannel *comms=hub->getComms();
			if(0!=comms){
				qDebug()<<"HUB comms start for " << adr.toString()<<":" << ui->lineEditBindPort->text();
				comms->start(adr, ui->lineEditBindPort->text().toInt());
				ui->tryToggleListen->setState(ON);
			}
			return;
		}
	}
	appendLog("No valid addresses found during host lookup, aborting bind");
	ui->tryToggleListen->setState(OFF);
}


void HubWindow::onRemoteHostLookupComplete(QHostInfo hi){
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



void HubWindow::onReceivePacket(QSharedPointer<QDataStream> ds, QHostAddress host, quint16 port){
	ui->logScroll->appendLog("GOT packet from "+host.toString()+":"+QString::number(port));
	qint32 mt=INVALID;
	*ds >> mt;
	switch((MessageType)mt){
		//Implemented
		case(STATUS):{
				SensorsMessage sm(ds);
			}break;
			//Not implemented yet
		case(QUERY):
		case(QUERY_RESULT):
		case(COMMAND):
			{
				qDebug()<<"WARNING: Handler for message type "<<mt<<" not implemented yet";
			}break;
			//Unknown/errors
		default:

		case(INVALID):{
				qDebug()<<"ERROR: Unknown message type found: "<<mt;
			}break;
	}
}

void HubWindow::onError(QString msg){
	if("Unable to send a message"==msg){

	}
	else{
		appendLog("GOT ERROR: "+msg);
	}
}



void HubWindow::onClientAdded(Client *c){
	if(0!=c){
		appendLog("CLIENT ADDED: "+c->getSummary());
		ui->widgetIncommingNodes->update();
	}
}


void HubWindow::on_pushButtonSendData_clicked(){
	CommsChannel *comms=hub->getComms();
	if(0!=comms){
		//QHostInfo::lookupHost(ui->lineEditRemoteAddress->text(),this, SLOT(onRemoteHostLookupComplete(QHostInfo)));
		QByteArray ba;
		ba="HELLO WORLD";
		quint64 w=comms->sendRawData(ba,ClientSignature(0,0,QHostAddress(ui->lineEditRemoteAddress->text()),ui->lineEditRemotePort->text().toInt()));
		qDebug()<<"Wrote "<<w<<" bytes raw helloworld packet";
	}
	else{
		appendLog("NOT READY TO SEND DATA");
	}
}


void HubWindow::on_pushButtonShowStats_clicked(){

}



void HubWindow::startProcess(QString base){
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

void HubWindow::on_tabWidget_currentChanged(int){

}





void HubWindow::onHexyTimer(){
	if(0==hexy){
		return;
	}
	if(0!=m_gait){
		m_gait->update();
	}
	qreal pos[HexySerial::SERVO_COUNT]={0.0f};
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 ival=now- lastTime;
	if(ival>0){
		lastTime=now;
		angle+=(((qreal)ival)/1000.0);
		angle-=floor(angle);
		for(quint32 i=0;i<HexySerial::SERVO_COUNT;++i){
			pos[i]=sin(angle*M_PI*2.0+(qreal)i*3.0);
			if(0==i){
				//qDebug()<<"SERVO "<<i<<": "<<pos[i];
			}
		}
		hexy->move(pos,0b00000000000000000000000000000011);
	}
	hexyTimer.start();
}

void HubWindow::onHexySettingsChanged(){
	hexyTimer.start();
}

void HubWindow::onConnectionStatusChanged(bool s){
	qDebug()<<"connection state changed: "<<s;

}


void HubWindow::on_pushButtonTest_clicked(){
	qDebug()<<"TEST BUTTON PRESSED";
}



void HubWindow::on_lineEditQR_textChanged(const QString &text){
	ui->widgetQR->setQRData(text);
}

void HubWindow::on_pushButtonUDPPunch_clicked()
{
	if(nullptr!=hub){
		ZooClient *zoo=hub->getZooClient();
		if(nullptr!=zoo){
			zoo->punchUDP("12345");
		}
	}
}
