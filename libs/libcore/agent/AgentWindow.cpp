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
#include "comms/CommsSessionDirectory.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/SyncParameter.hpp"

#include <QDebug>
#include <QMessageBox>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif



#include "puppet/NameMapping.hpp"
#include "widgets/NameMappingWidget.hpp"

void fubar()
{
	QStringList from;
	QStringList to;
	from<<"Left Front Knee";
	from<<"Right Front Knee";
	from<<"Left Back Knee";
	from<<"Right Back Knee";
	/*
	from<<"Left Front Hip";
	from<<"Right Front Hip";
	from<<"Left Back Hip";
	from<<"Right Back Hip";
	*/
	to<<"Arne";
	to<<"Trude";
	to<<"Bjarne";
	to<<"Fridjof";
	to<<"Ferdinand";
	to<<"Nancy";
	to<<"Otto";
	to<<"Gustavo";
	NameMapping *mapping=new NameMapping();
	/*
		(*mapping)["Left Back Knee"]="Arne";
		(*mapping)["Left Back Hip"]="Bjarne";
		(*mapping)["Left Front Hip"]="Fridjof";
		(*mapping)["Right Back Knee"]="Fridjof";
		*/
	NameMappingWidget *nm=new NameMappingWidget();
	nm->configure(*mapping,from,to);
	nm->show();

}


