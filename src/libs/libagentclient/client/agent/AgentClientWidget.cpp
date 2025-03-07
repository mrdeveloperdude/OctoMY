#include "AgentClientWidget.hpp"
#include "ui_AgentClientWidget.h"

#include "app/Constants.hpp"
#include "app/Settings.hpp"
#include "client/agent/AgentClient.hpp"
#include "comms/CommsChannel.hpp"
#include "components/TryToggleState.hpp"
#include "components/WaitingSpinnerWidget.hpp"
#include "node/CarSteeringWidget.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
//#include "agent/AgentConstants.hpp"

#include <QScrollBar>

AgentClientWidget::AgentClientWidget(QSharedPointer<AgentClient> client, QWidget *parent)
	: ClientWidget(parent)
	, ui(OC_NEW Ui::AgentClientWidget)
	, mSpinner(nullptr)
	, mAgentClient(client)
{
	OC_METHODGATE();
	ui->setupUi(this);
	qDebug()<<"CREATING AGENT CLIENT WIDGET AgentClient="<<(!mAgentClient.isNull()?mAgentClient->node()->nodeIdentity()->name():"NULL")<<", parent="<<parent;
	ui->widgetBirthCertificate->configure(nullptr, false, true);
	if(!mAgentClient.isNull()) {
		auto node=mAgentClient->node();
		if(!node.isNull()) {
			node->setHookCommsSignals(*mAgentClient.data(), true);
			ui->widgetSpeechControl->configure(node);
		} else {
			qWarning()<<"ERROR: no associate";
		}
		QSharedPointer<Associate> ass=mAgentClient->associate();
		if(!ass.isNull()) {
			ui->widgetBirthCertificate->setPortableID(ass->toPortableID());
		}
	} else {
		qWarning()<<"ERROR: no client";
	}
	
	ui->widgetFace->setDisabled(true);
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	ui->tryToggleListen->setState(OFF,false);
	
	if(!connect(ui->tryToggleListen, &TryToggle::stateChanged, this, &AgentClientWidget::onConnectButtonStateChanged, OC_CONTYPE)) {
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
	ui->stackedWidgetControl->setCurrentWidget(ui->pageConnection);
}

AgentClientWidget::~AgentClientWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void AgentClientWidget::prepareSpinner()
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

void AgentClientWidget::prepareSteering()
{
	OC_METHODGATE();
	if(!connect(ui->widgetCarSteering, &CarSteeringWidget::steeringChanged, this, &AgentClientWidget::onSteeringChanged,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void AgentClientWidget::setSpinnerActive(bool active)
{
	OC_METHODGATE();
	if(nullptr!=mSpinner) {
		mSpinner->setStarted( active);
	}
}


QString AgentClientWidget::id()
{
	OC_METHODGATE();
	if(!mAgentClient.isNull()) {
		QSharedPointer<Associate> na=mAgentClient->associate();
		if(!na.isNull()) {
			return na->id();
		}
	}
	return "";
}


// NOTE: This has a sister method in AgentWindow.cpp around line 625
//       Please remember that while they are similar they are very different!
//       While the other one is a "One agent, one remote" deal
//       this is a "one remote, many agents" deal.
void AgentClientWidget::updateOnlineStatus()
{
	OC_METHODGATE();
	if(!mAgentClient.isNull()) {
		mAgentClient->updateCourierRegistration();
	}
}


bool AgentClientWidget::setSetting(QString key, bool val)
{
	OC_METHODGATE();
	QSharedPointer<Node> ctl=controller();
	if(!ctl.isNull()) {
		QSharedPointer<Settings> s=ctl->settings();
		if(!s.isNull()) {
			s->setCustomSettingBool(key, val);
		}
		return true;
	} else {
		qWarning()<<"ERROR: No controller";
	}
	return false;
}

/*
bool AgentClientWidget::needConnection()
{
	OC_METHODGATE();
	if(!controller().isNull() && !nodeAssociate().isNull()) {
		Settings *s=&controller()->settings();
		if(nullptr!=s) {
			return s->getCustomSettingBool(CLIENT_ONLINE_STATUS_BASE_KEY+nodeAssociate()->name(), false);
		}
	}
	return false;
}
*/


void AgentClientWidget::init()
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


QSharedPointer<CommsChannel> AgentClientWidget::comms()
{
	OC_METHODGATE();
	if(!mAgentClient.isNull() && !mAgentClient->node().isNull()) {
		return mAgentClient->node()->comms();
	}
	return nullptr;
}



QSharedPointer<Node> AgentClientWidget::controller()
{
	OC_METHODGATE();
	return mAgentClient.isNull()?nullptr:mAgentClient->node();
}


QSharedPointer<Associate> AgentClientWidget::nodeAssociate() const
{
	OC_METHODGATE();
	return mAgentClient.isNull()?nullptr:mAgentClient->associate();
}

void AgentClientWidget::updateControlLevel(int level)
{
	OC_METHODGATE();
	//qDebug()<<"CONTROL LEVEL IS "<<level;
	ui->stackedWidgetControl->setCurrentIndex(level);
	//Enable spinner when any level is selected except connect and we are not conneted
	setSpinnerActive(level!=0 && ON != ui->tryToggleListen->state());
}


bool AgentClientWidget::eventFilter(QObject *object, QEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(object);
	Q_UNUSED(event);
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


void AgentClientWidget::onConnectButtonStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	qDebug()<< "AGENT CLIENT CONNECT BUTTON TRYSTATE CHANGED FROM " << ToggleStateToSTring(last)<< " TO " << ToggleStateToSTring(current);
	if(!mAgentClient.isNull()) {
		mAgentClient->setNeedsConnection( ( GOING_ON == current ) || (ON == current) );
	}
}

void AgentClientWidget::onSteeringChanged(qreal throttle, qreal steeringAngle)
{
	OC_METHODGATE();
	Q_UNUSED(throttle);
	Q_UNUSED(steeringAngle);
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



void AgentClientWidget::on_checkBoxShowEyes_toggled(bool checked)
{
	OC_METHODGATE();
	Q_UNUSED(checked);
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

void AgentClientWidget::on_checkBoxShowStats_toggled(bool checked)
{
	OC_METHODGATE();
	Q_UNUSED(checked);
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

void AgentClientWidget::on_checkBoxShowLog_toggled(bool checked)
{
	OC_METHODGATE();
	Q_UNUSED(checked);
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

void AgentClientWidget::on_checkBoxShowOnlineButton_toggled(bool checked)
{
	OC_METHODGATE();
	Q_UNUSED(checked);
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



void AgentClientWidget::on_widgetPanic_toggled(bool panic)
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
