#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "comms/Client.hpp"

#include "basic/Settings.hpp"
#include "basic/GenericKeyEventHandler.hpp"

#include "../libmap/MapControl.hpp"
#include "../libmap/OSMMapAdapter.hpp"
#include "../libmap/OpenAerialMapAdapter.hpp"
#include "../libmap/MapLayer.hpp"
#include "../libmap/LineString.hpp"

#include "comms/discovery/DiscoveryClient.hpp"

#include "security/PortableID.hpp"


#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QScrollBar>
#include <QComboBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif



RemoteWindow::RemoteWindow(Remote *remote, QWidget *parent) :
	QWidget(parent)
  , ui(new Ui::RemoteWindow)
  , mRemote(remote)
  , mSpinner(nullptr)
{
	ui->setupUi(this);
	updateIdentity();
	if(nullptr!=mRemote){
		Settings &s=remote->settings();

		//Select correct starting page
		QWidget *startPage=ui->pageRunning;
		if(!mRemote->keyStore().localKey().isValid(true)){
			qDebug()<<"STARTING WITH DELIVERY";
			startPage=ui->pageDelivery;
		}
		else if(mRemote->peers().getParticipantCount()<=0){
			qDebug()<<"STARTING WITH PAIRING";
			startPage=ui->pagePairing;
		}
		else{
			qDebug()<<"STARTING WITH RUN";
		}
		ui->stackedWidgetScreen->setCurrentWidget(startPage);
		//Make sure to switch page on "done"
		connect(ui->widgetPairing, &PairingWizard::done, [=]() {
			updateIdentity();
			ui->stackedWidgetScreen->setCurrentWidget(startPage);
		} );

		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		remote->hookSensorSignals(*this);

		ui->numericEntryServoTest->configure(&s,500,2500,200,1500," µs","Test remote servo control via network", "servo-position-1");
		if(!connect(ui->numericEntryServoTest,SIGNAL(valueChanged(int)),this,SLOT(onServoPositionChanged(int)))){
			qWarning()<<"ERROR: could not connect";
		}
		ui->labelTouch->setText("WAITING FOR TOUCH");
		ui->comboBoxControlLevel->setCurrentText(s.getCustomSetting("octomy.remote.control.level",ui->comboBoxControlLevel->currentText()));
		prepareMenu();
		prepareSpinner();
		updateControlLevel();
		GenericKeyEventHandler *gkh=new GenericKeyEventHandler(ui->plainTextEditSpeechText);
		gkh->setEventProcessor([=](QObject *o, QKeyEvent *keyEvent){
			auto t=keyEvent->type();
			if(t==QEvent::KeyPress || t==QEvent::KeyRelease){
				keyEvent->accept();
				if (nullptr!=keyEvent && keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ControlModifier)
				{
					// Ctrl + return pressed
					if (t == QEvent::KeyRelease) {
						ui->pushButtonSay->click();
					}

					// This event has been handled
					return true;
				}
			}
			return false;
		});
	}
	else{
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
		setDisabled(true);
		qWarning()<<"ERROR: no remote";
	}
#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

RemoteWindow::~RemoteWindow(){
	delete ui;
}

void RemoteWindow::updateIdentity(){
	qDebug()<<"UPDATE IDENTITY";
	if(nullptr!=mRemote){
		mRemote->updateDiscoveryClient();
		mRemote->hookPeerSignals(*this);
	}
	ui->widgetPairing->configure(mRemote);


	ui->comboBoxAgent->clear();
	if(nullptr!=mRemote){
		NodeAssociateStore &peerStore=mRemote->peers();
		QMap<QString, QSharedPointer<NodeAssociate> > &peers=peerStore.getParticipants();
		for(QMap<QString, QSharedPointer<NodeAssociate> >::iterator i=peers.begin(), e=peers.end();i!=e; ++i){
			const QString id=i.key();
			QSharedPointer<NodeAssociate> peer=i.value();
			QString name=peer->name().trimmed();
			if(""==name){
				name=peer->id().mid(0,16);
			}
			addAgentToList(name, ":/icons/agent.svg");
		}
	}
	else{
		qWarning()<<"ERROR: no remote";
	}
}



void RemoteWindow::addAgentToList(QString name, QString iconPath){
	QIcon icon;
	icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
	ui->comboBoxAgent->insertItem(0, icon, name);
	ui->comboBoxAgent->setCurrentIndex(0);
}


void RemoteWindow::appendLog(const QString& text){
	OC_METHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(nullptr!=vsb){
		vsb->setValue(vsb->maximum());
	}
}



void RemoteWindow::notifyAndroid(QString s){
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIF: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/remote/Remote",
											  "notify",
											  "(Ljava/lang/String;)V",
											  QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void RemoteWindow::toastAndroid(QString s){
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/remote/Remote",
											  "toast",
											  "(Ljava/lang/String;)V",
											  QAndroidJniObject::fromString(s).object<jstring>());
#endif
}



void RemoteWindow::keyReleaseEvent(QKeyEvent *e){
	if(Qt::Key_Back==e->key()){
		/*
		if(ui->pageConnect==ui->stackedWidgetScreen->currentWidget()){
			appendLog("EXITING APP ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConfirmQuit);
		}
		else if(ui->pageStatus==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConnect);
		}
		else
		*/
		if(ui->pageRunning==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO CONFIRM QUIT SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConfirmQuit);
		}
		else if(ui->pageConfirmQuit==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO RUNNING SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
		}
		else{
			appendLog("ERROR ON BACK BUTTON");
		}
		e->accept();
	}
	else{
		appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}


bool RemoteWindow::eventFilter(QObject *object, QEvent *event){
	//qDebug()<<"EVENT: "<<object<<": "<<event;
	if ((this==object) && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress )){
		QMouseEvent *r= static_cast<QMouseEvent *>(event);
		ui->labelTouch->setText("TOUCH: <"+QString::number(r->globalX())+", "+QString::number(r->globalY())+">");
		QPointF p=r->localPos();
		QSize s=size();
		mRemote->onTouchUpdated(QVector2D(p.x()/(qreal)s.width(), p.y()/(qreal)s.height()));
	}
	//object == ui->widgetBackground && (
	return false;
}



