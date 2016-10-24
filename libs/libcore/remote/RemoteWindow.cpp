#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "../libutil/utility/Utility.hpp"
#include "comms/Client.hpp"
#include "widgets/ControlDeliveryWizard.hpp"



#include "../libmap/MapControl.hpp"
#include "../libmap/OSMMapAdapter.hpp"
#include "../libmap/OpenAerialMapAdapter.hpp"
#include "../libmap/MapLayer.hpp"
#include "../libmap/LineString.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "security/PortableID.hpp"

#include "hub/ClientWidget.hpp"


#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QScrollBar>
#include <QComboBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

RemoteWindow::RemoteWindow(Remote *remote, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RemoteWindow)
	, mRemote(remote)
{
	ui->setupUi(this);
	ui->stackedWidgetScreen->setUpdatesEnabled(false);
	updateIdentity();
	if(nullptr!=mRemote) {
		Settings &s=remote->settings();
		//Select correct starting page
		if(!mRemote->keyStore().localKey().isValid(true)) {
			qDebug()<<"STARTING WITH DELIVERY";
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageDelivery);
		} else if(mRemote->peers().getParticipantCount()<=0) {
			qDebug()<<"STARTING WITH PAIRING";
			ui->widgetPairing->reset();
			ui->stackedWidgetScreen->setCurrentWidget(ui->pagePairing);
		} else {
			qDebug()<<"STARTING WITH RUN";
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
		}


		connect(ui->widgetDelivery, &ControlDeliveryWizard::done, [=](bool pairNow) {
			updateIdentity();
			ui->stackedWidgetScreen->setCurrentWidget(pairNow?ui->pagePairing:ui->pageRunning);
		} );

		ui->widgetDelivery->reset();


		//Make sure to switch page on "done"
		connect(ui->widgetPairing, &PairingWizard::done, [=]() {
			updateIdentity();
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
		} );

		//Remove the placeholder
		ui->stackedWidgetControl->removeWidget(ui->labelClientWidgetPlaceholder);


		remote->hookSensorSignals(*this);


		ui->comboBoxControlLevel->setCurrentText(s.getCustomSetting("octomy.remote.control.level",ui->comboBoxControlLevel->currentText()));
		prepareMenu();
		updateControlLevel();

	} else {

		qWarning()<<"ERROR: no remote";
	}
#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
	ui->stackedWidgetScreen->setUpdatesEnabled(true);
}

RemoteWindow::~RemoteWindow()
{
	delete ui;
}

void RemoteWindow::updateIdentity()
{
	qDebug()<<"UPDATE IDENTITY";
	if(nullptr!=mRemote) {
		mRemote->updateDiscoveryClient();
		mRemote->hookPeerSignals(*this);
	}

	ui->widgetDelivery->configure(mRemote);
	ui->widgetPairing->configure(mRemote);

	ui->comboBoxAgent->clear();
	if(nullptr!=mRemote) {
		NodeAssociateStore &peerStore=mRemote->peers();
		QMap<QString, QSharedPointer<NodeAssociate> > &peers=peerStore.getParticipants();
		for(QMap<QString, QSharedPointer<NodeAssociate> >::iterator i=peers.begin(), e=peers.end(); i!=e; ++i) {
			const QString id=i.key();
			QSharedPointer<NodeAssociate> peer=i.value();
			addAgentToList(peer);
		}
	} else {
		qWarning()<<"ERROR: no remote";
	}
}



void RemoteWindow::addAgentToList(QSharedPointer<NodeAssociate> peer)
{
	QIcon icon;
	QString iconPath=":/icons/agent.svg";
	QString name=peer->name().trimmed();
	if(""==name) {
		name=peer->id().mid(0,16);
	}
	icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
	QVariant userData=peer->id();
	ui->comboBoxAgent->insertItem(0, icon, name);
	ui->comboBoxAgent->setCurrentIndex(0);
}



Settings &RemoteWindow::settings()
{
	return mRemote->settings();
}

