#include "ClientWidget.hpp"
#include "ui_ClientWidget.h"

#include "../libutil/utility/Standard.hpp"

#include "basic/GenericKeyEventHandler.hpp"
#include "basic/Settings.hpp"

#include "widgets/WaitingSpinnerWidget.hpp"

#include <QScrollBar>


ClientWidget::ClientWidget(QWidget *parent, QSharedPointer<Node> node) :
	QWidget(parent)
	, ui(new Ui::ClientWidget)
	, mController(node)
	, mSpinner(nullptr)
{
	ui->setupUi(this);
	updateTimer.setInterval(100);
	if(!connect(&updateTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()),OC_CONTYPE)) {
		qDebug()<<"could not connect";
	}
	updateTimer.start();

	init();

}

ClientWidget::~ClientWidget()
{
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


void ClientWidget::init()
{
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

		ui->numericEntryServoTest->configure(&s,500,2500,200,1500," µs","Test remote servo control via network", "servo-position-1");
		if(!connect(ui->numericEntryServoTest,SIGNAL(valueChanged(int)),this,SLOT(onServoPositionChanged(int)))) {
			qWarning()<<"ERROR: could not connect";
		}
		ui->labelTouch->setText("WAITING FOR TOUCH");

		GenericKeyEventHandler *gkh=new GenericKeyEventHandler(ui->plainTextEditSpeechText);
		gkh->setEventProcessor([=](QObject *o, QKeyEvent *keyEvent) {
			auto t=keyEvent->type();
			if(t==QEvent::KeyPress || t==QEvent::KeyRelease) {
				keyEvent->accept();
				if (nullptr!=keyEvent && keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ControlModifier) {
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
	qDebug()<<"CONTROL LEVEL IS "<<level;
	ui->stackedWidgetControl->setCurrentIndex(level);
	if(nullptr!=mSpinner) {
		const bool startSpinner=  OFF != ui->widgetConnection->connectState() ;
		mSpinner->setStarted( startSpinner );
	}
}


bool ClientWidget::eventFilter(QObject *object, QEvent *event)
{
	//qDebug()<<"EVENT: "<<object<<": "<<event;
	if ((this==object) && (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress )) {
		QMouseEvent *r= static_cast<QMouseEvent *>(event);
		ui->labelTouch->setText("TOUCH: <"+QString::number(r->globalX())+", "+QString::number(r->globalY())+">");
		QPointF p=r->localPos();
		QSize s=size();
		//mRemote->onTouchUpdated(QVector2D(p.x()/(qreal)s.width(), p.y()/(qreal)s.height()));
	}
	//object == ui->widgetBackground && (
	return false;
}


///////////////////////////////////////// // Internal slots


void ClientWidget::onSummaryTimer()
{

}



void ClientWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendPlainText(text);
	QScrollBar *vsb=ui->logScroll->verticalScrollBar();
	if(nullptr!=vsb) {
		vsb->setValue(vsb->maximum());
	}
}






///////////////////////////////////////// // Internal UI slots

void ClientWidget::onTryToggleConnectionChanged(TryToggleState s)
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




void ClientWidget::on_pushButtonSay_clicked()
{
	QString text=ui->plainTextEditSpeechText->toPlainText();
	appendLog("SAID: "+text);
	ui->plainTextEditSpeechText->clear();
}
