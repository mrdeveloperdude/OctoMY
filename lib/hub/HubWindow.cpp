#include "HubWindow.hpp"
#include "ui_HubWindow.h"
#include "basic/Standard.hpp"
#include "map/mapcontrol.h"
#include "map/osmmapadapter.h"
#include "map/openaerialmapadapter.h"
#include "map/maplayer.h"
#include "map/linestring.h"
#include "hub/Client.hpp"
#include "comms/messages/MessageType.hpp"
#include "hub/Hub.hpp"
#include "ClientWindow.hpp"
#include "utility/Utility.hpp"
#include "models/ClientModel.hpp"


#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"


#include "agent/Agent.hpp"
#include "agent/AgentWindow.hpp"

#include <QScrollBar>
#include <QHostInfo>
#include <QNetworkInterface>



HubWindow::HubWindow(Hub *hub, QWidget *parent) :
	QMainWindow(parent)
  , ui(new Ui::HubWindow)
  , hub(hub)
  , startTime(QDateTime::currentMSecsSinceEpoch())
  , randomWalk(0.0)
  , mc(0)
  , hexy(0)

{
	summaryTimer.setInterval(100);
	gaugeTimer.setInterval(1000/60);
	gaugeTimer.setTimerType(Qt::PreciseTimer);
	hexyTimer.setSingleShot(true);
	hexyTimer.setTimerType(Qt::PreciseTimer);
	ui->setupUi(this);
	QAbstractItemModel *data = new ClientModel(hub->getComms()->getClients(), this);
	ui->widgetIncommingNodes->configure("Icons","hubwindiow-clients-list");
	ui->widgetIncommingNodes->setModel(data);

	ui->tabWidget->setEnabled(false);
	ui->tabWidget->setCurrentWidget(ui->tabIncomming);
	ui->lineEditBindPort->configure("","listen-port","The port to listen for incomming netork traffic");
	ui->lineEditRemoteAddress->configure("localhost","remote_address","The address of the remote host");
	ui->lineEditRemotePort->configure("","remote_port","The port of the remote host");
	ui->tryToggleListen->setText("Listen","Preparing...","Listening");
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onListenStateChanged(TryToggleState)),WWCONTYPE)){
		qDebug()<<"could not connect";
	}
	if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),WWCONTYPE)){
		qDebug()<<"could not connect";
	}

	if(!connect(&gaugeTimer,SIGNAL(timeout()),this,SLOT(onGaugeTimer()),WWCONTYPE)){
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

	prepareMap();
	utility::populateComboboxWithLocalAdresses(*ui->comboBoxBindAddress);
	ui->tabWidget->setCurrentIndex(0);
	//updateClientsList();
	appendLog("READY");


}

HubWindow::~HubWindow() {
	hub->getComms()->unHookSignals(*this);
	delete ui;
}



/////////////////////////////



void HubWindow::prepareMap(){
	mc=ui->widgetMap;
	if(0!=mc){
		mc->showScale(true);
		QDir dir("map.cache/");
		if (!dir.exists()) dir.mkpath(".");
		mc->enablePersistentCache ( dir,8192);
		// create MapAdapter to get maps from
		//		qmapcontrol::MapAdapter* mapadapter = new qmapcontrol::OSMMapAdapter();
		//qmapcontrol::MapAdapter* mapadapter = new qmapcontrol::OpenAerialMapAdapter();
		qmapcontrol::TileMapAdapter* mapadapter = new qmapcontrol::TileMapAdapter("tile.openstreetmap.org", "/%1/%2/%3.png", 256, 0, 17);

		// create a map layer with the mapadapter
		qmapcontrol::Layer* l = new qmapcontrol::MapLayer("Custom Layer", mapadapter);
		// add Layer to the MapControl
		mc->addLayer(l);

		// create a LineString
		QList<qmapcontrol::Point*> points;

		points.append(new qmapcontrol::Point( 5.456635,60.384571, "HQ"));
		points.append(new qmapcontrol::Point(  5.456249,60.384317, "Launch point"));

		// A QPen also can use transparency
		QPen* linepen = new QPen(QColor(0, 0, 255, 100));
		linepen->setWidth(5);
		// Add the Points and the QPen to a LineString
		qmapcontrol::LineString* ls = new qmapcontrol::LineString(points, "Launch vector", linepen);

		// Add the LineString to the layer
		l->addGeometry(ls);
		homeMap();
	}
}