void RemoteWindow::notifyAndroid(QString s)
{
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


void RemoteWindow::toastAndroid(QString s)
{
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



void RemoteWindow::keyReleaseEvent(QKeyEvent *e)
{
	if(Qt::Key_Back==e->key()) {
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


		if(ui->pageRunning==ui->stackedWidgetScreen->currentWidget()) {
			appendLog("GOING TO CONFIRM QUIT SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageConfirmQuit);
		} else if(ui->pageConfirmQuit==ui->stackedWidgetScreen->currentWidget()) {
			appendLog("GOING TO RUNNING SCREEN ON BACK BUTTON");
			ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
		} else {
			appendLog("ERROR ON BACK BUTTON");
		}
		e->accept();
	} else {
		appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}



void RemoteWindow::updateControlLevel()
{
	OC_METHODGATE();
	qDebug()<<"SWITCHING CONTROL LEVEL TO "<<ui->comboBoxControlLevel->currentText();
	const int idx=ui->comboBoxControlLevel->currentIndex();
	ClientWidget *clientWidget = qobject_cast<ClientWidget *>(ui->stackedWidgetControl->currentWidget());
	if(nullptr!=clientWidget) {
		clientWidget->updateControlLevel(idx);
	}
}


void RemoteWindow::updateActiveAgent()
{
	OC_METHODGATE();
	const int idx=ui->comboBoxAgent->currentIndex();
	const QString agentName=ui->comboBoxAgent->currentText();
	qDebug()<<"SWITCHING ACTIVE AGENT TO "<<agentName<<"("<<idx<<")";
	if(idx>=0) {
		QMap<int, ClientWidget *>::const_iterator it=mClientWidgets.find(idx);
		ClientWidget *cw=nullptr;
		if(mClientWidgets.end()==it) {
			cw=new ClientWidget(this, QSharedPointer<Node>(mRemote));
			if(nullptr!=cw) {
				qDebug()<<"Created client widget for index "<<idx;
				mClientWidgets[idx]=cw;
				ui->stackedWidgetControl->addWidget(cw);
			} else {
				qWarning()<<"ERROR: Could not allocate client widget";
			}
		} else {
			cw=it.value();
		}
		if(nullptr!=cw) {
			ClientWidget *old=qobject_cast<ClientWidget *>(ui->stackedWidgetControl->currentWidget());
			int oldIndex=ui->stackedWidgetControl->currentIndex();
			qDebug()<<"SWITCHING FROM "<<old << " TO "<<cw;
			ui->stackedWidgetControl->setCurrentWidget(cw);
			int newIndex=ui->stackedWidgetControl->currentIndex();
			qDebug()<<"SWITCHING FROM "<<oldIndex << " TO "<<newIndex;
			updateControlLevel();
		}
	}
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



void RemoteWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(nullptr!=vsb) {
		vsb->setValue(vsb->maximum());
	}
}



//////////////////////////////////////////////////
// Internal slots



void RemoteWindow::onStartShowBirthCertificate()
{
	PortableID id;
	Settings *s=(nullptr!=mRemote)?(&mRemote->settings()):nullptr;
	if(nullptr!=s) {
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
//	ui->labelGPS->setText("GPS: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	appendLog("GPS update: "+info.coordinate().toString()+"@"+info.timestamp().toString());
}


void RemoteWindow::onCompassUpdated(QCompassReading *r)
{
	OC_METHODGATE();
//	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void RemoteWindow::onAccelerometerUpdated(QAccelerometerReading *r)
{
	OC_METHODGATE();
//	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RemoteWindow::onGyroscopeUpdated(QGyroscopeReading *r)
{
	OC_METHODGATE();
	//ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}





void RemoteWindow::onServoPositionChanged(int val)
{
	OC_METHODGATE();
	if(0!=mRemote) {
		Pose p;
		p.pos1=val;
		mRemote->onDirectPoseChanged(p);
	}
}


///////////////////////////////////
//Internal UI slots




void RemoteWindow::on_comboBoxAgent_currentIndexChanged(int index)
{
	OC_METHODGATE();
	updateActiveAgent();
}

void RemoteWindow::on_comboBoxControlLevel_activated(const QString &cLevel)
{
	Settings *settings=(nullptr!=mRemote)?(&mRemote->settings()):nullptr;
	if(nullptr!=settings) {
		settings->setCustomSetting("octomy.remote.control.level",cLevel);
	}
	updateControlLevel();
}


void RemoteWindow::on_pushButtonConfirmQuit_clicked()
{
	OC_METHODGATE();
	exit(0);
}


void RemoteWindow::on_pushButtonMenu_clicked()
{
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}

void RemoteWindow::on_pushButtonBack_6_clicked()
{
	ui->stackedWidgetScreen->setCurrentWidget(ui->pageRunning);
}
