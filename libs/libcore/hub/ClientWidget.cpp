#include "ClientWidget.hpp"
#include "ui_ClientWidget.h"

#include "../libutil/utility/Standard.hpp"

#include "basic/GenericKeyEventHandler.hpp"
#include "basic/Settings.hpp"

#include "widgets/WaitingSpinnerWidget.hpp"
#include "audio/OneOffSpeech.hpp"

#include "security/PortableID.hpp"

#include "../libcore/widgets/CarSteeringWidget.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/ISyncParameter.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/BlobCourier.hpp"

#include "agent/AgentConstants.hpp"

#include <QScrollBar>


ClientWidget::ClientWidget(QSharedPointer<Node> controller, QSharedPointer<NodeAssociate> nodeAssoc, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ClientWidget)
	, mController(controller)
	, mNodeAssoc(nodeAssoc)
	, mSpinner(nullptr)
	, mAgentStateCourier(new AgentStateCourier(nullptr, this))
	, mSensorsCourier(new SensorsCourier(this))
	, mBlobCourier(new BlobCourier(this))

{
	OC_METHODGATE();
	qDebug()<<"CREATING CLIENT WIDGET mController="<<(nullptr!=mController?mController->name():"NULL")<<", mNodeAssoc="<<(nullptr!=mNodeAssoc?mNodeAssoc->name():"NULL")<<", parent="<<parent;

	ui->setupUi(this);

	ui->widgetBirthCertificate->configure(false,true);
	if(nullptr!=mNodeAssoc) {
		ui->widgetBirthCertificate->setPortableID(mNodeAssoc->toPortableID());
	} else {
		qWarning()<<"ERROR: no client";
	}
	updateTimer.setInterval(100);
	if(!connect(&updateTimer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	updateTimer.start();
	ui->widgetFace->setDisabled(true);


	ui->tryToggleListen->configure("Connect","Connecting","Connected", AgentConstants::AGENT_CONNECT_BUTTON_COLOR);
	ui->tryToggleListen->setState(OFF,false);

	if(!connect(ui->tryToggleListen, &TryToggle::stateChanged, this, &ClientWidget::onConnectButtonStateChanged ,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	} else {
		qDebug()<<"CONNECTED onConnectButtonStateChanged";
	}

	CommsSignature sig=mNodeAssoc->toClientSignature();
	qDebug()<<"REMOTE CLIENT WIDGET DESTINATION SET TO "<<sig;
	mAgentStateCourier->setDestination(sig);
	mSensorsCourier->setDestination(sig);
	mBlobCourier->setDestination(sig);


	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setHookSignals(*this,true);
	} else {
		qWarning()<<"ERROR: Could not hook agent state courier events, no courier";
	}

	if(nullptr!=mController) {
		mController->setHookCommsSignals(*this, true);
	} else {
		qWarning()<<"ERROR: Could not hook controller events, no controller";
	}

	//installEventFilter(this);
	init();

	updateOnlineStatus();
}

ClientWidget::~ClientWidget()
{
	OC_METHODGATE();
	delete ui;
}

void ClientWidget::prepareSpinner()
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

void ClientWidget::prepareSteering()
{
	OC_METHODGATE();
	if(!connect(ui->widgetCarSteering, &CarSteeringWidget::steeringChanged, this, &ClientWidget::onSteeringChanged ,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	} else {
		qDebug()<<"CONNECTED onSteeringChanged";
	}

}


void ClientWidget::setSpinnerActive(bool active)
{
	OC_METHODGATE();
	if(nullptr!=mSpinner) {
		mSpinner->setStarted( active);
	}
}



void ClientWidget::updateOnlineStatus()
{
	OC_METHODGATE();
	//qDebug()<<"START UPDATE ONLINE STATUS # # # # ";
	if(nullptr!=mController && nullptr!=mNodeAssoc) {
		// Find if we ARE online
		bool isOnline=false;
		if(nullptr!=mController) {
			isOnline=mController->isCommsConnected() && courierRegistration();
		}
		// Find if we WANT to be online
		bool wantToBeOnline=false;
		Settings *s=&mController->settings();
		if(nullptr!=s) {
			wantToBeOnline=s->getCustomSettingBool("octomy.online."+mNodeAssoc->name(), false);
		}
		//Spell it out for debugging
		//qDebug()<<"We are currently "<<(isOnline?"ONLINE":"OFFLINE")<<" and we want to be "<<(wantToBeOnline?"ONLINE":"OFFLINE")<<".";
		// Make necessary changes to state
		const TryToggleState currentTryState=ui->tryToggleListen->state();
		TryToggleState nextTryState=currentTryState;
		bool nextOnlineStatus=isOnline;
		if(wantToBeOnline ) {
			if(isOnline ) {
				nextTryState=ON;
			} else {
				nextTryState=TRYING;
				//qDebug()<<"Decided to go online";
				nextOnlineStatus=true;
			}
		} else {
			if(isOnline ) {
				//qDebug()<<"Decided to go offline";
				nextOnlineStatus=false;
			} else {
				nextTryState=OFF;
			}
		}
		if(nextOnlineStatus!=isOnline) {
			//qDebug()<<"Decided to change online status from "<<isOnline<<" -> "<<nextOnlineStatus;
			setCourierRegistration(nextOnlineStatus);
		} else {
			//qDebug()<<"No change in online status ("<<nextOnlineStatus<<")";
		}
		if(nextTryState!=currentTryState) {
			//qDebug()<<"Decided to change tristate button from "<<currentTryState<<" -> "<<nextTryState;
			ui->tryToggleListen->setState(nextTryState,false);
		} else {
			//qDebug()<<"No change tristate button ("<<nextTryState<<")";
		}
	}
	//qDebug()<<"END UPDATE ONLINE STATUS # # # # ";
}


bool ClientWidget::courierRegistration()
{
	OC_METHODGATE();
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		return cc->hasCourier(*mSensorsCourier);
	}
	return false;
}

void ClientWidget::setCourierRegistration(bool reg)
{
	OC_METHODGATE();
	CommsChannel *cc=comms();
	if(nullptr!=cc) {

		//qDebug()<<"REGISTERING SENSORS COURIER FOR " <<mNodeAssoc->id();
		cc->setCourierRegistered(*mAgentStateCourier, reg);
		cc->setCourierRegistered(*mSensorsCourier, reg);
		cc->setCourierRegistered(*mBlobCourier, reg);
		// Adaptively start commschannel when there are couriers registered
		const int ct=cc->courierCount();
		//qDebug()<<"COMMS LEFT WITH "<<ct<<" COURIERS";
		//qDebug()<< cc->getSummary();
		if(ct>0) {
			if( (nullptr != mController) && (nullptr!= mController->nodeIdentity() ) && (!cc->isStarted()) ) {
				//qDebug()<<"STARTING COMMS ";
				cc->start(mController->nodeIdentity()->localAddress());
			} else {
				//qDebug()<<"COMMS ALREADY STARTED";
			}
		} else {
			if( cc->isStarted() ) {
				//qDebug()<<"STOPPING COMMS ";
				cc->stop();
			} else {
				//qDebug()<<"COMMS ALREADY STOPPED";
			}
		}
	}
}

void ClientWidget::init()
{
	OC_METHODGATE();
//	ui->stackedWidgetControl->setUpdatesEnabled(false);
	prepareSpinner();
	prepareSteering();
	if(nullptr!=mController) {
//		Settings &s=mController->settings();
		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		ui->labelTouch->setText("WAITING FOR TOUCH");
		//ui->widgetActuatorControl->configure(10);
	} else {
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
		qWarning()<<"ERROR: No context provider set!";
	}
//	ui->stackedWidgetControl->setUpdatesEnabled(true);
}


CommsChannel *ClientWidget::comms()
{
	OC_METHODGATE();
	if(nullptr!=mController) {
		return mController->comms();
	}
	return nullptr;
}

QSharedPointer<NodeAssociate> ClientWidget::nodeAssoc() const
{
	OC_METHODGATE();
	return mNodeAssoc;
}

void ClientWidget::updateControlLevel(int level)
{
	//qDebug()<<"CONTROL LEVEL IS "<<level;
	ui->stackedWidgetControl->setCurrentIndex(level);
	//Enable spinner when any level is selected except connect and we are not conneted
	//setSpinnerActive(level!=0 && ON != ui->tryToggleListen->state());
}


bool ClientWidget::eventFilter(QObject *object, QEvent *event)
{
	OC_METHODGATE();
	/*

	//qDebug()<<"EVENT: "<<object<<": "<<event;
	if ((u==object) && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress )) {
		QMouseEvent *r= static_cast<QMouseEvent *>(event);
		ui->labelTouch->setText("TOUCH: <"+QString::number(r->globalX())+", "+QString::number(r->globalY())+">");
		QPointF p=r->localPos();
		QSize s=size();
		//mRemote->onTouchUpdated(QVector2D(p.x()/(qreal)s.width(), p.y()/(qreal)s.height()));
	}
	//object == ui->widgetBackground && (
	*/
	return false;
}


//////////////////////////////////////////////////
// Agent State Courier slots

void ClientWidget::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
	qDebug()<<"ClientWidget ASC: ON VALUE CHANGED: "<<sp->toString();
	if(nullptr!=mAgentStateCourier) {
		const bool panic=mAgentStateCourier->panic();
		ui->widgetPanic->setPanic(panic);
	}
}

//////////////////////////////////////////////////
// CommsChannel slots


void ClientWidget::onCommsError(QString e)
{
	OC_METHODGATE();
	qDebug()<<"ClientWidget UNIMP Comms error: "<<e;
}

void ClientWidget::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	//qDebug()<<"ClientWidget UNIMP Client added: "<<c->toString();
}

