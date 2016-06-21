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
  , startTime(QDateTime::currentMSecsSinceEpoch())
  , randomWalk(0.0)
  , hexy(0)
  , m_gait(0)
{
	setObjectName("HubWindow");
	if(nullptr!=hub){
		summaryTimer.setInterval(100);
		gaugeTimer.setInterval(1000/60);
		gaugeTimer.setTimerType(Qt::PreciseTimer);
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
		ui->tabWidget->setCurrentWidget(ui->tabIncomming);
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
		if(!connect(&gaugeTimer,SIGNAL(timeout()),this,SLOT(onGaugeTimer()),OC_CONTYPE)){
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

		ui->tabWidget->setCurrentWidget(ui->tabPlan);




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
	}
}

HubWindow::~HubWindow() {
	hub->getComms()->unHookSignals(*this);
	delete ui;
	delete m_gait;
}



/////////////////////////////





void HubWindow::onGaugeTimer(){
	const float cmsse=QDateTime::currentMSecsSinceEpoch()-startTime;
	const float ms = cmsse/1000.0f;

	float alpha     =  0.0f;
	float beta      =  0.0f;
	float roll      =  0.0f;
	float pitch     =  0.0f;
	float heading   =  0.0f;
	float slipSkid  =  0.0f;
	float turnRate  =  0.0f;
	float devH      =  0.0f;
	float devV      =  0.0f;
	float airspeed  =  0.0f;
	float altitude  =  0.0f;
	float pressure  = 28.0f;
	float climbRate =  0.0f;
	float machNo    =  0.0f;
	float adf       =  0.0f;
	float dme       =  0.0f;


	alpha     =   20.0f * sin( ms /  10.0f );
	beta      =   15.0f * sin( ms /  10.0f );
	roll      =  180.0f * sin( ms /  10.0f );
	pitch     =   90.0f * sin( ms /  20.0f );
	heading   =  360.0f * sin( ms /  40.0f );
	slipSkid  =    1.0f * sin( ms /  10.0f );
	turnRate  =    7.0f * sin( ms /  10.0f );
	devH      =    1.0f * sin( ms /  20.0f );
	devV      =    1.0f * sin( ms /  20.0f );
	airspeed  =  125.0f * sin( ms /  40.0f ) +  125.0f;
	altitude  = 9000.0f * sin( ms /  40.0f ) + 9000.0f;
	pressure  =    2.0f * sin( ms /  20.0f ) +   30.0f;
	climbRate =  650.0f * sin( ms /  20.0f );
	machNo    = airspeed / 650.0f;
	adf       = -360.0f * sin( ms /  50.0f );
	dme       =   99.0f * sin( ms / 100.0f );

	ui->widgetPFD->setFlightPathMarker ( alpha, beta );
	ui->widgetPFD->setRoll          ( roll     );
	ui->widgetPFD->setPitch         ( pitch     );
	ui->widgetPFD->setSlipSkid      ( slipSkid  );
	ui->widgetPFD->setTurnRate      ( turnRate / 6.0f );
	ui->widgetPFD->setDevH          ( devH      );
	ui->widgetPFD->setDevV          ( devV      );
	ui->widgetPFD->setHeading       ( heading   );
	ui->widgetPFD->setAirspeed      ( airspeed  );
	ui->widgetPFD->setMachNo        ( machNo    );
	ui->widgetPFD->setAltitude      ( altitude  );
	ui->widgetPFD->setPressure      ( pressure  );
	ui->widgetPFD->setClimbRate     ( climbRate / 100.0f );

	ui->widgetNAV->setHeading    ( heading   );
	ui->widgetNAV->setHeadingBug ( 0.0f );
	ui->widgetNAV->setCourse     ( 0.0f );
	ui->widgetNAV->setBearing    ( adf  , true );
	ui->widgetNAV->setDeviation  ( devH , true );
	ui->widgetNAV->setDistance   ( dme  , true );

	ui->widgetSix->setRoll      ( roll      );
	ui->widgetSix->setPitch     ( pitch     );
	ui->widgetSix->setAltitude  ( altitude  );
	ui->widgetSix->setPressure  ( pressure  );
	ui->widgetSix->setAirspeed  ( airspeed  );
	ui->widgetSix->setHeading   ( heading   );
	ui->widgetSix->setSlipSkid  ( slipSkid * 15.0f );
	ui->widgetSix->setTurnRate  ( turnRate  );
	ui->widgetSix->setClimbRate ( climbRate );
	if(ui->tabWidgetAviation->currentWidget()==ui->tabPFD){
		ui->widgetPFD->update();
	}
	if(ui->tabWidgetAviation->currentWidget()==ui->tabNAV){
		ui->widgetNAV->update();
	}
	if(ui->tabWidgetAviation->currentWidget()==ui->tabSix){
		ui->widgetSix->update();
	}
}

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

	stats.addGraph("RTT",graphRTT,QPen(QBrush( QColor(0,192,128)), 2, Qt::SolidLine));
	stats.addGraph("Sent",graphSent, QPen(QBrush(QColor(192,128,0)), 2, Qt::SolidLine));
	stats.addGraph("Received",graphReceived, QPen(QBrush(QColor(128,0,192)), 2, Qt::DashLine));

	stats.addGraph("Acked",graphAcked,QPen(QBrush(QColor(0,192,0)), 2, Qt::DotLine));
	stats.addGraph("Lost",graphLost, QPen(QBrush(QColor(192,0,0)), 2, Qt::SolidLine));
	stats.addGraph("Send BW",graphSendBW,QPen(QBrush(QColor(192,0,128)), 2, Qt::SolidLine));
	stats.addGraph("Receive BW",graphReceiveBW,QPen(QBrush(QColor(0,128,192)), 2, Qt::SolidLine));
	stats.show();
}



