#include "EyesWidget.hpp"

#include "identity/Personality.hpp"
#include "identity/PersonalityColors.hpp"
#include "identity/Identicon.hpp"
#include "security/PortableID.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"

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
	, mStartTime(utility::time::currentMsecsSinceEpoch<quint64>())
	, mLastTime(0)
	, mBlink(0.0)
	, mCycle(0.0)
	, mLeftEye(QVector2D(-0.2f, 0.0f), static_cast<float>(-0.1*M_PI) )
	, mRightEye(QVector2D(0.2f, 0.0f), static_cast<float>(0.1*M_PI) )
	, mBgBrush("black")
	, mEyeSteer(0,0)
	, mHideEyes(true)
{
	OC_METHODGATE();
	mTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mTimer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	mLeftEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	mRightEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	mLeftEye.setSteer(mEyeSteer);
	mRightEye.setSteer(mEyeSteer);
	onUpdateTimer();
	updateIris();
}


void EyesWidget::updateIris()
{
	OC_METHODGATE();
	const float sLeft=mLeftEye.irisRadius() * width() * 2.0f;
	const int sLefti=static_cast<int>(std::floor(sLeft));
	QRect rectLeft(0,0, sLefti, sLefti);
	QSize sizeLeft=rectLeft.size();
	if(sizeLeft.width() > 0 && sizeLeft.height() > 0 ) {
		QImage img(sizeLeft, QImage::Format_ARGB32);
		QPainter painter(&img);
		mIrisRendrer.draw(rectLeft, painter, 0);
		mLeftEye.setIrisImage(img);
	}

	const float sRight=mRightEye.irisRadius()*width()*2.0f;
	const int sRighti=static_cast<int>(std::floor(sRight));
	QRect rectRight(0,0, sRighti, sRighti);
	QSize sizeRight=rectRight.size();
	if(sizeRight.width() > 0 && sizeRight.height() > 0 ) {
		QImage img(sizeRight,QImage::Format_ARGB32);
		QPainter painter(&img);
		mIrisRendrer.draw(rectRight, painter, 1);
		mRightEye.setIrisImage(img);
	}

	update();
}


void EyesWidget::setPortableID(PortableID &pid)
{
	OC_METHODGATE();
	// qDebug()<<"Setting eye colors from PID: "<<pid;
	QString id=pid.id();
	const bool oldHideEyes=mHideEyes;
	mHideEyes=!id.isEmpty();
	PersonalityColors colors(id);

	QColor irisColor=colors.backgroundColorLow();//QColor("#2d8ac9");
	mLeftEye.setColor(irisColor);
	mRightEye.setColor(irisColor);

	if((mIrisRendrer.portableID().id() !=pid.id() ) || ( oldHideEyes!=mHideEyes) ) {
		mIrisRendrer.setPortableID(pid);
		updateIris();
	}
}


void EyesWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	mLeftEye.update();
	mRightEye.update();

	QPainter painter(this);
	const float w=painter.device()->width();
	const float h=painter.device()->height();
	painter.setRenderHint(QPainter::Antialiasing, true);

	if(mHideEyes) {
		// Eyes denied!
		QPen pen(QColor(192, 64, 32, 128));
		const int b=5, b2=b*2, b4=b2*2, wi=static_cast<int>(std::floor(w)), hi=static_cast<int>(std::floor(h));
		pen.setStyle(Qt::DotLine);
		pen.setCapStyle(Qt::RoundCap);
		pen.setWidth(b);
		painter.setPen(pen);
		painter.drawRoundedRect(QRect(b2, b2, wi-b4, hi-b4), b4, b4);
		painter.drawLine(QPoint(b4, b4), QPoint(wi-b4, hi-b4));
		//painter.drawLine(QPoint(b4, h-b4), QPoint(w-b4, b4));
	} else {
		painter.setPen(Qt::NoPen);
		painter.translate(static_cast<qreal>(w/2), static_cast<qreal>(h/2));
		const qreal s=static_cast<qreal>(w);// Width is most important.
		//const float fh=h/w;// wee need to know face height
		painter.scale(s,s);
		//const float angle=(cycle*M_PI*2.0f)/cycleTime;
		//painter.drawRect(0,sin(angle)/2,1,0.1);


		painter.translate(0.2,0);
		mLeftEye.paint(painter);
		painter.translate(-0.4,0);
		mRightEye.paint(painter);
	}
//qDebug()<<"UPD";
}


void EyesWidget::onUpdateTimer()
{
	OC_METHODGATE();
	const float alpha=0.8f, beta=1.0f-alpha;
	const QVector2D lastEyeSteerSmooth=mEyeSteerSmooth;
	mEyeSteerSmooth=(mEyeSteerSmooth*static_cast<float>(alpha))+(mEyeSteer*static_cast<float>(beta));
	QVector2D dif=(mEyeSteerSmooth-mEyeSteer);
	if(dif.length()<0.001f) {
		mEyeSteerSmooth=mEyeSteer;
	}

	if(lastEyeSteerSmooth!=mEyeSteerSmooth) {
		//qDebug()<<"SMOOTH STEER: "<<eyeSteer<<", "<<eyeSteerSmooth<<", "<<dif.length();
		mLeftEye.setSteer(mEyeSteerSmooth);
		mRightEye.setSteer(mEyeSteerSmooth);
	}
	const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
	//const quint64 sinceStart=now-startTime;
	const quint64 sinceLastTime=now-mLastTime;
	mLastTime=now;
	const float cycleTime=7.0;
	mCycle+=(static_cast<float>(sinceLastTime))/1000.0f;
	mCycle=std::fmod(mCycle,cycleTime);
	float lastBlink=mBlink;
	const float blinkTime=0.2f;
	if(mCycle<blinkTime) {
		mBlink=mCycle/blinkTime;
	} else {
		mBlink=0.0;
	}
	if(qFuzzyCompare(mBlink, lastBlink)) {
		mLeftEye.setBlink(mBlink);
		mRightEye.setBlink(mBlink);
	}
	if(mLeftEye.isDirty() || mRightEye.isDirty()) {
		update();
	}
}



void EyesWidget::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	mTimer.stop();
	setMouseTracking(false);
}

void EyesWidget::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	mTimer.start(1000/60);
	setMouseTracking(true);
}


void EyesWidget::mousePressEvent(QMouseEvent *e)
{
	OC_METHODGATE();
	QVector2D p=QVector2D(e->pos());
	QSize sz=size();
	QVector2D s=QVector2D(sz.width(),sz.height());
	lastPress=(p-(s/2))/s;
}

void EyesWidget::mouseMoveEvent(QMouseEvent *e)
{
	OC_METHODGATE();
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
	mEyeSteer=m * 0.1f;


	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	if(lastPress.x()>0) {
		mLeftEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	} else {
		mRightEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	}
}



void EyesWidget::leaveEvent(QEvent *)
{
	OC_METHODGATE();
	mEyeSteer=QVector2D(0,0);
}


void EyesWidget::resizeEvent(QResizeEvent *)
{
	OC_METHODGATE();
	// TODO: Limit frequency in a smart way
	updateIris();
}
