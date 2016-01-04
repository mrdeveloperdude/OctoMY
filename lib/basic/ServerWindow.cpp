#include "ServerWindow.hpp"
#include "ui_ServerWindow.h"
#include "basic/Standard.hpp"
#include "map/mapcontrol.h"
#include "map/osmmapadapter.h"
#include "map/openaerialmapadapter.h"
#include "map/maplayer.h"
#include "map/linestring.h"
#include "comms/Client.hpp"

#include <QScrollBar>
#include <QHostInfo>
#include <QNetworkInterface>

ServerWindow::ServerWindow(QCommandLineParser &opts, Server *server, QWidget *parent) :
	QMainWindow(parent)
  , cs(server)
  , ui(new Ui::ServerWindow)
  , summaryTimer(this)
  , opts(opts)

{
	summaryTimer.setInterval(100);
	ui->setupUi(this);
	ui->lineEditLocalPort->configure("1234","local_port","The port of the local host");
	ui->lineEditRemoteAddress->configure("localhost","remote_address","The address of the remote host");
	ui->lineEditRemotePort->configure("1234","remote_port","The port of the remote host");
	ui->tryToggleListen->setText("Connect","Disconnect...","Disconnect");
	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState)),this,SLOT(onListenStateChanged(TryToggleState)),WWCONTYPE)){
		qDebug()<<"could not connect";
	}
	if(!connect(&summaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),WWCONTYPE)){
		qDebug()<<"could not connect";
	}

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


	qRegisterMetaType<QHostAddress>("QHostAddress");
	if(0!=cs){
		if(!connect(cs,SIGNAL(receivePacket(QByteArray,QHostAddress,quint16)),this,SLOT(onReceivePacket(QByteArray,QHostAddress,quint16)),WWCONTYPE)){
			qDebug()<<"could not connect";
		}
		if(!connect(cs,SIGNAL(error(QString)),this,SLOT(onError(QString)),WWCONTYPE)){
			qDebug()<<"could not connect";
		}
	}


	prepareMap();
	prepareLocalAddresses();
	ui->listViewClients->setModel(&simClients);
	ui->tabWidget->setCurrentIndex(0);
	//updateClientsList();
	appendLog("READY");
}

ServerWindow::~ServerWindow() {
	delete ui;
}



/////////////////////////////

#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QGraphicsEffect>


QImage tint(QImage src, QColor color, qreal strength=1.0){
	if(src.isNull()) return QImage();
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(src));
	QGraphicsColorizeEffect effect;
	effect.setColor(color);
	effect.setStrength(strength);
	item.setGraphicsEffect(&effect);
	scene.addItem(&item);
	QImage res(src);
	QPainter ptr(&res);
	scene.render(&ptr, QRectF(), src.rect() );
	return res;
}



void ServerWindow::updateClientsList(){
	simClients.clear();
	QMap<quint64, Client *> &clients=cs->getClients();
	int n=0;
	for (QMap<quint64, Client *>::iterator i = clients.begin(), e=clients.end(); i != e; ++i){
		const Client *nl=i.value();
		if(0!=nl){
			QStandardItem *newItem = new QStandardItem;
			newItem->setText(nl->getListText());
			QVariant v = qVariantFromValue((void *) nl);
			newItem->setData(v);
			QIcon ic(QPixmap::fromImage(tint(QImage("://icons/spider.svg"),nl->connected?QColor(0,192,0):QColor(192,0,0),1.0)));
			newItem->setIcon(ic);
			simClients.setItem(n++,newItem);
		}
	}
}