void ClientWidget::onCommsConnectionStatusChanged(bool s)
{
	OC_METHODGATE();
	//qDebug() <<"CLIENT WIDGET New connection status: "<<(s?"ONLINE":"OFFLINE");
	updateOnlineStatus();
}


///////////////////////////////////////// // Internal slots


void ClientWidget::onUpdateTimer()
{
	OC_METHODGATE();
	//qDebug()<<"TIME for summary";
}



void ClientWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}




///////////////////////////////////////// // Internal UI slots

void ClientWidget::onConnectButtonStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	//qDebug()<< "CONNECT BUTTON TRYSTATE CHANGED FROM " << last<< " TO " << current;
	if(current!=last) {
		if(nullptr!= mNodeAssoc) {
			Settings *s=&mController->settings();
			if(nullptr!=s) {
				s->setCustomSettingBool("octomy.online."+mNodeAssoc->name(), OFF!=current);
			}
		}
	}
	updateOnlineStatus();
}

void ClientWidget::onSteeringChanged(qreal throttle, qreal steeringAngle)
{
	OC_METHODGATE();
	//qDebug()<<"THROT: "<<throttle<<" STEER: "<<steeringAngle;
	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setPoseValue(0,throttle);
		mAgentStateCourier->setPoseValue(1,steeringAngle);
	} else {
		qWarning()<<"ERROR: no ASC when changing THROT: "<<throttle<<" STEER: "<<steeringAngle;
	}
}



void ClientWidget::on_checkBoxShowEyes_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.face",checked);
		s.setCustomSettingBool("octomy.online.show",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
}

void ClientWidget::on_checkBoxShowStats_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.debug.stats",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
}

void ClientWidget::on_checkBoxShowLog_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.debug.log",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
}

void ClientWidget::on_checkBoxShowOnlineButton_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.online.show",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
}



void ClientWidget::on_widgetPanic_toggled(bool panic)
{
	OC_METHODGATE();
	if(panic) {
		QString str="P A N I C !";
		qWarning()<<str;
		appendLog(str);
	} else {
		QString str="Panic averted";
		qWarning()<<str;
		appendLog(str);
	}
	if(nullptr!=mAgentStateCourier) {
		mAgentStateCourier->setPanic(panic);
	} else {
		qWarning()<<"ERROR: no ASC when changing panic to "<<panic;
	}

}
