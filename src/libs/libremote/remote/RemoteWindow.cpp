#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"
#include "comms/CommsChannel.hpp"
#include "utility/Utility.hpp"
#include "comms/CommsSession.hpp"
#include "widgets/ControlDeliveryWizard.hpp"



#include "map/MapControl.hpp"
#include "map/OSMMapAdapter.hpp"
#include "map/OpenAerialMapAdapter.hpp"
#include "map/MapLayer.hpp"
#include "map/LineString.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "security/PortableID.hpp"

#include "node/RemoteClientWidget.hpp"

#include "identity/Identicon.hpp"


#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QScrollBar>
#include <QComboBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

RemoteWindow::RemoteWindow(QSharedPointer<Remote> remote, QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::RemoteWindow)
	, mRemote(remote)
{
	OC_METHODGATE();
	ui->setupUi(this);
	prepareDelivery();
	prepareDiscovery();
	preparePairing();
	prepareClientList();
	prepareControlLevelList();
	prepareMenu();

	hookSensorSignals();
	goToStartPage();

	updateClientsList();
	updateClientWidgetList();

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

RemoteWindow::~RemoteWindow()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


Settings &RemoteWindow::settings()
{
	OC_METHODGATE();
	return mRemote->settings();
}


void RemoteWindow::prepareDelivery()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE DELIVERY";
	if(!mRemote.isNull()) {
		// Make sure to go to correct page after delivery is complete
		if(!connect(ui->widgetDelivery, SIGNAL(done(bool)), this, SLOT(onDeliveryDone(bool)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
	} else {
		qWarning()<<"ERROR: no remote";
	}
}


void RemoteWindow::prepareDiscovery()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE DISCOVERY";
	if(!mRemote.isNull()) {
		mRemote->updateDiscoveryClient();
		mRemote->addressBook().setHookSignals(*this, true);
		mRemote->addressBook().synchronize([=](SimpleDataStore &ab, bool ok) {
			qDebug()<<"Address book synchronized: "<<ok;
			prepareDiscovery();
			preparePairing();
		});
	} else {
		qWarning()<<"ERROR: no remote";
	}
}

void RemoteWindow::preparePairing()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE PAIRING";
	if(!mRemote.isNull()) {
		//Make sure to go to correct page when pairing is complete
		connect(ui->widgetPairing, &PairingWizard::done, [=]() {
			setCurrentPage(ui->pageRunning);
		});
	} else {
		qWarning()<<"ERROR: no remote";
	}

}

void RemoteWindow::prepareClientList()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE CLIENT LIST";
	updateClientsList();
}


void RemoteWindow::prepareControlLevelList()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE CONTROL LEVEL LIST";
	if(nullptr!=mRemote) {
		Settings &s=mRemote->settings();
		ui->comboBoxControlLevel->setCurrentText(s.getCustomSetting("octomy.remote.control.level",ui->comboBoxControlLevel->currentText()));
		updateControlLevel();
	} else {
		qWarning()<<"ERROR: no remote";
	}

}


void RemoteWindow::prepareMenu()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN PREPARE MENU";
	QAction *pairingAction = OC_NEW QAction(tr("Pair"), this);
	pairingAction->setStatusTip(tr("Do the pairing dance"));
	pairingAction->setIcon(QIcon(":/icons/pair.svg"));
	connect(pairingAction, &QAction::triggered, this, &RemoteWindow::onStartPairing);
	mMenu.addAction(pairingAction);

	QAction *certAction = OC_NEW QAction(tr("Show My ID"), this);
	certAction->setStatusTip(tr("Show the identification of this remote"));
	certAction->setIcon(QIcon(":/icons/certificate.svg"));
	connect(certAction, &QAction::triggered, this, &RemoteWindow::onStartShowBirthCertificate);
	mMenu.addAction(certAction);
}






