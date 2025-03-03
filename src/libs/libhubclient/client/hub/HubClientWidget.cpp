#include "HubClientWidget.hpp"
#include "ui_HubClientWidget.h"

#include "app/Constants.hpp"
#include "app/Settings.hpp"
#include "client/hub/HubClient.hpp"
#include "comms/CommsChannel.hpp"
#include "components/WaitingSpinnerWidget.hpp"
#include "node/CarSteeringWidget.hpp"
//#include "security/PortableID.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
//#include "agent/AgentConstants.hpp"

#include <QScrollBar>

static const QString NODE_ONLINE_SETTINGS_KEY_BASE("octomy.online.");

HubClientWidget::HubClientWidget(QSharedPointer<HubClient> client, QWidget *parent)
	: ClientWidget(parent)
	, ui(OC_NEW Ui::HubClientWidget)
	, mSpinner(nullptr)
	, mHubClient(client)
{
	OC_METHODGATE();
	qDebug()<<"CREATING HUB CLIENT WIDGET RemoteClient="<<(!mHubClient.isNull()?mHubClient->node()->nodeIdentity()->name():"NULL")<<", parent="<<parent;

	ui->setupUi(this);

	ui->widgetBirthCertificate->configure(false,true);

	if(!mHubClient.isNull()) {
		QSharedPointer<Node> node=mHubClient->node();
		if(!node.isNull()) {
			ui->widgetSpeechControl->configure(node);
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
	ui->tryToggleListen->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	ui->tryToggleListen->setState(OFF,false);

	if(!connect(ui->tryToggleListen, &TryToggle::stateChanged, this, &HubClientWidget::onConnectButtonStateChanged,OC_CONTYPE)) {
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

HubClientWidget::~HubClientWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void HubClientWidget::prepareSpinner()
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

void HubClientWidget::prepareSteering()
{
	OC_METHODGATE();
	if(!connect(ui->widgetCarSteering, &CarSteeringWidget::steeringChanged, this, &HubClientWidget::onSteeringChanged,OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void HubClientWidget::setSpinnerActive(bool active)
{
	OC_METHODGATE();
	if(nullptr!=mSpinner) {
		mSpinner->setStarted( active);
	}
}


void HubClientWidget::updateOnlineStatus()
{
	OC_METHODGATE();
	if(!mHubClient.isNull()) {
		mHubClient->updateOnlineStatus();
	}
}



bool HubClientWidget::setSetting(QString key, bool val)
{
	OC_METHODGATE();
	QSharedPointer<Node> ctl=controller();
	if(!ctl.isNull()) {
		QSharedPointer<Settings> s=ctl->settings();
		s->setCustomSettingBool(key,val);
		return true;
	} else {
		qWarning()<<"ERROR: No controller";
	}
	return false;
}

bool HubClientWidget::needConnection()
{
	OC_METHODGATE();
	if(!controller().isNull() && !nodeAssociate().isNull()) {
		QSharedPointer<Settings> s=controller()->settings();
		if(!s.isNull()) {
			return s->getCustomSettingBool(NODE_ONLINE_SETTINGS_KEY_BASE+nodeAssociate()->name(), false);
		}
	}
	return false;
}



void HubClientWidget::init()
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


QSharedPointer<CommsChannel> HubClientWidget::comms()
{
	OC_METHODGATE();
	if(!mHubClient.isNull() && !mHubClient->node().isNull()) {
		return mHubClient->node()->comms();
	}
	return nullptr;
}



QSharedPointer<Node> HubClientWidget::controller()
{
	OC_METHODGATE();
	return mHubClient.isNull()?nullptr:mHubClient->node();
}


QSharedPointer<Associate> HubClientWidget::nodeAssociate() const
{
	OC_METHODGATE();
	return mHubClient.isNull()?nullptr:mHubClient->associate();
}

void HubClientWidget::updateControlLevel(int level)
{
	//qDebug()<<"CONTROL LEVEL IS "<<level;
	ui->stackedWidgetControl->setCurrentIndex(level);
	//Enable spinner when any level is selected except connect and we are not conneted
	//setSpinnerActive(level!=0 && ON != ui->tryToggleListen->state());
}


bool HubClientWidget::eventFilter(QObject *object, QEvent *event)
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


void HubClientWidget::onConnectButtonStateChanged(const TryToggleState last, const TryToggleState current)
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

void HubClientWidget::onSteeringChanged(qreal throttle, qreal steeringAngle)
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



void HubClientWidget::on_checkBoxShowEyes_toggled(bool checked)
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

void HubClientWidget::on_checkBoxShowStats_toggled(bool checked)
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

void HubClientWidget::on_checkBoxShowLog_toggled(bool checked)
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

void HubClientWidget::on_checkBoxShowOnlineButton_toggled(bool checked)
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



void HubClientWidget::on_widgetPanic_toggled(bool panic)
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
