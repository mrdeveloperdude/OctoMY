#include "HubWindow.hpp"

#include "ui_HubWindow.h"

#include "agent/Agent.hpp"
#include "agent/AgentWindow.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/messages/MessageType.hpp"
#include "gait/GaitController.hpp"
#include "hellocl/HelloGLCLViewRenderer.hpp"
#include "Hub.hpp"
#include "hw/controllers/servotor32/Servotor32ControllerWidget.hpp"
#include "models/ClientModel.hpp"
#include "models/TriggerListModel.hpp"
#include "node/ClientWidget.hpp"
#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"
#include "security/Key.hpp"
#include "trigger/Action.hpp"
#include "trigger/Condition.hpp"
#include "trigger/TriggerSet.hpp"
#include "utility/ScopedTimer.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include "widgets/HardwareTemplate.hpp"
#include "widgets/hexedit/QHexEditData.hpp"
#include "widgets/hexedit/QHexEdit.hpp"
#include "zbar/ZBarScanner.hpp"
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
	, mHub(hub)
	, mGait(nullptr)
	, mScanner(nullptr)
{
	OC_METHODGATE();
	setObjectName("HubWindow");
	if(nullptr!=mHub) {
		restoreGeometry(mHub->settings().getCustomSettingByteArray("window.geometry"));
		QSplashScreen *splash=new QSplashScreen(this, QPixmap(":/images/hub_butterfly.svg"), Qt::WindowStaysOnTopHint);
		splash->show();
		mSummaryTimer.setInterval(100);
		{
			//ScopedTimer setupUITimer("hub setupUI");
			ui->setupUi(this);
		}
		ui->widgetPairing->configure(mHub);
		/*
		QAbstractItemModel *data = new ClientModel(hub->getComms()->getClients(), this);
		ui->widgetIncommingNodes->configure("Icons","hubwindiow-clients-list");
		ui->widgetIncommingNodes->setModel(data);
		*/
		ui->tabWidget->setEnabled(true);
		ui->tabWidget->setCurrentWidget(ui->tabPairing);

		ui->tabWidgetDevelopment->setCurrentIndex(0);
		ui->tabWidgetUtilities->setCurrentIndex(0);

		ui->widgetActiveNodes->configure(mHub->settings(),"active_nodes","List");
		ui->widgetIncommingNodes->configure(mHub->settings(),"incomming_nodes","List");

		//Listen
		ui->comboBoxLocalAddress->configure(&mHub->settings(), "hub-listen-address","Local address");
		ui->lineEditBindPort->configure(&mHub->settings(), "","hub-listen-port","The port to listen for incomming netork traffic");

		ui->lineEditRemoteAddress->configure(&mHub->settings(), "localhost","hub-listen_address","The address of the remote host");
		ui->lineEditRemotePort->configure(&mHub->settings(), "","hub-port","The port of the remote host");
		ui->tryToggleListen->configure("Listen","Preparing...","Listening");
		if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SLOT(onListenStateChanged(const TryToggleState, const TryToggleState)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(&mSummaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}

		{
			TriggerSet *ts=new TriggerSet;
			Trigger *t1=new Trigger("Trigger #1");
			Trigger *t2=new Trigger("Trigger #2");
			Trigger *t3=new Trigger("Trigger #3");
			t1->addCondition(*new Condition("Condition #1", "Formula #1"));
			t1->addCondition(*new Condition("Condition #2", "Formula #2"));
			t1->addAction(*new Action("Action #1"));
			t1->addAction(*new Action("Action #2"));

			t2->addCondition(*new Condition("Condition #3", "Formula #3"));
			t2->addAction(*new Action("Action #3"));

			t3->addCondition(*new Condition("Condition #4", "Formula #4"));
			t3->addAction(*new Action("Action #4"));

			*ts<<t1<<t2<<t3;

			for(int i=0; i<10; ++i) {
				Trigger *t=new Trigger("Trigger #5");
				t->addCondition(*new Condition("Condition #5", "Formula #5"));
				t->addAction(*new Action("Action #5"));
				*ts<<t;
			}
			ui->widgetTriggerManager->configure(*ts);
		}

		//TODO: WOW we need to update this
//		ui->widgetActuatorControl->configure(5);
		PoseMapping *pm=new PoseMapping(5);
		ui->widgetPoseMapping->configure(*pm);

#ifdef EXTERNAL_LIB_OPENCL
		if(!connect(ui->openGLWidgetCLGLView,SIGNAL(glInitialized()),this,SLOT(onGLWidgetInitialized()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
#endif // EXTERNAL_LIB_OPENCL

		const QCommandLineParser &opts=mHub->options();
		if(opts.isSet("local-port")) {
			ui->lineEditBindPort->setText(opts.value("local-port"));
			qDebug()<<"OVERRIDING LOCAL PORT WITH VALUE FROM CMDLINE: "<<opts.value("local-port");
		}
		if(opts.isSet("remote-port")) {
			ui->lineEditRemotePort->setText(opts.value("remote-port"));
			qDebug()<<"OVERRIDING REMOTE PORT WITH VALUE FROM CMDLINE: "<<opts.value("remote-port");
		}
		ui->lineEditBindPort->setValidator( new QIntValidator(0, 65535, this) );
		ui->lineEditRemotePort->setValidator( new QIntValidator(0, 65535, this) );

		mHub->comms()->setHookCommsSignals(*this,true);

		QByteArray ba("HELLO WORLD");
		QHexEditData* hexdata = QHexEditData::fromMemory(ba);

		ui->hexEditor->setData(hexdata);

		mGait=new GaitController<qreal> ();
		if(0!=mGait) {
			ui->widgetGait->setGait(*mGait);
		}

		if(!connect(ui->widgetQR, SIGNAL(qrRedraw()), this, SLOT(onQRRedraw()),OC_CONTYPE) ) {
			qWarning()<<"ERROR: Could not connect";
		}




		appendLog("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure("hub.plan");

		//updateClientsList();
		appendLog("READY");
		QTimer::singleShot(1500,splash,SLOT(deleteLater()));
		splash=nullptr;
		updateIdentityWidgets();
	}
}

HubWindow::~HubWindow()
{
	OC_METHODGATE();
	if(nullptr!=mHub) {
		mHub->settings().setCustomSettingByteArray("window.geometry", saveGeometry());
		mHub->comms()->setHookCommsSignals(*this,false);
	}
	delete ui;
	ui=nullptr;
	delete mGait;
	mGait=nullptr;
	delete mScanner;
	mScanner=nullptr;
}



/////////////////////////////




void HubWindow::onSummaryTimer()
{
	OC_METHODGATE();
	CommsChannel *comms=mHub->comms();
	if(0==comms) {
		ui->plainTextEditSummary->setPlainText("N/A");
	} else {

	}
}


void HubWindow::onQRRedraw()
{
	OC_METHODGATE();
	if(nullptr==mScanner) {
		mScanner=new ZBarScanner();
	}
	const QPixmap &px=ui->widgetQR->qrPixmap();
	if(nullptr!=mScanner) {
		QList<ZScanResult> found=mScanner->scan(px);
		int fsz=found.size();
		qDebug()<<"fsz="<<fsz;
		if(fsz<1) {
			ui->labelQR->setText("ERROR: no QR code found!");
			ui->labelQRScanner->setPixmap(QPixmap());
		} else if(fsz>1) {
			ui->labelQR->setText("ERROR: found more than one QR code!");
			ui->labelQRScanner->setPixmap(QPixmap());
		} else {
			QString data=found[0].data;
			if(data.size()<=0) {
				ui->labelQR->setText("ERROR: byte segments not currently supported!");
				ui->labelQRScanner->setPixmap(QPixmap());
			} else {
				ui->labelQR->setText(data);
				QPixmap px2(px);
				QPainter p(&px2);
				found[0].paint(p);
				ui->labelQRScanner->setPixmap(px2);
			}
		}
	} else {
		qWarning()<<"PX: "<<px<<" scanner: "<<mScanner;
	}
}

void HubWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}

void HubWindow::onListenStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	ui->lineEditBindPort->setEnabled(OFF==current);
	ui->comboBoxLocalAddress->setEnabled(OFF==current);
	ui->pushButtonSendData->setEnabled(ON==current);
	const bool on=(ON==current);
	if(!on) {
		ui->tabWidget->setCurrentIndex(0);
	}
	//ui->tabWidget->setEnabled(on);
	on?mSummaryTimer.start():mSummaryTimer.stop();
	appendLog("New listening state: "+ToggleStateToSTring(current));
	if(TRYING==current) {
		QHostInfo::lookupHost("localhost",this, SLOT(onLocalHostLookupComplete(QHostInfo)));
	} else if(OFF==current) {
		CommsChannel *comms=mHub->comms();
		if(0!=comms) {
			comms->stop();
		}
	}
}



void HubWindow::onLocalHostLookupComplete(QHostInfo hi)
{
	OC_METHODGATE();
	for(QHostAddress adr:hi.addresses()) {
		if(adr.isNull()) {
			ui->logScroll->appendLog("Skipping invalid address during local host lookup: "+adr.toString());
		} else {
			CommsChannel *comms=mHub->comms();
			if(0!=comms) {
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
	OC_METHODGATE();
	qDebug()<<"## onRemoteHostLookupComplete";
	for(QHostAddress adr:hi.addresses()) {
		if(adr.isNull()) {
			ui->logScroll->appendLog("Skipping invalid address during remote host lookup: "+adr.toString());
		} else {
			const int l=ui->horizontalSliderSendCount->value();
			const int sz=int(500*ui->horizontalSliderSendSize->value())/100;
			for(int i=0; i<l; ++i) {
				QByteArray ba=("PING "+QString::number(i)+"/"+QString::number(l)).toUtf8()+" ";
				const int sz2=sz-ba.size();
				for(int i=0; i<sz2; ++i) {
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


void HubWindow::on_pushButtonSendData_clicked()
{
	//TODO: Implement or remove

}





void HubWindow::startProcess(QString base)
{
	OC_METHODGATE();
	const QString program = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath()+"/../"+base+"/"+base;
	qDebug()<<"Starting process: "<<program;
	QStringList arguments;
	//arguments << "-style" << "fusion";
	QProcess *proc = new QProcess(this);
	proc->setProcessChannelMode(QProcess::ForwardedChannels);
	proc->start(program, arguments);
	//TODO: make async
	proc->waitForStarted();
	if(QProcess::Running!=proc->state()) {
		qDebug()<<"ERROR could not start: "<<proc->errorString();
		proc->deleteLater();
		proc=0;
	} else {
		qDebug()<<"A-OK";
	}
}


#ifdef EXTERNAL_LIB_OPENCL

void HubWindow::onGLWidgetInitialized()
{
	qDebug()<<"INIT CL ----- ";
	HelloGLCLViewRenderer *rendrer=new HelloGLCLViewRenderer();
	if(nullptr!=rendrer) {
		rendrer->initialize(ui->openGLWidgetCLGLView->sharingGLContext());
		ui->openGLWidgetCLGLView->setRenderer(rendrer);

		if(!connect(ui->pushButtonCLGLDisplay, &QPushButton::toggled,ui->openGLWidgetCLGLView,&CLGLView::onDisplayToggle,OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(ui->pushButtonCLGLRender, &QPushButton::toggled,ui->openGLWidgetCLGLView,&CLGLView::onRenderToggle,OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	} else {
		qWarning()<<"ERROR: could not allocate rendrer";
	}
}

#endif // EXTERNAL_LIB_OPENCL

void HubWindow::on_comboBoxAddLocal_currentIndexChanged(const QString &arg1)
{
	OC_METHODGATE();
	if("Remote"==arg1) {
		//TODO: synthesize arguments object that points to parent hub
		startProcess("remote");

	} else if("Agent"==arg1) {
		//TODO: synthesize arguments that points to parent hub
		startProcess("agent");
	}
	ui->comboBoxAddLocal->setCurrentIndex(0);
}

void HubWindow::on_tabWidget_currentChanged(int)
{

}

// CommsChannel slots
///////////////////////////////////////////

void HubWindow::onCommsError(QString msg)
{
	OC_METHODGATE();
	if("Unable to send a message"==msg) {

	} else {
		appendLog("GOT ERROR: "+msg);
	}
}



void HubWindow::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	if(0!=c) {
		appendLog("CLIENT ADDED: "+c->summary());
		ui->widgetIncommingNodes->update();
	}
}


void HubWindow::onCommsConnectionStatusChanged(bool s)
{
	OC_METHODGATE();
	qDebug()<<"connection state changed: "<<s;
}


void HubWindow::on_pushButtonTest_clicked()
{
	OC_METHODGATE();
	qDebug()<<"TEST BUTTON PRESSED";
}



void HubWindow::on_lineEditQR_textChanged(const QString &text)
{
	OC_METHODGATE();
	// Initialize scanner
	ui->labelQR->setText("Working...");
	ui->labelQRScanner->setPixmap(QPixmap());
	//Initialize generator
	ui->widgetQR->setQRData(text);
}


void HubWindow::on_pushButtonRandomIdenticonID_clicked()
{
	OC_METHODGATE();
	QString id=Key::hash(QString::number(qrand()));
	ui->lineEditIdenticonID->setText(id);
}


void HubWindow::updateIdentityWidgets()
{
	OC_METHODGATE();
	QString id=ui->lineEditIdenticonID->text();
	PortableID pid;
	pid.setName("Dummy Identicon");
	pid.setID(id);
	pid.setType(TYPE_AGENT);
	ui->widgetIdenticonAgent->setPortableID(pid);
	ui->widgetIris->setPortableID(pid);
	ui->widgetEyes->setPortableID(pid);
	ui->widgetQR_2->setQRData(id);
	pid.setType(TYPE_REMOTE);
	ui->widgetIdenticonRemote->setPortableID(pid);
	pid.setType(TYPE_HUB);
	ui->widgetIdenticonHub->setPortableID(pid);
}

void HubWindow::on_lineEditIdenticonID_textChanged(const QString &)
{
	OC_METHODGATE();
	updateIdentityWidgets();
}


void HubWindow::on_horizontalSliderIrisIndex_valueChanged(int value)
{
	ui->widgetIris->setIrixIndex(value);
}



static const QDir topDir("/");
static QDir lastDir = topDir;

static QString saveIdenticonWidget(IdenticonWidget *iw, QString base="identicon")
{
	QFileDialog fd;
	QStringList filters;
	QString filePath="";
	if(topDir == lastDir) {
		lastDir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	fd.setDirectory(lastDir);
	filters << "PNG (*.png)";
	filters << "SVG (*.svg)";
	fd.setNameFilters(filters);
	fd.selectFile(base);
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setAcceptMode(QFileDialog::AcceptSave);
	if (fd.exec()) {
		lastDir=fd.directory();
		QStringList  fileNames = fd.selectedFiles();
		if(fileNames.size()>0) {
			QString fileName=fileNames.first();
			QFileInfo fileInfo(fileName);

			if(fileInfo.fileName().endsWith(".svg")) {
				QByteArray xml=iw->svgXML();
				filePath=fileInfo.absoluteFilePath();
				QFile file(filePath);
				file.open(QIODevice::WriteOnly);
				file.write(xml);
				file.close();
			} else if(fileInfo.fileName().endsWith(".png")) {
				Identicon identicon=iw->identicon();
				const quint32 sz=1024;
				QImage image=identicon.image(sz, sz);
				filePath=fileInfo.absoluteFilePath();
				image.save(filePath);
			}
		}
	}
	return filePath;
}

static QString saveIrisWidget(IrisWidget *iw, quint32 irisIndex=0, QString base="iris")
{
	QFileDialog fd;
	QStringList filters;
	QString filePath="";
	if(topDir == lastDir) {
		lastDir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
	}
	fd.setDirectory(lastDir);
	filters << "PNG (*.png)";
	fd.setNameFilters(filters);
	fd.selectFile(base);
	fd.setFileMode(QFileDialog::AnyFile);
	fd.setAcceptMode(QFileDialog::AcceptSave);
	if (fd.exec()) {
		lastDir=fd.directory();
		QStringList  fileNames = fd.selectedFiles();
		if(fileNames.size()>0) {
			QString fileName=fileNames.first();
			QFileInfo fileInfo(fileName);
			if(fileInfo.fileName().endsWith(".png")) {
				PortableID pid=iw->portableID();
				const quint32 sz=1024;
				QImage irisImage(sz,sz, QImage::Format_ARGB32);
				irisImage.fill(Qt::transparent);
				QPainter painter(&irisImage);
				IrisRendrer ir;
				ir.setPortableID(pid);
				QRect r(0,0,sz,sz);
				ir.draw(r, painter, irisIndex);
				filePath=fileInfo.absoluteFilePath();
				irisImage.save(filePath);
			}
		}
	}
	return filePath;
}

void HubWindow::on_pushButtonSaveIdenticonAgent_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonAgent, "agent_"+ui->widgetIdenticonAgent->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		ui->logScroll->appendPlainText("Saved "+filePath);
	} else {
		ui->logScroll->appendPlainText("Agent identicon save aborted");
	}
}



void HubWindow::on_pushButtonSaveIdenticonRemote_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonRemote, "remote_"+ui->widgetIdenticonRemote->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		ui->logScroll->appendPlainText("Saved "+filePath);
	} else {
		ui->logScroll->appendPlainText("Remote identicon save aborted");
	}
}

void HubWindow::on_pushButtonSaveIdenticonHub_clicked()
{
	const QString filePath=saveIdenticonWidget(ui->widgetIdenticonHub, "hub_"+ui->widgetIdenticonHub->identicon().id().id()+".svg");
	if(!filePath.isEmpty()) {
		ui->logScroll->appendPlainText("Saved "+filePath);
	} else {
		ui->logScroll->appendPlainText("Hub identicon save aborted");
	}
}



void HubWindow::on_pushButtonSaveIdenticonIris_clicked()
{
	const quint32 irisIndex = ui->horizontalSliderIrisIndex->value();
	const QString filePath=saveIrisWidget(ui->widgetIris, irisIndex, "iris_"+QString::number(irisIndex)+"_"+ui->widgetIris->portableID().id()+".png");
	if(!filePath.isEmpty()) {
		ui->logScroll->appendPlainText("Saved "+filePath);
	} else {
		ui->logScroll->appendPlainText("Iris image save aborted");
	}
}