// Rebuild the stack of client widgets based on available clients
void RemoteWindow::updateClientWidgetList()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN UPDATE CLIENT WIDGET LIST";
	if(!mRemote.isNull()) {
		QList <QWidget * > widgets=mRemote->clientList().widgets();
		utility::clearStackedWidget(ui->stackedWidgetControl);
		if(!mRemote.isNull()) {
			qDebug()<<"REMOTEWIN CLIENT WIDGET LIST CONTAINED "<<widgets.count()<<" ITEMS";
			for(QWidget *widget: widgets) {
				if(nullptr!=widget) {
					qDebug()<<"REMOTEWIN ADDING CLIENT WIDGET";
					ui->stackedWidgetControl->addWidget(widget);
				} else {
					qWarning()<<"No client widget";
				}
			}
		} else {
			qWarning()<<"No remote";
		}
		int ct=ui->stackedWidgetControl->count();
		qDebug()<<"REMOTEWIN ADDED TOTAL OF " << ct << " CLIENT WIDGET(S)";
		//Add the placeholder back if count <=0
		if(ct<=0) {
			qDebug()<<"REMOTEWIN ADDING PLACEHOLDER";
			ui->stackedWidgetControl->addWidget(ui->widgetClientPlaceholder);
		}
	}
}

// Update the control level combo box based on the currently selected client
void RemoteWindow::updateControlLevel()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN UPDATE CONTROL LEVEL";
	//qDebug()<<"SWITCHING CONTROL LEVEL TO "<<ui->comboBoxControlLevel->currentText();
	if(ui->comboBoxClient->count() >0) {
		ui->comboBoxControlLevel->setEnabled(true);
		const int idx=ui->comboBoxControlLevel->currentIndex();
		RemoteClientWidget *clientWidget = qobject_cast<RemoteClientWidget *>(ui->stackedWidgetControl->currentWidget());
		if(nullptr!=clientWidget) {
			clientWidget->updateControlLevel(idx);
		}
	} else {
		ui->comboBoxControlLevel->setEnabled(false);
		ui->comboBoxControlLevel->setCurrentIndex(0);
	}
}


// Update the currently displayed client widget based on the selection in the client combobox
void RemoteWindow::updateActiveClient()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN UPDATE ACTIVE CLIENT";
	if(!mRemote.isNull()) {
		const int idx=ui->comboBoxClient->currentIndex();
		const QString clientName=ui->comboBoxClient->currentText();
		const QString clientID=ui->comboBoxClient->currentData().toString();

		QSharedPointer<Associate>  clientAssociate=mRemote->addressBook().associateByID(clientID);
		qDebug()<<"SWITCHING ACTIVE CLIENT TO "<<clientName<<"("<<idx<<", "<<clientID<<")";
		QList <QWidget * > widgets=mRemote->clientList().widgets();
		const int ct=widgets.size();
		if((idx>=0) && (idx<ct)) {
			QWidget *cw=widgets[idx];
			if(nullptr!=cw) {

				/*
				const RemoteClientWidget *old=qobject_cast<RemoteClientWidget *>(ui->stackedWidgetControl->currentWidget());
				const int oldIndex=ui->stackedWidgetControl->currentIndex();
				const QString oldName=(nullptr==old)?"NULL":old->nodeAssociate()->name();
				*/
				ui->stackedWidgetControl->setCurrentWidget(cw);
				/*
				const int newIndex=ui->stackedWidgetControl->currentIndex();
				const QString newName=(nullptr==cw)?"NULL":cw->nodeAssociate()->name();
				qDebug()<<"SWITCHING FROM "<<old << "("<<oldIndex << ", '"<<oldName << "') TO "<<cw <<"("<<newIndex << ", '"<<newName << "')";
				*/
				updateControlLevel();
			} else {
				qWarning()<<"No current widget found";
			}
		} else {
			qWarning().noquote().nospace()<<"Selected client index "<< idx<< " out of range 0 - "<<ct;
		}
	} else {
		qWarning()<<"No remote";
	}
}


