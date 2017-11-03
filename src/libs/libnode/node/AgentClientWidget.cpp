#include "RemoteClientWidget.hpp"
#include "ui_ClientWidget.h"

#include "utility/Standard.hpp"

#include "basic/GenericKeyEventHandler.hpp"
#include "basic/Settings.hpp"

#include "widgets/WaitingSpinnerWidget.hpp"
#include "audio/OneOffSpeech.hpp"

#include "security/PortableID.hpp"

#include "widgets/CarSteeringWidget.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/ISyncParameter.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "comms/couriers/SensorsCourier.hpp"
#include "comms/couriers/blob/BlobCourier.hpp"

#include "agent/AgentConstants.hpp"

#include "node/RemoteClient.hpp"

#include <QScrollBar>

static const QString NODE_ONLINE_SETTINGS_KEY_BASE("octomy.online.");

RemoteClientWidget::RemoteClientWidget(QSharedPointer<RemoteClient> client, QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ClientWidget)
	, mSpinner(nullptr)
	, mRemoteClient(client)
{
	OC_METHODGATE();
	qDebug()<<"CREATING REMOTE CLIENT WIDGET RemoteClient="<<(!mRemoteClient.isNull()?mRemoteClient->node()->name():"NULL")<<", parent="<<parent;

	ui->setupUi(this);

	ui->widgetBirthCertificate->configure(false,true);

	if(!mRemoteClient.isNull()) {
		QSharedPointer<Node> node=mRemoteClient->node();
		if(!node.isNull()) {
			QSharedPointer<Associate> nodeAssociate=node->nodeIdentity();
			if(!nodeAssociate.isNull()) {
				ui->widgetBirthCertificate->setPortableID(nodeAssociate->toPortableID());
			}
			node->setHookCommsSignals(*this, true);
		} else {
			qWarning()<<"ERROR: no associate";
		}


	} else {
		qWarning()<<"ERROR: no client";
	}

	ui->widgetFace->setDisabled(true);
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", AgentConstants::AGENT_CONNECT_BUTTON_COLOR, AgentConstants::AGENT_DISCONNECT_COLOR);
	ui->tryToggleListen->setState(OFF,false);

	if(!connect(ui->tryToggleListen, &TryToggle::stateChanged, this, &RemoteClientWidget::onConnectButtonStateChanged ,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	//TODO: Handle this
//	const QString fullID=mNodeAssoc->id();
//	qDebug()<<"REMOTE CLIENT WIDGET DESTINATION SET TO "<<fullID;
//	mAgentStateCourier->setDestination(fullID);
//	mSensorsCourier->setDestination(fullID);
//	mBlobCourier->setDestination(fullID);


//	if(nullptr!=mAgentStateCourier) {
//		mAgentStateCourier->setHookSignals(*this,true);
//	} else {
//		qWarning()<<"ERROR: Could not hook agent state courier events, no courier";
//	}


	//installEventFilter(this);
	init();

	updateOnlineStatus();
}

RemoteClientWidget::~RemoteClientWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void RemoteClientWidget::prepareSpinner()
{
	OC_METHODGATE();
	mSpinner=OC_NEW WaitingSpinnerWidget(ui->stackedWidgetControl, true, true);
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

void RemoteClientWidget::prepareSteering()
{
	OC_METHODGATE();
	if(!connect(ui->widgetCarSteering, &CarSteeringWidget::steeringChanged, this, &RemoteClientWidget::onSteeringChanged ,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void RemoteClientWidget::setSpinnerActive(bool active)
{
	OC_METHODGATE();
	if(nullptr!=mSpinner) {
		mSpinner->setStarted( active);
	}
}


// NOTE: This has a sister method in AgentWindow.cpp around line 625
//       Please remember that while they are similar they are very different!
//       While the other one is a "One agent, one remote" deal
//       this is a "one remote, many agents" deal.
void RemoteClientWidget::updateOnlineStatus()
{
	OC_METHODGATE();
	if(!mRemoteClient.isNull()) {
		mRemoteClient->updateOnlineStatus();
	}
}


/*
bool RemoteClientWidget::courierRegistration()
{
	OC_METHODGATE();
	return mCouriers.commsEnabled();
}

void RemoteClientWidget::setCourierRegistration(bool reg)
{
	OC_METHODGATE();
	mCouriers.setCommsEnabled(reg);
}
*/


bool RemoteClientWidget::setSetting(QString key, bool val)
{
	OC_METHODGATE();
	QSharedPointer<Node> ctl=controller();
	if(!ctl.isNull()) {
		Settings&s=ctl->settings();
		s.setCustomSettingBool(key,val);
		return true;
	} else {
		qWarning()<<"ERROR: No controller";
	}
	return false;
}

bool RemoteClientWidget::needConnection()
{
	OC_METHODGATE();
	if(!controller().isNull() && !nodeAssociate().isNull()) {
		Settings *s=&controller()->settings();
		if(nullptr!=s) {
			return s->getCustomSettingBool(NODE_ONLINE_SETTINGS_KEY_BASE+nodeAssociate()->name(), false);
		}
	}
	return false;
}



void RemoteClientWidget::init()
{
	OC_METHODGATE();
//	ui->stackedWidgetControl->setUpdatesEnabled(false);
	prepareSpinner();
	prepareSteering();
	if(!controller().isNull()) {
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


CommsChannel *RemoteClientWidget::comms()
{
	OC_METHODGATE();
	if(!mRemoteClient.isNull() && !mRemoteClient->node().isNull()) {
		return mRemoteClient->node()->comms();
	}
	return nullptr;
}



QSharedPointer<Node> RemoteClientWidget::controller()
{
	OC_METHODGATE();
	return mRemoteClient.isNull()?nullptr:mRemoteClient->node();
}


QSharedPointer<Associate> RemoteClientWidget::nodeAssociate() const
{
	OC_METHODGATE();
	return mRemoteClient.isNull()?nullptr:mRemoteClient->nodeAssociate();
}

void RemoteClientWidget::updateControlLevel(int level)
{
	//qDebug()<<"CONTROL LEVEL IS "<<level;
	ui->stackedWidgetControl->setCurrentIndex(level);
	//Enable spinner when any level is selected except connect and we are not conneted
	//setSpinnerActive(level!=0 && ON != ui->tryToggleListen->state());
}


bool RemoteClientWidget::eventFilter(QObject *object, QEvent *event)
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



///////////////////////////////////////// // Internal UI slots


void RemoteClientWidget::onConnectButtonStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	//qDebug()<< "CONNECT BUTTON TRYSTATE CHANGED FROM " << ToggleStateToSTring(last)<< " TO " << ToggleStateToSTring(current);
	if(current!=last) {
		QSharedPointer<Associate> nodeAssoc=nodeAssociate();
		if(!nodeAssoc.isNull()) {
			const bool on=(OFF!=current);
			setSetting("octomy.online."+nodeAssoc->name(), on);
		}
	} else {
		qWarning()<<"ERROR: no nodeAssociate";
	}
	updateOnlineStatus();
}

void RemoteClientWidget::onSteeringChanged(qreal throttle, qreal steeringAngle)
{
	OC_METHODGATE();
	// TODO Upgrade this
	/*
	//qDebug()<<"THROT: "<<throttle<<" STEER: "<<steeringAngle;
	QSharedPointer<AgentStateCourier> asc=mCouriers.agentStateCourier();
	if(!asc.isNull()) {
		asc->setPoseValue(0,throttle);
		asc->setPoseValue(1,steeringAngle);
	} else {
		qWarning()<<"ERROR: no ASC when changing THROT: "<<throttle<<" STEER: "<<steeringAngle;
	}
	*/
}



void RemoteClientWidget::on_checkBoxShowEyes_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO Upgrade this
	/*
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.face",checked);
		s.setCustomSettingBool("octomy.online.show",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
	*/
}

void RemoteClientWidget::on_checkBoxShowStats_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO Upgrade this
	/*
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.debug.stats",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
	*/
}

void RemoteClientWidget::on_checkBoxShowLog_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO Upgrade this
	/*
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.debug.log",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
	*/
}

void RemoteClientWidget::on_checkBoxShowOnlineButton_toggled(bool checked)
{
	OC_METHODGATE();
	// TODO Upgrade this
	/*
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.online.show",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
	*/
}



void RemoteClientWidget::on_widgetPanic_toggled(bool panic)
{
	OC_METHODGATE();
	if(panic) {
		QString str="P A N I C !";
		qWarning()<<str;
	//	appendLog(str);
	} else {
		QString str="Panic averted";
		qWarning()<<str;
//		appendLog(str);
	}
	// TODO Upgrade this
	/*
	QSharedPointer<AgentStateCourier> asc=mCouriers.agentStateCourier();
	if(asc.isNull()) {
		asc->setPanic(panic);
	} else {
		qWarning()<<"ERROR: no ASC when changing panic to "<<panic;
	}
	*/

}