//////////////////////////////////////////////////
// Internal slots



void RemoteWindow::onStartShowBirthCertificate()
{
	PortableID id;
	Settings *s=(nullptr!=mRemote)?(&mRemote->settings()):nullptr;
	if(nullptr!=s){
		id.fromPortableString(s->getCustomSetting("octomy.portable.id",""));
	}
	id.setID(mRemote->keyStore().localKey().id());
	id.setType(TYPE_REMOTE);
	ui->widgetBirthCertificate->setPortableID(id);
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageMyID);
}


void RemoteWindow::onStartPairing()
{
	ui->widgetPairing->reset();
	ui->stackedWidgetScreen->setCurrentWidget(ui->pagePairing);
}


void RemoteWindow::onStartPlanEditor()
{
	ui->stackedWidgetScreen->setCurrentWidget(ui->pagePlan);
}



//////////////////////////////////////////////////
// Peer Store slots


void RemoteWindow::onPeerAdded(QString id)
{
	qDebug()<<"REMOTEW peer added: "<<id;
}

void RemoteWindow::onPeerRemoved(QString id)
{
	qDebug()<<"REMOTEW peer removed: "<<id;
}

void RemoteWindow::onPeersChanged()
{
	qDebug()<<"REMOTEW peers changed: ";
	updateIdentity();
}


void RemoteWindow::onStoreReady(bool ok)
{
	qDebug()<<"REMOTEW peers load compelte: "<<(ok?"OK":"ERROR");
	updateIdentity();
}


//////////////////////////////////////////////////
// CommsChannel slots


void RemoteWindow::onError(QString e)
{
	qDebug()<<"REMOTEW comms: error "<<e;
}

void RemoteWindow::onClientAdded(Client *c)
{

	qDebug()<<"REMOTEW comms: client added "<<(0==c?"null":QString::number(c->getShortHandID(),16));
}

void RemoteWindow::onConnectionStatusChanged(bool c)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEW comms: new connection status: "<<c;
	//ui->widgetConnection->setConnectState(c?ON:OFF);
}

//////////////////////////////////////////////////
// Internal sensor slots

