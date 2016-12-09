#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "widgets/TryToggle.hpp"
#include "Agent.hpp"
#include "comms/CommsChannel.hpp"
#include "../libutil/utility/Utility.hpp"
#include "hw/actuators/HexyTool.hpp"
#include "hw/BluetoothList.hpp"
#include "basic/Settings.hpp"
#include "security/PortableID.hpp"
#include "audio/OneOffSpeech.hpp"
#include "comms/ClientDirectory.hpp"
#include "comms/couriers/SensorsCourier.hpp"

#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

AgentWindow::AgentWindow(Agent *agent, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentWindow)
	, mAgent(agent)
	, mHexy(nullptr)
	, mCameraAction(new QAction(tr("Camera"), this))
	, mPairingAction(new QAction(tr("Pair"), this))
	, mHardwareAction(new QAction(tr("Configure HW"), this))
	, mPlanAction(new QAction(tr("Plan"), this))
	, mOnlineAction(new QAction(tr("Online"), this))
	, mShowOnlineButtonAction(new QAction(tr("Online Button"), this))
	, mShowFaceAction(new QAction(tr("Show Face"), this))
	, mShowLogAction(new QAction(tr("Show Log"), this))
	, mShowStatsAction(new QAction(tr("Show Stats"), this))
	, mShowBirthCertificateAction(new QAction(tr("Show Birth Certificate"), this))
	, mUnbirthAction(new QAction(tr("Unbirth!"), this))
{
	ui->setupUi(this);

	updateIdentity();

	if(nullptr!=mAgent) {
		//Settings &s=agent->settings();
		//Select correct starting page
		QWidget *startPage=ui->pageRunning;
		ui->widgetDelivery->reset();
		ui->stackedWidget->setCurrentWidget(mAgent->keyStore().fileExists()?startPage:ui->pageDelivery);

		if(!QObject::connect(ui->widgetDelivery, &AgentDeliveryWizard::done, [=](bool pairNow) {
		updateIdentity();
			ui->stackedWidget->setCurrentWidget(pairNow?ui->pagePairing:startPage);
		} ) ) {
			qWarning()<<"ERROR: Could not connect ";
		}

		if(!connect(ui->widgetPairing, &PairingWizard::done, [=]() {
		ui->stackedWidget->setCurrentWidget(startPage);
		} )) {
			qWarning()<<"ERROR: Could not connect ";
		}

		mAgent->hookCommsSignals(*this);
		mAgent->hookColorSignals(*ui->widgetFace);
		ui->widgetFace->hookSignals(*this);
		ui->widgetPlanEditor->configure("agent.plan");
		prepareMenu();
		updateFaceVisibility();

		updateOnlineStatus();
		//QString text="Hello, my name is "+mAgent->name()+". I am an octomy agent. What is your bidding master?";
		//QString text="Hello, my name is Bodhi. I am an octomy agent. What is your bidding master? 00 0 01010 010 010 010 010101 ";		PortableID id=mAgent->localNodeAssociate()->toPortableID();		new OneOffSpeech(id, text);
	}

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow()
{
	delete ui;
}



void AgentWindow::updateIdentity()
{
	if(nullptr!=mAgent) {
		mAgent->updateDiscoveryClient();
		updateIcon();
	} else {
		qWarning()<<"WARNING: No Agent in agent window";
	}
	ui->widgetFace->setAgent(mAgent);
	ui->widgetDelivery->configure(mAgent);
	ui->widgetPairing->configure(mAgent);

}

void AgentWindow::updateIcon()
{
	if(nullptr!=mAgent) {
		//Set our custom identicon as window icon
		PortableID pid;
		pid.setID(mAgent->keyStore().localKey().id());
		Identicon id(pid);
		QIcon icon;//=windowIcon();
		icon.addPixmap(id.pixmap());
		//	icon.addFile(QStringLiteral(":/icons/agent.svg"), QSize(), QIcon::Normal, QIcon::Off);
		setWindowIcon(icon);
	}
}



void AgentWindow::prepareMenu()
{
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;

	// Camera
	//////////////////


	mCameraAction->setStatusTip(tr("Do the camera dance"));
	mCameraAction->setIcon(QIcon(":/icons/eye.svg"));
	connect(mCameraAction, &QAction::triggered, this, &AgentWindow::onStartCameraPairing);
	mMenu.addAction(mCameraAction);

	// Pairing wizard
	//////////////////

	mPairingAction->setStatusTip(tr("Do the pairing dance"));
	mPairingAction->setIcon(QIcon(":/icons/pair.svg"));
	connect(mPairingAction, &QAction::triggered, this, &AgentWindow::onStartPairing);
	mMenu.addAction(mPairingAction);

	// Hardware wizard
	//////////////////

	mHardwareAction->setStatusTip(tr("Connect to the body of the robot"));
	mHardwareAction->setIcon(QIcon(":/icons/actuator_control.svg"));
	connect(mHardwareAction, &QAction::triggered, this, &AgentWindow::onStartHardware);
	mMenu.addAction(mHardwareAction);

	// Plan editor
	//////////////////
	mPlanAction->setStatusTip(tr("Do the planning dance"));
	mPlanAction->setIcon(QIcon(":/icons/mandate.svg"));
	connect(mPlanAction, &QAction::triggered, this, &AgentWindow::onStartPlanEditor);
	mMenu.addAction(mPlanAction);


	// Online status
	//////////////////
	mOnlineAction->setStatusTip(tr("Toggle availability of Agent"));
	mOnlineAction->setCheckable(true);

	if(nullptr!=s) {
		mOnlineAction->setChecked(s->getCustomSettingBool("octomy.online"));
	}
	connect(mOnlineAction, &QAction::triggered, this, &AgentWindow::onOnlineChanged);
	mMenu.addAction(mOnlineAction);

	// Online button
	//////////////////
	mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
	mShowOnlineButtonAction->setCheckable(true);

	if(nullptr!=s) {
		mShowOnlineButtonAction->setChecked(s->getCustomSettingBool("octomy.online.show"));
	}
	connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
	mMenu.addAction(mShowOnlineButtonAction);

	// Show Face
	//////////////////

	mShowFaceAction->setStatusTip(tr("Show face in main screen"));
	mShowFaceAction->setCheckable(true);

	if(nullptr!=s) {
		mShowFaceAction->setChecked(s->getCustomSettingBool("octomy.face"));
	}
	connect(mShowFaceAction, &QAction::triggered, this, &AgentWindow::onFaceVisibilityChanged);
	mMenu.addAction(mShowFaceAction);

	// Show Log scroll
	//////////////////
	mShowLogAction->setStatusTip(tr("Show log in main screen"));
	mShowLogAction->setCheckable(true);
	if(nullptr!=s) {
		mShowLogAction->setChecked(s->getCustomSettingBool("octomy.debug.log"));
	}
	connect(mShowLogAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
	mMenu.addAction(mShowLogAction);

	// Show Stats
	//////////////////
	mShowStatsAction->setStatusTip(tr("Show stats in main screen"));
	mShowStatsAction->setCheckable(true);
	if(nullptr!=s) {
		mShowStatsAction->setChecked(s->getCustomSettingBool("octomy.debug.stats"));
	}
	connect(mShowStatsAction, &QAction::triggered, this, &AgentWindow::onStatsVisibilityChanged);
	mMenu.addAction(mShowStatsAction);

	// Show birth certificate
	///////////////////////////
	mShowBirthCertificateAction->setStatusTip(tr("Show the birth certificate of this agent"));
	mShowBirthCertificateAction->setIcon(QIcon(":/icons/certificate.svg"));
	connect(mShowBirthCertificateAction, &QAction::triggered, this, &AgentWindow::onStartShowBirthCertificate);
	mMenu.addAction(mShowBirthCertificateAction);

	// Unbirth
	//////////////////
	mUnbirthAction->setStatusTip(tr("Delete the identity of this agent to restart birth"));
	mUnbirthAction->setIcon(QIcon(":/icons/kill.svg"));
	connect(mUnbirthAction, &QAction::triggered, this, [=]() {
		if(nullptr!=mAgent) {
			QMessageBox::StandardButton reply = QMessageBox::question(this, "Unbirth", "Are you sure you want to DELETE the personality of this robot forever?", QMessageBox::No|QMessageBox::Yes);
			if (QMessageBox::Yes==reply) {
				QSharedPointer<NodeAssociate> assID=mAgent->nodeIdentity();
				if(nullptr!=assID) {
					mAgent->peers().removeParticipant(assID->id());
				} else {
					qWarning()<<"WARNING: there was no assID during unbirth";
				}
				mAgent->peers().save();
				mAgent->keyStore().clear();
				updateIdentity();
				ui->widgetDelivery->reset();
				ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
				qDebug()<<"UNBIRTHED!";
			}
		}
	});
	mMenu.addAction(mUnbirthAction);
}





CommsChannel *AgentWindow::comms()
{
	OC_METHODGATE();
	if(nullptr!=mAgent) {
		return mAgent->comms();
	}
	return nullptr;
}

QSet<QSharedPointer<NodeAssociate> > AgentWindow::activeControls()
{
	QSet<QSharedPointer<NodeAssociate> > out;

	const quint64 lastActive=QDateTime::currentMSecsSinceEpoch()-(1000*60);//One minute ago. TODO: Turn into constant or setting

	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		NodeAssociateStore &peers=mAgent->peers();
		ClientDirectory *cd=cc->clients();
		if(nullptr!=cd) {
			QSet<QSharedPointer<Client> > clients=cd->getByActiveTime(lastActive);
			for(QSharedPointer<Client> client: clients) {
				ClientSignature &sig=client->signature();
				QString id=sig.fullID();
				if(id.size()>0) {
					QSharedPointer<NodeAssociate> peer=peers.getParticipant(id);
					if(nullptr!=peer) {
						if(DiscoveryRole::ROLE_CONTROL==peer->role()) {
							out<<peer;
						}
					}
				}
			}
		}
	}

	return out;
}

void AgentWindow::courierRegistration(QSharedPointer<NodeAssociate> assoc, bool reg)
{
	/*
	 * //TODO: Find and squish strange new const QString * build error
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		if(nullptr != assoc) {
			const QString id=assoc->id();
			if(reg) {
				qDebug()<<"REGISTERING COURIERS FOR " <<id;
				QSet< QSharedPointer<Courier> > set;
				Courier *courier=new SensorsCourier;
				set<< QSharedPointer<Courier>(courier);
				mCourierSets.insert(id, set);
				cc->registerCourier(*courier);
			} else {
				qDebug()<<"UN-REGISTERING COURIERS FOR " <<id;
				if(mCourierSets.contains(id)) {
					QSet< QSharedPointer<Courier> > set=mCourierSets.take(id);
					for(QSharedPointer<Courier>  courier:set) {
						if(nullptr!=courier) {
							cc->unregisterCourier(*courier);
						}
					}
				}
			}
		}
		// Adaptively start commschannel when there are couriers registered
		const int ct=cc->courierCount();
		if(ct>0) {
			if(nullptr != assoc) {
				cc->start(assoc->localAddress());
			}
		} else {
			cc->stop();
		}
	}
	*/
}

void AgentWindow::updateCourierRegistration()
{
	QSet<QSharedPointer<NodeAssociate> > active=activeControls();
	if(active!=mLastActiveControls) {
		for(QSharedPointer<NodeAssociate> assoc:mLastActiveControls) {
			if(!active.contains(assoc)) {
				//Decomission it
				courierRegistration(assoc,false);
			}
		}
		for(QSharedPointer<NodeAssociate> assoc:active) {
			if(!mLastActiveControls.contains(assoc)) {
				//Comission it
				courierRegistration(assoc,true);
			}
		}
	}
	mLastActiveControls.clear();
	mLastActiveControls=active;
}




void AgentWindow::updateFaceVisibility()
{
	ui->widgetFace->updateVisibility();
}


// From Face widget ///////////////////////////////////////////////////

void AgentWindow::onConnectionStateChanged(const TryToggleState last, const TryToggleState current)
{
	//appendLog("CONNECT BUTTON TRYSTATE CHANGED FROM " +ToggleStateToSTring(last) +" TO "+ToggleStateToSTring(current));
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	const bool on=(OFF!=current);
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online",on);
	}
	updateOnlineStatus();
}