void HubWindow::homeMap() {
	if(0!=mc){
		qDebug()<<"HOME";
		QList<QPointF> londalen;
		londalen <<QPointF(5.452844, 60.385883);
		londalen <<QPointF(5.457945, 60.380353);
		mc->setViewAndZoomIn ( londalen) ;
	}
}



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
	WWMETHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
}

void HubWindow::onListenStateChanged(TryToggleState s){
	ui->lineEditBindPort->setEnabled(OFF==s);
	ui->comboBoxBindAddress->setEnabled(OFF==s);
	ui->pushButtonSendData->setEnabled(ON==s);
	const bool on=ON==s;
	if(!on){
		ui->tabWidget->setCurrentIndex(0);
	}
	ui->tabWidget->setEnabled(on);
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
			ui->logScroll->appendHtml("Skipping invalid address during local host lookup: "+adr.toString());
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
	for(QHostAddress adr:hi.addresses()){
		if(adr.isNull()){
			ui->logScroll->appendHtml("Skipping invalid address during remote host lookup: "+adr.toString());
		}
		else{
			const int l=ui->horizontalSliderSendCount->value();
			const int sz=(500*ui->horizontalSliderSendSize->value())/100;
			for(int i=0;i<l;++i){
				QByteArray ba=("PING "+QString::number(i)+"/"+QString::number(l)).toUtf8()+" ";
				const int sz2=sz-ba.size();
				for(int i=0;i<sz2;++i){
					ba.append("X");
				}
				//				quint64 port=ui->lineEditRemotePort->text().toInt();
				//			CommsChannel *comms=hub->getComms();
				//TODO: convert this to use couriers
				//comms->sendPackage(ba,adr,port);
			}
			appendLog( "SENDING "+QString::number(l)+" DATA PACKETS OF SIZE "+QString::number(sz)+" to "+ui->lineEditRemoteAddress->text()+ "("+adr.toString()+"):"+ui->lineEditRemotePort->text());
			return;
		}
	}
	ui->logScroll->appendHtml("No valid addresses found during remote host lookup, aborting send");
}



void HubWindow::onReceivePacket(QSharedPointer<QDataStream> ds, QHostAddress host, quint16 port){
	ui->logScroll->appendHtml("GOT packet from "+host.toString()+":"+QString::number(port));
	qint32 mt=INVALID;
	*ds >> mt;
	switch((MessageType)mt){
		//Implemented
		case(STATUS):{
				StatusMessage sm(ds);
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
		QHostInfo::lookupHost(ui->lineEditRemoteAddress->text(),this, SLOT(onRemoteHostLookupComplete(QHostInfo)));
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


void HubWindow::on_comboBoxAddLocal_currentIndexChanged(const QString &arg1){
	if("Remote"==arg1){
		//TODO: synthesize options object that points to parent hub
		Remote *remote=new Remote(hub->getOptions());
		RemoteWindow *w=new RemoteWindow (remote,0);
		w->show();

	}
	else if("Agent"==arg1){
		//TODO: synthesize options object that points to parent hub
		Agent *agent=new Agent(hub->getOptions());
		AgentWindow *w=new AgentWindow (agent,0);
		w->show();
	}
	ui->comboBoxAddLocal->setCurrentIndex(0);
}

void HubWindow::on_tabWidget_currentChanged(int){
	ui->tabWidget->currentWidget()==ui->tabAviation?gaugeTimer.start():gaugeTimer.stop();
}


void HubWindow::on_pushButtonOpenModel_clicked(){
	qDebug()<<"BUTTON PRESSED";
}



void HubWindow::onHexyTimer(){
	if(0==hexy){
		return;
	}
	qreal pos[HexySerial::SERVO_COUNT]={0.0f};
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	const quint64 ival=now- lastTime;
	if(ival>0){
		lastTime=now;
		angle+=(((qreal)ival)/1000.0);
		angle-=floor(angle);
		for(int i=0;i<HexySerial::SERVO_COUNT;++i){
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

#include "hw/actuators/HexyTool.hpp"
HexyTool *ht=0;
void HubWindow::on_pushButtonTest_clicked(){
	qDebug()<<"TEST BUTTON PRESSED";
	if(0==ht){
		ht=new HexyTool(0);
	}
	ht->show();
	/*
	if(0==hexy){
		hexy=new HexySerial;
		connect( hexy, SIGNAL(settingsChanged()), this, SLOT(onHexySettingsChanged()));
	}
	else{
		delete hexy;
		hexy=0;
	}
	if(0!=hexy){
		hexy->configure();
	}
	*/
}
