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


//#include "agent/Agent.hpp"
//#include "agent/AgentWindow.hpp"

#include <QScrollBar>
#include <QHostInfo>
#include <QNetworkInterface>



HubWindow::HubWindow(Hub *hub, QWidget *parent) :
	QMainWindow(parent)
  , ui(new Ui::HubWindow)
  , hub(hub)
  , summaryTimer(this)
{
	summaryTimer.setInterval(100);
	ui->setupUi(this);
	QAbstractItemModel *data = new ClientModel(hub->getComms()->getClients(), this);
	ui->widgetMulti->configure("Icons","hubwindiow-clients-list");
	ui->widgetMulti->setModel(data);

	ui->tabWidget->setEnabled(false);
	ui->tabWidget->setCurrentWidget(ui->tabIncomming);
	ui->lineEditLocalPort->configure("","local_port","The port of the local host");
	ui->lineEditRemoteAddress->configure("localhost","remote_address","The address of the remote host");
	ui->lineEditRemotePort->configure("","remote_port","The port of the remote host");
	ui->tryToggleListen->setText("Listen","Preparing...","Listening");
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onListenStateChanged(TryToggleState)),WWCONTYPE)){
		qDebug()<<"could not connect";
	}
	if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),WWCONTYPE)){
		qDebug()<<"could not connect";
	}

	QCommandLineParser &opts=hub->getOptions();
	if(opts.isSet("local-port")){
		ui->lineEditLocalPort->setText(opts.value("local-port"));
		qDebug()<<"OVERRIDING LOCAL PORT WITH VALUE FROM CMDLINE: "<<opts.value("local-port");
	}
	if(opts.isSet("remote-port")){
		ui->lineEditRemotePort->setText(opts.value("remote-port"));
		qDebug()<<"OVERRIDING REMOTE PORT WITH VALUE FROM CMDLINE: "<<opts.value("remote-port");
	}
	ui->lineEditLocalPort->setValidator( new QIntValidator(0, 65535, this) );
	ui->lineEditRemotePort->setValidator( new QIntValidator(0, 65535, this) );

	hub->getComms()->hookSignals(*this);

	prepareMap();
	utility::populateComboboxWithLocalAdresses(*ui->comboBoxIP);
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
	ui->lineEditLocalPort->setEnabled(OFF==s);
	ui->pushButtonSendData->setEnabled(ON==s);
	const bool on=ON==s;
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
				qDebug()<<"HUB comms start for " << adr.toString()<<":" << ui->lineEditLocalPort->text();
				comms->start(adr, ui->lineEditLocalPort->text().toInt());
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
				quint64 port=ui->lineEditRemotePort->text().toInt();
				CommsChannel *comms=hub->getComms();
				comms->sendPackage(ba,adr,port);
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
		ui->widgetMulti->update();
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

void HubWindow::on_toolButtonAddLocalRemote_clicked(){
	//TODO: synthesize options object that points to parent hub
	Remote *remote=new Remote(hub->getOptions());
	RemoteWindow *w=new RemoteWindow (remote,0);
	//		remote->setLogOutput(&w);
	w->show();
}

void HubWindow::on_toolButtonAddLocalAgent_clicked(){
	/*
	//TODO: synthesize options object that points to parent hub
	Agent *agent=new Agent(hub->getOptions());
	AgentWindow *w=new AgentWindow (agent,0);
	//		remote->setLogOutput(&w);
	w->show();
	*/
}



void HubWindow::on_horizontalSliderIdenticon_sliderMoved(int position){
	//ui->widgetIdenticon->setIdenticonData(ui->horizontalSliderIdenticon->value());
}