AgentWindow::AgentWindow(Agent *agent, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgentWindow)
	, mAgent(agent)
	, mHexy(nullptr)
	, mCameraAction(new QAction(tr("Camera"), this))
	, mPairingAction(new QAction(tr("Pair"), this))
	, mHardwareAction(new QAction(tr("Configure HW"), this))
	, mPlanAction(new QAction(tr("Plan"), this))
	, mOnlineAction(new QAction(tr("Go Online"), this))
	, mShowOnlineButtonAction(new QAction(tr("Show Online Button"), this))
	, mShowFaceAction(new QAction(tr("Show Face"), this))
	, mShowLogAction(new QAction(tr("Show Log"), this))
	, mShowStatsAction(new QAction(tr("Show Stats"), this))
	, mShowBirthCertificateAction(new QAction(tr("Show Birth Certificate"), this))
	, mUnbirthAction(new QAction(tr("Unbirth!"), this))
	, mWasEverUndelivered(false)
{
	OC_METHODGATE();
	ui->setupUi(this);

	updateIdentity();

	if(nullptr!=mAgent) {
		//Settings &s=agent->settings();
		ui->widgetHardware->configure(mAgent->configurationStore());
		//Select correct starting page
		ui->widgetDelivery->reset();
		mWasEverUndelivered=!mAgent->keyStore().fileExists();
		ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
		gotoNextConfigPage();

		if(!QObject::connect(ui->widgetDelivery, &AgentDeliveryWizard::done, [=]() {
		updateIdentity();
			gotoNextConfigPage();
		} ) ) {
			qWarning()<<"ERROR: Could not connect ";
		}

		if(!connect(ui->widgetPairing, &PairingWizard::done, [=]() {
		gotoNextConfigPage();
		} )) {
			qWarning()<<"ERROR: Could not connect ";
		}


		if(!connect(ui->widgetHardware, &HardwareWizard::done, [=]() {
		gotoNextConfigPage();
		} )) {
			qWarning()<<"ERROR: Could not connect ";
		}


		mAgent->setHookCommsSignals(*this, true);
		mAgent->hookColorSignals(*ui->widgetFace);
		ui->widgetFace->hookSignals(*this);
		ui->widgetPlanEditor->configure("agent.plan");
		prepareMenu();
		updateFaceVisibility();

		updateOnlineStatus();
		//	fubar();
		//QString text="Hello, my name is "+mAgent->name()+". I am an octomy agent. What is your bidding master?";
		//QString text="Hello, my name is Bodhi. I am an octomy agent. What is your bidding master? 00 0 01010 010 010 010 010101 ";		PortableID id=mAgent->localNodeAssociate()->toPortableID();		new OneOffSpeech(id, text);
	}

#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

AgentWindow::~AgentWindow()
{
	OC_METHODGATE();
	delete ui;
}

void AgentWindow::gotoNextConfigPage()
{
	// Find next config screen to show
	QWidget *cur=ui->stackedWidget->currentWidget();
	const bool isDelivered=mAgent->keyStore().fileExists();
	auto ac=mAgent->configurationStore().agentConfig();
	const QString ctl=ac->controllerName();
	const bool isHardwareConfigured=(nullptr!=ac) && (""!=ctl) || (ui->pageHardware==cur);
	auto pct=mAgent->peers().getParticipantCount();
	const bool isPaired=(pct>1) || (ui->pagePairing==cur);

	qDebug()<<"NEXT CONFIG SCREEN: isDelivered"<<isDelivered<<", isPaired"<<isPaired<<", isHardwareConfigured"<<isHardwareConfigured<<" (ctlname="<<ctl<<", pct="<<pct<<")";
	if(isDelivered) {
		cur=ui->pageHardware;

	}

	// Only ask the first time
	if(!mWasEverUndelivered) {
		cur=ui->pageRunning;
	} else {
		if(isHardwareConfigured) {
			cur=ui->pagePairing;
		}
		if(isPaired) {
			cur=ui->pageRunning;
		}
	}
	ui->stackedWidget->setCurrentWidget(cur);

	// In case user skips through the whole thing, remind her
	ui->stackedWidgetFaceOrConfig->setCurrentWidget((mWasEverUndelivered || (isPaired && isHardwareConfigured))?ui->pageFace : ui->pageConfig);
	ui->widgetDeliveryStatus->setLightOn(isDelivered);
	ui->pushButtonDeliver->setEnabled(!isDelivered);
	ui->widgetHardwareStatus->setLightOn(isHardwareConfigured);
	ui->pushButtonConfigureHardware->setEnabled(!isHardwareConfigured);
	ui->widgetPairStatus->setLightOn(isPaired);
	ui->pushButtonPairWithControls->setEnabled(!isPaired);
}

void AgentWindow::updateIdentity()
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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


#include "agent/AgentConstants.hpp"

void AgentWindow::prepareMenu()
{
	OC_METHODGATE();
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
		mShowOnlineButtonAction->setChecked(s->getCustomSettingBool(AgentConstants::AGENT_CONNECTION_STATUS));
	}
	connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
	mMenu.addAction(mShowOnlineButtonAction);

	// Show Face
	//////////////////

	mShowFaceAction->setStatusTip(tr("Show face in main screen"));
	mShowFaceAction->setCheckable(true);

	if(nullptr!=s) {
		mShowFaceAction->setChecked(s->getCustomSettingBool(AgentConstants::AGENT_FACE_EYES_SHOW));
	}
	connect(mShowFaceAction, &QAction::triggered, this, &AgentWindow::onFaceVisibilityChanged);
	mMenu.addAction(mShowFaceAction);

	// Show Log scroll
	//////////////////
	mShowLogAction->setStatusTip(tr("Show log in main screen"));
	mShowLogAction->setCheckable(true);
	if(nullptr!=s) {
		mShowLogAction->setChecked(s->getCustomSettingBool(AgentConstants::AGENT_FACE_LOG_SHOW));
	}
	connect(mShowLogAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
	mMenu.addAction(mShowLogAction);

	// Show Stats
	//////////////////
	mShowStatsAction->setStatusTip(tr("Show stats in main screen"));
	mShowStatsAction->setCheckable(true);
	if(nullptr!=s) {
		mShowStatsAction->setChecked(s->getCustomSettingBool(AgentConstants::AGENT_FACE_STATS_SHOW));
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
				mWasEverUndelivered=true;
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
	OC_METHODGATE();
	QSet<QSharedPointer<NodeAssociate> > out;

	const quint64 lastActive=QDateTime::currentMSecsSinceEpoch()-(1000*60);//One minute ago. TODO: Turn into constant or setting

	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		NodeAssociateStore &peers=mAgent->peers();
		CommsSessionDirectory &sessionDirectory=cc->sessions();
		QSet<QSharedPointer<CommsSession> > sessions=sessionDirectory.getByActiveTime(lastActive);
		for(QSharedPointer<CommsSession> client: sessions) {
			CommsSignature &sig=client->signature();
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

	return out;
}

void AgentWindow::courierRegistration(QSharedPointer<NodeAssociate> assoc, bool reg)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
//	mAgent->onC
}

void AgentWindow::onPanic()
{
	OC_METHODGATE();

}


// From Actions ///////////////////////////////////////////////////

void AgentWindow::onOnlineChanged(bool on)
{
	OC_METHODGATE();
	qDebug()<<"ONLINE ACTION CHANGED TO "<<(on?"ONLINE":"OFFLINE");
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool(AgentConstants::AGENT_CONNECTION_STATUS,on);
	}
	updateOnlineStatus();
}


void AgentWindow::onOnlineButtonVisibilityChanged(bool on)
{
	OC_METHODGATE();
	qDebug()<<"ONLINE BUTTON IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool(AgentConstants::AGENT_FACE_ONLINE_BUTTON_SHOW,on);
	}
	updateFaceVisibility();
}