void ServerWindow::prepareMap(){
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



void ServerWindow::prepareLocalAddresses(){
	ui->comboBoxIP->clear();
	QList<QNetworkInterface> list= QNetworkInterface::allInterfaces();
	for(int i=0; i<list.size();i++) {
		QNetworkInterface inter = list.at(i);
		if (inter.flags().testFlag(QNetworkInterface::IsUp) && !inter.flags().testFlag(QNetworkInterface::IsLoopBack)) {
			QList<QNetworkAddressEntry> list2= inter.addressEntries();
			for(int j=0; j<list2.size();j++) {
				QNetworkAddressEntry entry = list2.at(j);
				ui->comboBoxIP->addItem(entry.ip().toString());
			}
		}
	}
}


void ServerWindow::homeMap() {
	if(0!=mc){
		qDebug()<<"HOME";
		QList<QPointF> londalen;
		londalen <<QPointF(5.452844, 60.385883);
		londalen <<QPointF(5.457945, 60.380353);
		mc->setViewAndZoomIn ( londalen) ;
	}
}

void ServerWindow::onSummaryTimer(){
	if(0==cs){
		ui->plainTextEditSummary->setPlainText("N/A");
	}
	else{
		updateClientsList();
		ui->plainTextEditSummary->setPlainText(cs->getSummary());
		const ReliabilitySystem rs;//=cs->getReliabilitySystem();
		const int sentPackets=rs.sentPackets()-lastSentPackets;
		const int receivedPackets=rs.receivedPackets()-lastReceivedPackets;
		const int lostPackets=rs.lostPackets()-lastLostPackets;
		const int ackedPackets=rs.ackedPackets()-lastAckedPackets;
		lastSentPackets+=sentPackets;
		lastReceivedPackets+=receivedPackets;
		lastLostPackets+=lostPackets;
		lastAckedPackets+=ackedPackets;
		appendGraphData(rs.roundTripTime(), sentPackets, receivedPackets, lostPackets, ackedPackets, rs.sentBandwidth(), rs.ackedBandwidth());
	}
}

void ServerWindow::appendLog(const QString& text){
	WWMETHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
}

void ServerWindow::onListenStateChanged(TryToggleState s){
	ui->lineEditLocalPort->setEnabled(OFF==s);
	ui->pushButtonSendData->setEnabled(ON==s);
	ON==s?summaryTimer.start():summaryTimer.stop();
	appendLog("New connection state: "+ToggleStateToSTring(s));
	if(TRYING==s){
		QHostInfo::lookupHost("localhost",this, SLOT(onLocalHostLookupComplete(QHostInfo)));
	}
	else if(OFF==s){
		if(0!=cs){
			cs->stop();
		}
	}
}



void ServerWindow::onLocalHostLookupComplete(QHostInfo hi){
	for(QHostAddress adr:hi.addresses()){
		if(adr.isNull()){
			ui->logScroll->appendHtml("Skipping invalid address during local host lookup: "+adr.toString());
		}
		else{
			if(0!=cs){
				cs->start(adr, ui->lineEditLocalPort->text().toInt());
				ui->tryToggleListen->setState(ON);
			}
			return;
		}
	}
	appendLog("No valid addresses found during host lookup, aborting bind");
	ui->tryToggleListen->setState(OFF);
}

void ServerWindow::onRemoteHostLookupComplete(QHostInfo hi){
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
				cs->sendPackage(ba,adr,port);
			}
			appendLog( "SENDING "+QString::number(l)+" DATA PACKETS OF SIZE "+QString::number(sz)+" to "+ui->lineEditRemoteAddress->text()+ "("+adr.toString()+"):"+ui->lineEditRemotePort->text());
			return;
		}
	}
	ui->logScroll->appendHtml("No valid addresses found during remote host lookup, aborting send");
}


void ServerWindow::onReceivePacket(QByteArray ba, QHostAddress host, quint16 port){
	ui->logScroll->appendHtml("GOT DATA: '"+ba+"'' from "+host.toString()+":"+QString::number(port));
}

void ServerWindow::onError(QString msg){
	if("Unable to send a message"==msg){

	}
	else{
		appendLog("GOT ERROR: "+msg);
	}
}

void ServerWindow::on_pushButtonSendData_clicked(){
	if(0!=cs){
		QHostInfo::lookupHost(ui->lineEditRemoteAddress->text(),this, SLOT(onRemoteHostLookupComplete(QHostInfo)));
	}
	else{
		appendLog("NOT READY TO SEND DATA");
	}
}


void ServerWindow::on_pushButtonShowStats_clicked(){

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
void ServerWindow::appendGraphData(double rtt, int sent,int received,int lost,int acked, float sendBW,float receiveBW){
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

#include "ClientWindow.hpp"

void ServerWindow::on_listViewClients_clicked(const QModelIndex &index){
	QStandardItem *item=simClients.itemFromIndex(index);
	if(0!=item){
		Client *c=(Client *)item->data().value<void *>();
		if(0!=c){
			appendLog("OPENING CLINET "+c->getListText());
			ClientWindow *cw=new ClientWindow(c,0);
			cw->show();
		}
		else{
			appendLog("could not find client for item");
		}
	}
	else{
		appendLog("could not find item");
	}

}
