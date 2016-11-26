#include "FaceWidget.hpp"
#include "ui_FaceWidget.h"

#include "basic/Settings.hpp"
#include "agent/Agent.hpp"

#include "widgets/Identicon.hpp"

#include <QScrollBar>


FaceWidget::FaceWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FaceWidget)
{
	ui->setupUi(this);
	ui->tryToggleConnect->setText("Go Online", "Connecting..", "Online");
	if(!connect(ui->tryToggleConnect,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SIGNAL(connectionStateChanged(const TryToggleState, const TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	updateEyeColor();

	//Make panic button RED
	QPalette p=ui->pushButtonPanic->palette();
	p.setColor(QPalette::Button,"#CC4422");
	ui->pushButtonPanic->setPalette(p);

}

FaceWidget::~FaceWidget()
{
	delete ui;
}


Agent *FaceWidget::agent()
{
	return ui->widgetRealtimeValues->getAgent();
}


void FaceWidget::updateEyeColor()
{
	Agent *ag=agent();
	if(nullptr!=ag) {
		QSharedPointer<NodeAssociate> ass=ag->nodeIdentity();
		if(nullptr!=ass) {
			const QString id=ass->id();
			if(id!=lastID) {
				lastID=id;
				PortableID pid=ag->nodeIdentity()->toPortableID();
				Identicon identicon;
				identicon.setPortableID(pid);
				ui->widgetEyes->setColor(identicon.bodyColorHigh());
			}
		}
	}
}

void FaceWidget::updateVisibility()
{
	Agent *ag=agent();
	if(nullptr!=ag) {
		Settings &s=ag->settings();
		ui->widgetEyes->setVisible(s.getCustomSettingBool("octomy.face"));
		ui->logScroll->setVisible(s.getCustomSettingBool("octomy.debug.log"));
		ui->widgetRealtimeValues->setVisible(s.getCustomSettingBool("octomy.debug.stats"));
		ui->widgetConnect->setVisible(s.getCustomSettingBool("octomy.online.show"));
	}
}


void FaceWidget::appendLog(const QString& text)
{
	ui->logScroll->appendLog(text);
}


void FaceWidget::setAgent(Agent *a)
{
	ui->widgetRealtimeValues->setAgent(a);
	updateEyeColor();
}


void FaceWidget::setConnectionState(const TryToggleState s, const bool doEmit)
{
	qDebug()<<"FACE TRYSTATE CHANGED TO: "<<s<<" WITH EMIT="<<doEmit;
	ui->tryToggleConnect->setState(s, doEmit);
}


TryToggleState FaceWidget::connectionState() const
{
	return ui->tryToggleConnect->state();
}


void FaceWidget::hookSignals(QObject &ob)
{
	if(!connect(this,SIGNAL(connectionStateChanged(TryToggleState, TryToggleState)),&ob,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(colorChanged(QColor)),&ob,SLOT(onColorChanged(QColor)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(panic()),&ob,SLOT(onPanic()),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
}


void FaceWidget::unHookSignals(QObject &ob)
{
	if(!disconnect(this,SIGNAL(connectionStateChanged(TryToggleState, TryToggleState)),&ob,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}

	if(!disconnect(this,SIGNAL(colorChanged(QColor)),&ob,SLOT(onColorChanged(QColor)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}

	if(!disconnect(this,SIGNAL(panic()),&ob,SLOT(onPanic()))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}
}



void FaceWidget::on_pushButtonNewColor_clicked()
{
	QPalette p=ui->pushButtonNewColor->palette();
	QColor col;
	col.setHslF(((qreal)qrand())/((qreal)RAND_MAX),1.0,0.5);
	p.setColor(QPalette::Button,col);
	ui->pushButtonNewColor->setPalette(p);
	emit colorChanged(col);
}

void FaceWidget::on_pushButtonPanic_clicked()
{
	QString str="P A N I C !";
	qWarning()<<str;
	appendLog(str);
	emit panic();
}