// Rebuild the list of clients in the client combobox based on the actual list of clients
int RemoteWindow::updateClientsList()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN UPDATE CLIENT LIST";
	int ct=0;
	ui->comboBoxClient->clear();
	if(nullptr!=mRemote) {
		QSharedPointer<Associate> local=mRemote->nodeIdentity();
		if(!local.isNull()) {
			const QString myID=local->id();
			AddressBook &peerStore=mRemote->addressBook();
			QMap<QString, QSharedPointer<Associate> > &peers=peerStore.all();
			for(QMap<QString, QSharedPointer<Associate> >::iterator i=peers.begin(), e=peers.end(); i!=e; ++i) {
				const QString id=i.key();
				QSharedPointer<Associate> peer=i.value();
				if(NodeType::TYPE_REMOTE!=peer->type()) {
					addClientToList(peer);
					ct++;
				}
			}
		}
	} else {
		qWarning()<<"ERROR: no remote";
	}
	return ct;
}


// Helper to add a single client to the client combo box based on associate
void RemoteWindow::addClientToList(QSharedPointer<Associate> peer)
{
	qDebug()<<"REMOTEWIN ADD CLIENT TO LIST";
	OC_METHODGATE();
	if(!peer.isNull()) {
		QString name=peer->name().trimmed();
		if(""==name) {
			name=peer->id().mid(0,16);
		}
		QVariant userData=peer->id();
		PortableID pid=peer->toPortableID();
		Identicon identicon(pid);
		QIcon icon=QIcon(identicon.pixmap());
		qDebug()<<" + INSERTING CLIENT IN DROPDOWN "<<peer->toPortableID().toPortableString();
		ui->comboBoxClient->insertItem(0, icon, name, userData);
		ui->comboBoxClient->setCurrentIndex(0);
	}
}



void RemoteWindow::hookSensorSignals()
{
	OC_METHODGATE();
	if(nullptr!=mRemote) {
		mRemote->setHookSensorSignals(*this, true);
	} else {
		qWarning()<<"ERROR: no remote";
	}
}


void RemoteWindow::setCurrentPage(QWidget *curr)
{
	OC_METHODGATE();
	const QWidget *cur=curr;
	if(cur == ui->pageRunning) {
		if(!mRemote.isNull()) {
			ui->widgetPairing->configure(mRemote);
		}
	} else if(cur == ui->pageDelivery) {
		if(!mRemote.isNull()) {
			ui->widgetDelivery->configure(mRemote);
		}
		ui->widgetDelivery->reset();
	} else if(cur == ui->pagePairing) {
		if(!mRemote.isNull()) {
			ui->widgetPairing->configure(mRemote);
		}
		ui->widgetPairing->reset();
	} else if(cur == ui->pageMyID) {
		PortableID id;
		Settings *s=(!mRemote.isNull())?(&mRemote->settings()):nullptr;
		if(nullptr!=s) {
			id.fromPortableString(s->getCustomSetting("octomy.portable.id",""));
		}
		QSharedPointer<Key> key=(!mRemote.isNull())?mRemote->keyStore().localKey():QSharedPointer<Key>(nullptr);
		if(!key.isNull()) {
			id.setID(key->id());
		} else {
			qWarning()<<"ERROR: No key";
		}
		id.setType(TYPE_REMOTE);
		ui->widgetBirthCertificate->setPortableID(id);

	}
	ui->stackedWidgetScreen->setCurrentWidget(curr);
}