/*
	graphRTT.clear();
	float randomWalk=0.0f;
	int n=100;
	for(int i=0;i<n;++i){
		graphRTT[i]=QCPData(i,randomWalk);
		randomWalk+=(qrand()/(float)RAND_MAX-0.5f)*0.1f;
	}
	stats.addGraph("Random data",graphRTT);

 */
void HubWindow::appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW){
	double now=QDateTime::currentMSecsSinceEpoch()/1000.0;
	graphRTT[now]=QCPData(now,rtt);
	graphSent[now]=QCPData(now,sent);
	graphReceived[now]=QCPData(now,received);
	graphLost[now]=QCPData(now,lost);
	graphAcked[now]=QCPData(now,acked);
	graphSendBW[now]=QCPData(now,sendBW);
	graphReceiveBW[now]=QCPData(now,receiveBW);
	stats.repaintPlot();
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
	ui->tabWidget->currentWidget()==ui->tabAviation?gaugeTimer.start():gaugeTimer.stop();
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

#include "../libpki/qpolarsslpki.hpp"

void HubWindow::on_pushButtonGenerateKeyPair_clicked(){
	qpolarssl::Pki pki;
	int ret=0;
	ret=pki.generateKeyPair(2048);
	if(0==ret){
		QByteArray key=pki.getPEMKey();
		QByteArray pubkey=pki.getPEMPubkey();
		ui->plainTextEditPrivateKey->setPlainText(key);
		ui->plainTextEditPublicKey->setPlainText(pubkey);
	}
	else{
		qWarning("ERROR Exporting generated key as PEM");
	}

}

void HubWindow::on_pushButtonTestKeyPair_clicked(){
	//const auto priPath = QString("/home/lennart/keypairs/private_key.pem"); 	const auto pubPath = QString("/home/lennart/keypairs/public_key.pem"); utility::fileToByteArray(pubPath);
	int ret=0;
	qpolarssl::Pki pkiPrivate;
	QByteArray priKeyData  = ui->plainTextEditPrivateKey->toPlainText().toUtf8();
	if( (ret=pkiPrivate.parseKey(priKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of private key failed with code "+QString::number(ret));
		msgBox.exec();
		return;
	}
	else if(!pkiPrivate.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of private key");
		msgBox.exec();
		return;
	}

	qpolarssl::Pki pkiPublic;
	QByteArray pubKeyData  = ui->plainTextEditPublicKey->toPlainText().toUtf8();
	if( (ret=pkiPublic.parsePublicKey(pubKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of public key failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}
	else if(!pkiPublic.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of public key");
		msgBox.exec();
		return;
	}

	const QByteArray sourceData = QString("OctoMY™ test data in cleartext").toUtf8();

	const QByteArray signature = pkiPrivate.sign(sourceData, qpolarssl::THash::SHA256);

	const int siglen=signature.length();
	if((siglen <= 64) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature size "+QString::number(siglen)+" was less than 64");
		msgBox.exec();
		return;
	}

	ret = pkiPublic.verify(sourceData, signature, qpolarssl::THash::SHA256);

	if(ret!=0){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature verification failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}

	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("Signature verification succeeded.");
	msgBox.exec();
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
