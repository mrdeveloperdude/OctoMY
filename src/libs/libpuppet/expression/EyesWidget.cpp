#include "EyesWidget.hpp"

#include "identity/Personality.hpp"
#include "identity/PersonalityColors.hpp"
#include "identity/Identicon.hpp"
#include "security/PortableID.hpp"



#include <QDateTime>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QImage>
#include <QDebug>
#include <QVector2D>
#include <QMouseEvent>
#include <QRectF>
#include <QSizeF>

#include <QtMath>


////////////////////////////////////////////////////////////////////////////////



EyesWidget::EyesWidget(QWidget *parent)
	: QWidget(parent)
	, startTime(QDateTime::currentMSecsSinceEpoch())
	, lastTime(0)
	, cycle(0.0)
	, leftEye(QVector2D(-0.2,0),-0.1*M_PI)
	, rightEye(QVector2D(0.2,0),0.1*M_PI)
	, bgBrush("black")
	, eyeSteer(0,0)
{
	if(!connect(&timer,SIGNAL(timeout()),this,SLOT(onUpdateTimer())), OC_CONTYPE) {
		qWarning()<<"ERROR: Could not connect";
	}
	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	leftEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	rightEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	leftEye.setSteer(eyeSteer);
	rightEye.setSteer(eyeSteer);
}


void EyesWidget::updateIris()
{
	const qreal sLeft=leftEye.irisRadius()*width()*2.0;
	QRect rectLeft(0,0, sLeft, sLeft);
	QSize sizeLeft=rectLeft.size();
	if(sizeLeft.width() > 0 && sizeLeft.height() > 0 ) {
		qDebug()<<"SIZE JUMP LEFT: "<<sizeLeft;
		QImage img(sizeLeft, QImage::Format_ARGB32);
		QPainter painter(&img);
		mIrisRendrer.draw(rectLeft, painter, 0);
		//painter.fillRect(rectLeft, Qt::blue);
		leftEye.setIrisImage(img);
	}

	const qreal sRight=rightEye.irisRadius()*width()*2.0;
	QRect rectRight(0,0, sRight, sRight);
	QSize sizeRight=rectRight.size();
	QImage imgRight(sizeRight,QImage::Format_ARGB32);
	{
		QPainter painter(&imgRight);
		mIrisRendrer.draw(rectRight, painter, 1);
	}
	rightEye.setIrisImage(imgRight);

	update();
}


void EyesWidget::setPortableID(PortableID &pid)
{
	PersonalityColors colors(pid.id());
	QColor irisColor=colors.backgroundColorLow();//QColor("#2d8ac9");
	leftEye.setColor(irisColor);
	rightEye.setColor(irisColor);

	if(mIrisRendrer.portableID().id() !=pid.id()) {
		mIrisRendrer.setPortableID(pid);
		updateIris();
	}
}


void EyesWidget::paintEvent(QPaintEvent *)
{
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	//const quint64 sinceStart=now-startTime;
	const quint64 sinceLastTime=now-lastTime;
	const qreal cycleTime=7.0;
	cycle+=((qreal)sinceLastTime)/1000.0;
	cycle=fmod(cycle,cycleTime);
	qreal lastBlink=blink;
	const qreal blinkTime=0.2;
	if(cycle<blinkTime) {
		blink=cycle/blinkTime;
	} else {
		blink=0.0;
	}
	if(blink!=lastBlink) {
		leftEye.setBlink(blink);
		rightEye.setBlink(blink);
	}

	leftEye.update();
	rightEye.update();

	QPainter painter(this);
	const qreal w=painter.device()->width();
	const qreal h=painter.device()->height();
	painter.setPen(Qt::NoPen);

	painter.setRenderHint(QPainter::Antialiasing,true);
	painter.translate(w/2,h/2);
	const qreal s=w;// Width is most important.
	//const qreal fh=h/w;// wee need to know face height
	painter.scale(s,s);
	//const qreal angle=(cycle*M_PI*2.0f)/cycleTime;
	lastTime=now;
	//painter.drawRect(0,sin(angle)/2,1,0.1);
	painter.translate(0.2,0);
	leftEye.paint(painter);
	painter.translate(-0.4,0);
	rightEye.paint(painter);
}


void EyesWidget::onUpdateTimer()
{
	const qreal alpha=0.8, beta=1.0f-alpha;
	const QVector2D lastEyeSteerSmooth=eyeSteerSmooth;
	eyeSteerSmooth=(eyeSteerSmooth*alpha)+(eyeSteer*beta);
	QVector2D dif=(eyeSteerSmooth-eyeSteer);
	if(dif.length()<0.001) {
		eyeSteerSmooth=eyeSteer;
	}

	if(lastEyeSteerSmooth!=eyeSteerSmooth) {
		//qDebug()<<"SMOOTH STEER: "<<eyeSteer<<", "<<eyeSteerSmooth<<", "<<dif.length();
		leftEye.setSteer(eyeSteerSmooth);
		rightEye.setSteer(eyeSteerSmooth);

		update();
	}
}



void EyesWidget::hideEvent(QHideEvent *)
{
	timer.stop();
	setMouseTracking(false);

}

void EyesWidget::showEvent(QShowEvent *)
{
	timer.start(1000/60);
	setMouseTracking(true);
}


void EyesWidget::mousePressEvent(QMouseEvent *e)
{
	QVector2D p=QVector2D(e->pos());
	QSize sz=size();
	QVector2D s=QVector2D(sz.width(),sz.height());
	lastPress=(p-(s/2))/s;
}

void EyesWidget::mouseMoveEvent(QMouseEvent *e)
{
	QVector2D p=QVector2D(e->pos());
	QSize sz=size();
	QVector2D s=QVector2D(sz.width(),sz.height());
	QVector2D m=(p-(s/2))/s;//+lastPress;
	Qt::MouseButtons but=e->buttons();
	if(but & Qt::MiddleButton) {
		squintSteer=m;
	} else if(but & Qt::LeftButton) {
		lowerLidSteer=m;
	} else if(but & Qt::RightButton) {
		upperLidSteer=m;
	}
	eyeSteer=m*0.1;


	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	if(lastPress.x()>0) {
		leftEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	} else {
		rightEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	}
}



void EyesWidget::leaveEvent(QEvent *)
{
	eyeSteer=QVector2D(0,0);
}


void EyesWidget::resizeEvent(QResizeEvent *)
{
	// TODO: Limit frequency in a smart way
	updateIris();
}