void RemoteWindow::goToStartPage()
{
	OC_METHODGATE();
	//qDebug()<<"";
	//qDebug()<<"----------------- - - - - - ------------------------- - - - - ";
	if(nullptr!=mRemote) {
		//Select correct starting page
		QSharedPointer<Key> key=mRemote->keyStore().localKey();
		if(key.isNull() || !key->isValid(true)) {
			//qDebug()<<"STARTING WITH DELIVERY";
			setCurrentPage(ui->pageDelivery);
		} else if(mRemote->addressBook().associateCount()<=0) {
			//qDebug()<<"STARTING WITH PAIRING";
			setCurrentPage(ui->pagePairing);
		} else {
			//qDebug()<<"STARTING WITH RUN";
			setCurrentPage(ui->pageRunning);
		}
	} else {
		qWarning()<<"ERROR: no remote";
	}
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




void RemoteWindow::notifyAndroid(QString s)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	if(Qt::Key_Back==e->key()) {
		/*
		if(ui->pageConnect==ui->stackedWidgetScreen->currentWidget()){
			appendLog("EXITING APP ON BACK BUTTON");
			setCurrentPage(ui->pageConfirmQuit);
		}
		else if(ui->pageStatus==ui->stackedWidgetScreen->currentWidget()){
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			setCurrentPage(ui->pageConnect);
		}
		else
		*/


		if(ui->pageRunning==ui->stackedWidgetScreen->currentWidget()) {
			appendLog("GOING TO CONFIRM QUIT SCREEN ON BACK BUTTON");
			setCurrentPage(ui->pageConfirmQuit);
		} else if(ui->pageConfirmQuit==ui->stackedWidgetScreen->currentWidget()) {
			appendLog("GOING TO RUNNING SCREEN ON BACK BUTTON");
			setCurrentPage(ui->pageRunning);
		} else {
			appendLog("ERROR ON BACK BUTTON");
		}
		e->accept();
	} else {
		//appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}




void RemoteWindow::closeEvent(QCloseEvent *event)
{
	OC_METHODGATE();
	if(!mRemote.isNull()) {
		emit mRemote->appClose();
	}
}



//////////////////////////////////////////////////
// Internal slots



void RemoteWindow::onStartShowBirthCertificate()
{
	OC_METHODGATE();
	setCurrentPage(ui->pageMyID);
}


void RemoteWindow::onStartPairing()
{
	OC_METHODGATE();
	ui->widgetPairing->reset();
	setCurrentPage(ui->pagePairing);
}


void RemoteWindow::onStartPlanEditor()
{
	OC_METHODGATE();
	setCurrentPage(ui->pagePlan);
}


//////////////////////////////////////////////////
// Delivery Wizard slots


void RemoteWindow::onDeliveryDone(bool pairNow)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN DELIVERY DONE ";
	prepareDiscovery();
	preparePairing();
	if(pairNow) {
		ui->widgetPairing->reset();
	}
	setCurrentPage(pairNow?ui->pagePairing:ui->pageRunning);
}

//////////////////////////////////////////////////
// Keystore slots


void RemoteWindow::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN keystore load compelte: "<<(ok?"OK":"ERROR");
	prepareDiscovery();
	preparePairing();
}

//////////////////////////////////////////////////
// Peer Store slots


void RemoteWindow::onAssociateAdded(QString id)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN peer added: "<<id;
	// Handled by change handler
}

void RemoteWindow::onAssociateRemoved(QString id)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN peer removed: "<<id;
	// Handled by change handler
}


void RemoteWindow::onAssociateChanged()
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN peers changed: ";
	updateClientsList();
	updateClientWidgetList();
}



//////////////////////////////////////////////////
// CommsChannel slots


void RemoteWindow::onError(QString e)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN comms: error "<<e;
}

void RemoteWindow::onClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN comms: client added "<<(0==c?"null":QString::number(c->localSessionID(),16));
}

void RemoteWindow::onConnectionStatusChanged(bool c)
{
	OC_METHODGATE();
	qDebug()<<"REMOTEWIN comms: new connection status: "<<c;
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


///////////////////////////////////
//Internal UI slots


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
	OC_METHODGATE();
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}

void RemoteWindow::on_pushButtonBack_6_clicked()
{
	OC_METHODGATE();
	setCurrentPage(ui->pageRunning);
}

void RemoteWindow::on_pushButtonStartPairing_clicked()
{
	OC_METHODGATE();
	ui->widgetPairing->reset();
	setCurrentPage(ui->pagePairing);
}