void AgentWindow::onFaceVisibilityChanged(bool on)
{
	OC_METHODGATE();
	qDebug()<<"FACE VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool(AgentConstants::AGENT_FACE_EYES_SHOW,on);
	}
	updateFaceVisibility();
}


void AgentWindow::onLogVisibilityChanged(bool on)
{
	OC_METHODGATE();
	qDebug()<<"LOG VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool(AgentConstants::AGENT_FACE_LOG_SHOW,on);
	}
	updateFaceVisibility();
}

void AgentWindow::onStatsVisibilityChanged(bool on)
{
	OC_METHODGATE();
	qDebug()<<"STATS VIS IS NOW: "<<on;
	Settings *s=(nullptr!=mAgent)?(&mAgent->settings()):nullptr;
	if(nullptr!=s) {
		s->setCustomSettingBool(AgentConstants::AGENT_FACE_STATS_SHOW,on);
	}
	updateFaceVisibility();
}



void AgentWindow::onStartShowBirthCertificate()
{
	OC_METHODGATE();
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



void AgentWindow::onStartDelivery()
{
	OC_METHODGATE();
	ui->widgetDelivery->reset();
	ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
}

void AgentWindow::onStartCameraPairing()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageCamera);
}

void AgentWindow::onStartPairing()
{
	OC_METHODGATE();
	ui->widgetPairing->reset();
	ui->stackedWidget->setCurrentWidget(ui->pagePairing);
}


void AgentWindow::onStartHardware()
{
	OC_METHODGATE();
	ui->widgetHardware->reset();
	ui->stackedWidget->setCurrentWidget(ui->pageHardware);
}



void AgentWindow::onStartPlanEditor()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pagePlan);
}


void AgentWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->widgetFace->appendLog(text);
}




void AgentWindow::keyReleaseEvent(QKeyEvent *e)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
			wantToBeOnline=s->getCustomSettingBool(AgentConstants::AGENT_CONNECTION_STATUS, false);
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


QSharedPointer<PoseMapping> AgentWindow::poseMapping()
{
	if(nullptr!=mAgent) {
		return mAgent->poseMapping();
	}
	return QSharedPointer<PoseMapping>(nullptr);
}


///////////////////////////////////////////////////////////////////////////////

void AgentWindow::notifyAndroid(QString s)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
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
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}


void AgentWindow::on_pushButtonMenu_clicked()
{
	OC_METHODGATE();
	mMenu.exec(mapToGlobal(ui->pushButtonMenu->pos()));
}



//////////////////////////////////////////////////
//Agent State Courier slots

void AgentWindow::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
	qDebug()<<"AgentWindow ASC changed: "<<sp->toString();
	const AgentControls &controls=mAgent->controls();
	CourierSet*set=controls.activeControl();
	if(nullptr!=set) {
		AgentStateCourier *asc=set->agentStateCourier();
		if(nullptr!=asc) {
			const bool panic=asc->panic();
			ui->widgetFace->setPanic(panic);
			ui->widgetFace->panic();
		}
	}
}




//////////////////////////////////////////////////
// CommsChannel slots


void AgentWindow::onCommsError(QString e)
{
	OC_METHODGATE();
	//qDebug()<<"AgentWindow UNIMP Comms error: "<<e;
}

void AgentWindow::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	//qDebug()<<"AgentWindow UNIMP Client added: "<<c->toString();
}

void AgentWindow::onCommsConnectionStatusChanged(bool s)
{
	OC_METHODGATE();
	//qDebug() <<"AGENT WINDOW New connection status: "<<(s?"ONLINE":"OFFLINE");
	updateOnlineStatus();
}

//////////////////////////////////////////////////
// UI slots


void AgentWindow::on_pushButtonConfirmQuit_clicked()
{
	OC_METHODGATE();
	exit(0);
}

void AgentWindow::on_pushButtonBack_2_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_4_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_3_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_5_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonBack_6_clicked()
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}

void AgentWindow::on_pushButtonSkipConfig_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
	ui->stackedWidgetFaceOrConfig->setCurrentWidget(ui->pageFace);
}

void AgentWindow::on_pushButtonPairWithControls_clicked()
{
	onStartPairing();
}

void AgentWindow::on_pushButtonConfigureHardware_clicked()
{
	onStartHardware();
}

void AgentWindow::on_pushButtonDeliver_clicked()
{
	onStartDelivery();
}