void AgentWindow::onColorChanged(QColor c)
{
//	mAgent->onC
}

void AgentWindow::onPanic()
{

}


// From Actions ///////////////////////////////////////////////////

void AgentWindow::onOnlineChanged(bool on)
{
	qDebug()<<"ONLINE ACTION CHANGED TO "<<(on?"ONLINE":"OFFLINE");
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online",on);
	}
	updateOnlineStatus();
}


void AgentWindow::onOnlineButtonVisibilityChanged(bool on)
{
	qDebug()<<"ONLINE BUTTON IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.online.show",on);
	}
	updateFaceVisibility();
}

void AgentWindow::onFaceVisibilityChanged(bool on)
{
	qDebug()<<"FACE VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.face",on);
	}
	updateFaceVisibility();
}


void AgentWindow::onLogVisibilityChanged(bool on)
{
	qDebug()<<"LOG VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.debug.log",on);
	}
	updateFaceVisibility();
}

void AgentWindow::onStatsVisibilityChanged(bool on)
{
	qDebug()<<"STATS VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool("octomy.debug.stats",on);
	}
	updateFaceVisibility();
}


void AgentWindow::onStartShowBirthCertificate()
{
	PortableID pid;
	if(nullptr!=mAgent) {
		QString id=mAgent->keyStore().localKey().id();
		QSharedPointer<NodeAssociate> myAss=mAgent->peers().getParticipant(id);
		if(nullptr!=myAss) {
			pid=myAss->toPortableID();
		} else {
			qWarning()<<"ERROR: No ass";
		}

	}
	ui->widgetBirthCertificate->setPortableID(pid);
	ui->stackedWidget->setCurrentWidget(ui->pageBirthCertificate);
}