void RemoteWindow::onPositionUpdated(const QGeoPositionInfo &info)
{
	OC_METHODGATE();
	ui->labelGPS->setText("GPS: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	appendLog("GPS update: "+info.coordinate().toString()+"@"+info.timestamp().toString());
}


void RemoteWindow::onCompassUpdated(QCompassReading *r)
{
	OC_METHODGATE();
	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void RemoteWindow::onAccelerometerUpdated(QAccelerometerReading *r)
{
	OC_METHODGATE();
	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RemoteWindow::onGyroscopeUpdated(QGyroscopeReading *r)
{
	OC_METHODGATE();
	ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}





void RemoteWindow::onServoPositionChanged(int val)
{
	OC_METHODGATE();
	if(0!=mRemote){
		Pose p;
		p.pos1=val;
		mRemote->onDirectPoseChanged(p);
	}
}


///////////////////////////////////
//Internal UI slots



void RemoteWindow::onTryToggleConnectionChanged(TryToggleState s)
{
	OC_METHODGATE();
	appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	/*
	bool ce=false;
	QWidget *page=ui->pageConnect;
	switch(s){
		case(TRYING):{
				remote->start(ui->widgetConnection->getLocalAddress(),ui->widgetConnection->getLocalPort(), ui->widgetConnection->getTargetAddress(), ui->widgetConnection->getTargetPort());
				ui->labelLocal->setText("LOCAL:"+ui->widgetConnection->getLocalAddress().toString()+":"+QString::number(ui->widgetConnection->getLocalPort()));
				ui->labelHub->setText("HUB: "+ui->widgetConnection->getTargetAddress().toString()+ ":"+ QString::number(ui->widgetConnection->getTargetPort()));
			}break;
		case(ON):{
				page=ui->pageStatus;
				installEventFilter(this);
			}break;
		default:
		case(OFF):{
				removeEventFilter(this);
				ce=true;
			}break;
	}
	ui->widgetConnection->setEnabled(ce);
	ui->stackedWidgetScreen->setCurrentWidget(page);
	*/
}


void RemoteWindow::on_pushButtonConfirmQuit_clicked()
{
	OC_METHODGATE();
	exit(0);
}


void RemoteWindow::updateControlLevel()
{
	OC_METHODGATE();
	qDebug()<<"SWITCHING CONTROL LEVEL TO "<<ui->comboBoxControlLevel->currentText();
	const int idx=ui->comboBoxControlLevel->currentIndex();
	ui->stackedWidgetControl->setCurrentIndex(idx);
	if(nullptr!=mSpinner){
		mSpinner->setStarted( (0!=idx) &&  ( OFF != ui->widgetConnection->connectState() ) );
	}
}


void RemoteWindow::updateActiveAgent()
{
	OC_METHODGATE();
	//qDebug()<<"SWITCHING ACTIVE AGENT TO "<<agentName;
	const int idx=ui->comboBoxControlLevel->currentIndex();
	ui->stackedWidgetControl->setCurrentIndex(idx);

}

void RemoteWindow::prepareMenu()
{
	OC_METHODGATE();
	QAction *pairingAction = new QAction(tr("Pair"), this);
	pairingAction->setStatusTip(tr("Do the pairing dance"));
	pairingAction->setIcon(QIcon(":/icons/pair.svg"));
	connect(pairingAction, &QAction::triggered, this, &RemoteWindow::onStartPairing);
	mMenu.addAction(pairingAction);

	QAction *certAction = new QAction(tr("Show My ID"), this);
	certAction->setStatusTip(tr("Show the identification of this remote"));
	certAction->setIcon(QIcon(":/icons/certificate.svg"));
	connect(certAction, &QAction::triggered, this, &RemoteWindow::onStartShowBirthCertificate);
	mMenu.addAction(certAction);
}

void RemoteWindow::prepareSpinner()
{
	OC_METHODGATE();
	mSpinner=new WaitingSpinnerWidget(ui->stackedWidgetControl, true, true);
	SpinnerStyle style;
	style.setColor(QColor("white"));
	style.setRelatveSize(true);
	style.setNumberOfLines(24);
	style.setLineLength(10);
	style.setInnerRadius(40);
	style.setLineWidth(3);
	mSpinner->setText("Reconnecting..");
	mSpinner->setStyle(style);

}




void RemoteWindow::on_comboBoxAgent_currentIndexChanged(int index)
{
	OC_METHODGATE();
	updateActiveAgent();
}

void RemoteWindow::on_comboBoxControlLevel_activated(const QString &cLevel)
{
	Settings *settings=(nullptr!=mRemote)?(&mRemote->settings()):nullptr;
	if(nullptr!=settings){
		settings->setCustomSetting("octomy.remote.control.level",cLevel);
	}
	updateControlLevel();
}

void RemoteWindow::on_pushButtonSay_clicked()
{
	QString text=ui->plainTextEditSpeechText->toPlainText();
	ui->logScroll->appendLog("SAID: "+text);
	ui->plainTextEditSpeechText->clear();
}

void RemoteWindow::on_pushButtonMenu_clicked()
{
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}

void RemoteWindow::on_pushButtonBack_6_clicked()
{
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
}
