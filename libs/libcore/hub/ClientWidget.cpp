#include "ClientWidget.hpp"
#include "ui_ClientWidget.h"

#include "../libutil/utility/Standard.hpp"

#include "basic/GenericKeyEventHandler.hpp"
#include "basic/Settings.hpp"

#include "widgets/WaitingSpinnerWidget.hpp"
#include "audio/OneOffSpeech.hpp"

#include "security/PortableID.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/couriers/SensorsCourier.hpp"

#include <QScrollBar>


ClientWidget::ClientWidget(QSharedPointer<Node> controller, QSharedPointer<NodeAssociate> nodeAssoc, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ClientWidget)
	, mController(controller)
	, mNodeAssoc(nodeAssoc)
	, mSpinner(nullptr)
	, mSensorsCourier(new SensorsCourier(this))
{
	ui->setupUi(this);
	ui->tryToggleListen->setText("Connect","Connecting","Connected");
	ui->widgetBirthCertificate->configure(false,true);
	if(nullptr!=mNodeAssoc) {
		ui->widgetBirthCertificate->setPortableID(mNodeAssoc->toPortableID());
	} else {
		qWarning()<<"ERROR: no client";
	}
	updateTimer.setInterval(100);
	if(!connect(&updateTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	updateTimer.start();
	ui->widgetFace->setDisabled(true);

	if(!connect(ui->tryToggleListen,SIGNAL(stateChanged(TryToggleState, TryToggleState)),this,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}

	mSensorsCourier->setDestination(mNodeAssoc->toClientSignature());

	installEventFilter(this);
	init();
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


void ClientWidget::setSpinnerActive(bool active)
{
	OC_METHODGATE();
	if(nullptr!=mSpinner) {
		mSpinner->setStarted( active);
	}
}

CommsChannel *ClientWidget::comms()
{
	OC_METHODGATE();
	if(nullptr!=mController) {
		return mController->comms();
	}
	return nullptr;
}


void ClientWidget::courierRegistration(bool reg)
{
	CommsChannel *cc=comms();
	if(nullptr!=cc) {
		if(reg) {
			qDebug()<<"REGISTERING SENSORS COURIER FOR " <<mNodeAssoc->id();
			cc->registerCourier(*mSensorsCourier);
		} else {
			qDebug()<<"UN-REGISTERING SENSORS COURIER FOR " <<mNodeAssoc->id();
			cc->unregisterCourier(*mSensorsCourier);
		}
		// Adaptively start commschannel when there are couriers registered
		const int ct=cc->courierCount();
		if(ct>0) {
			if(nullptr != mNodeAssoc) {
				cc->start(mNodeAssoc->localAddress());
			}
		} else {
			cc->stop();
		}
	}
}

void ClientWidget::init()
{
	OC_METHODGATE();
	ui->stackedWidgetControl->setUpdatesEnabled(false);
	prepareSpinner();
	if(nullptr!=mController) {
		Settings &s=mController->settings();
		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		ui->labelTouch->setText("WAITING FOR TOUCH");
		ui->widgetActuatorControl->configure(10);
	} else {
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
		qWarning()<<"ERROR: No context provider set!";
	}
	ui->stackedWidgetControl->setUpdatesEnabled(true);
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


///////////////////////////////////////// // Internal slots


void ClientWidget::onSummaryTimer()
{

}



void ClientWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}




///////////////////////////////////////// // Internal UI slots

void ClientWidget::onConnectionStateChanged(const TryToggleState last, const TryToggleState current)
{
	OC_METHODGATE();
	//appendLog("TRYSTATE CHANGED TO "+ToggleStateToSTring(s));
	switch(current) {
	case(TRYING): {
		courierRegistration(true);
	}
	break;
	case(ON): {

	}
	break;
	default:
	case(OFF): {
		courierRegistration(false);
	}
	break;
	}
}



void ClientWidget::on_checkBoxShowEyes_toggled(bool checked)
{
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
	// TODO: Find better storage for these values than settings
	if(nullptr!=mController) {
		Settings&s=mController->settings();
		s.setCustomSettingBool("octomy.online.show",checked);
		ui->widgetFace->updateVisibility();
	} else {
		qWarning()<<"ERROR: No controller";
	}
}