void AgentWindow::onStartCameraPairing()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCamera);
}

void AgentWindow::onStartPairing()
{
	ui->widgetPairing->reset();
	ui->stackedWidget->setCurrentWidget(ui->pagePairing);
}


void AgentWindow::onStartHardware()
{
	ui->widgetHardware->reset();
	ui->stackedWidget->setCurrentWidget(ui->pageHardware);
}



void AgentWindow::onStartPlanEditor()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePlan);
}


void AgentWindow::appendLog(const QString& text)
{
	ui->widgetFace->appendLog(text);
}




void AgentWindow::keyReleaseEvent(QKeyEvent *e)
{
	if(Qt::Key_Back==e->key()) {
		/*
		if(ui->pageConnect==ui->stackedWidget->currentWidget()) {
			appendLog("EXITING APP ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConfirmQuit);
		} else if(ui->pageRunning==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		} else if(ui->pageConfirmQuit==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO CONNECTION SCREEN ON BACK BUTTON");
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		} else {
			appendLog("ERROR ON BACK BUTTON");
		}
		*/
		e->accept();
	} else {
		//appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}


void AgentWindow::updateOnlineStatus()
{
	if(nullptr!=mAgent) {
		// Find if we ARE online
		bool isOnline=false;
		if(nullptr!=mAgent) {
			isOnline=mAgent->isCommsConnected();
		}
		// Find if we WANT to be online
		bool wantToBeOnline=false;
		Settings *s=&mAgent->settings();
		if(nullptr!=s) {
			wantToBeOnline=s->getCustomSettingBool("octomy.online", false);
		}
		//Spell it out for debugging
		//qDebug()<<"We are currently "<<(isOnline?"ONLINE":"OFFLINE")<<" and we want to be "<<(wantToBeOnline?"ONLINE":"OFFLINE")<<".";
		// Make necessary changes to state
		const TryToggleState current=ui->widgetFace->connectionState();
		TryToggleState next=current;
		if(wantToBeOnline ) {
			if(isOnline ) {
				next=ON;
			} else {
				next=TRYING;
				//qDebug()<<"Decided to start comms";
				QTimer::singleShot(1000,[this]() {
					QSharedPointer<NodeAssociate> ni=mAgent->nodeIdentity();
					if(nullptr!=ni) {
						mAgent->start(ni->localAddress());
					}
				});
			}
		} else {
			if(isOnline ) {
				//qDebug()<<"Decided to stop comms";
				mAgent->stop();
			} else {
				next=OFF;
			}
		}
		if(next!=current) {
			//qDebug()<<"Decided to change tristate button from "<<current<<" -> "<<next;
			ui->widgetFace->setConnectionState(next,false);
		} else {
			//qDebug()<<"No change tristate button ("<<current<<")";
		}
		// Propagate the possibly changed state to UI
		updateFaceVisibility();
		mOnlineAction->setChecked(wantToBeOnline);
	}

}

///////////////////////////////////////////////////////////////////////////////

void AgentWindow::notifyAndroid(QString s)
{
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIF: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"notify",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void AgentWindow::toastAndroid(QString s)
{
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"toast",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void AgentWindow::on_pushButtonBack_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}


void AgentWindow::on_pushButtonMenu_clicked()
{
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}




//////////////////////////////////////////////////
// CommsChannel slots


void AgentWindow::onCommsError(QString e)
{
	//qDebug()<<"AgentWindow UNIMP Comms error: "<<e;
}

void AgentWindow::onCommsClientAdded(Client *c)
{
	//qDebug()<<"AgentWindow UNIMP Client added: "<<c->toString();
}

void AgentWindow::onCommsConnectionStatusChanged(bool s)
{
	//qDebug() <<"AGENT WINDOW New connection status: "<<(s?"ONLINE":"OFFLINE");
	updateOnlineStatus();
}

//////////////////////////////////////////////////
// UI slots


void AgentWindow::on_pushButtonConfirmQuit_clicked()
{
	exit(0);
}

void AgentWindow::on_pushButtonBack_2_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_4_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_3_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_5_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_6_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}
