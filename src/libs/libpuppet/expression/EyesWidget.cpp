#include "EyesWidget.hpp"

#include "identity/Personality.hpp"
#include "identity/PersonalityColors.hpp"
#include "security/PortableID.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"

#include <QCursor>
#include <QDateTime>
#include <QDebug>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>
#include <QVector2D>

#include <QtMath>

static const QVector2D centerEyeSteer(0,0);


//TODO: Look at implementing full FACS https://imotions.com/blog/learning/research-fundamentals/facial-action-coding-system/

EyesWidget::EyesWidget(QWidget *parent)
	: QWidget(parent)
	, mStartTime(utility::time::currentMsecsSinceEpoch<quint64>())
	, mLeftEye(QVector2D(-0.2f, 0.0f), static_cast<float>(-0.1*M_PI) )
	, mRightEye(QVector2D(0.2f, 0.0f), static_cast<float>(0.1*M_PI) )
	, mBgBrush("black")
	, mEyeSteer(0, 0)
{
	OC_METHODGATE();
	mTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mTimer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	updateEyeShape();
	onUpdateTimer();
	updateIrisImage();
}

void EyesWidget::updateSteerFromGlobal()
{
	auto gpos = QCursor::pos();
	auto lpos = mapFromGlobal(gpos);
	auto sz = size();
	if (sz.width() <= 0 || sz.height() <= 0){
		return;
	}
	auto clampedX = std::clamp(lpos.x(), 0, sz.width());
	auto clampedY = std::clamp(lpos.y(), 0, sz.height());
	QVector2D center(sz.width()  / 2.0f, sz.height() / 2.0f);
	QVector2D p(clampedX, clampedY);
	QVector2D m = (p - center) / QVector2D(sz.width(), sz.height());
	mEyeSteer = m * 0.1f;
}

void EyesWidget::updateEyeShape(){
	OC_METHODGATE();
	mLeftEye.setExpression(mLowerLidSteer,mUpperLidSteer,mSquintSteer);
	mRightEye.setExpression(mLowerLidSteer,mUpperLidSteer,mSquintSteer);
	mLeftEye.setSteer(mEyeSteer);
	mRightEye.setSteer(mEyeSteer);
}
void EyesWidget::updateIrisImage()
{
	OC_METHODGATE();
	const auto sLeft = mLeftEye.irisRadius() * width() * 2.0f;
	const auto sLefti=static_cast<int>(std::floor(sLeft));
	QRect rectLeft(0,0, sLefti, sLefti);
	QSize sizeLeft=rectLeft.size();
	if(sizeLeft.width() > 0 && sizeLeft.height() > 0 ) {
		QImage img(sizeLeft, QImage::Format_ARGB32);
		QPainter painter(&img);
		mIrisRendrer.draw(rectLeft, painter, 0);
		mLeftEye.setIrisImage(img);
	}

	const auto sRight = mRightEye.irisRadius()*width()*2.0f;
	const auto sRighti = static_cast<int>(std::floor(sRight));
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
	const auto id = pid.id();
	const auto oldEyesVisible = mEyesVisible;
	mEyesVisible = !id.isEmpty();
	PersonalityColors colors(id);
	
	QColor irisColor=colors.backgroundColorLow();//QColor("#2d8ac9");
	mLeftEye.setColor(irisColor);
	mRightEye.setColor(irisColor);
	
	if((mIrisRendrer.portableID().id() != pid.id() ) || ( oldEyesVisible != mEyesVisible) ) {
		mIrisRendrer.setPortableID(pid);
		updateIrisImage();
	}
	if(mDebug){
		qDebug() << "Setting eye colors from PID: " << pid;
		qDebug() << "Color: " << irisColor;
		qDebug() << "STATUS" << (oldEyesVisible?"SHOWN":"HIDDEN") << " --> " << (mEyesVisible?"SHOWN":"HIDDEN");
		
	}
}


void EyesWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	mLeftEye.update();
	mRightEye.update();

	QPainter painter(this);
	const auto w=painter.device()->width();
	const auto h=painter.device()->height();
	painter.setRenderHint(QPainter::Antialiasing, true);

	if(mEyesVisible) {
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
	} else {
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
	}
//qDebug()<<"UPD";
}

void EyesWidget::updateAttention(){
	static const auto alpha=0.97f;
	static const auto beta=1.0f-alpha;
	auto gpos = QCursor::pos();
	auto lpos = mapFromGlobal(gpos);
	auto c = size()/2;
	QPointF center(c.width(), c.height());
	auto delta = lpos - center;
	auto dist = delta.manhattanLength()/qMax(c.width(), c.height())*2;
	auto attention = qMax(0.0, 4.0-dist);
	mAttention = mAttention * alpha + attention * beta;
	// qDebug()<< dist<< attention << mAttention;
}

void EyesWidget::onUpdateTimer()
{
	OC_METHODGATE();
	const auto alpha=0.8f;
	const auto beta=1.0f-alpha;
	if (!underMouse()) {
		updateSteerFromGlobal();
	}
	updateAttention();
	const auto lastEyeSteerSmooth = mEyeSteerSmooth;
	mEyeSteerSmooth=(mEyeSteerSmooth*alpha)+(mEyeSteer*beta);
	
	const auto alphaSteer = qMin(mAttention,1.0);
	const auto betaSteer=1.0-alphaSteer;
	auto alphaAttentionSteer = mEyeSteer*alphaSteer + centerEyeSteer * betaSteer;
	
	mEyeSteerSmooth=alphaAttentionSteer;

	if(lastEyeSteerSmooth!=mEyeSteerSmooth) {
		//qDebug()<<"SMOOTH STEER: "<<eyeSteer<<", "<<eyeSteerSmooth<<", "<<dif.length();
		mLeftEye.setSteer(mEyeSteerSmooth);
		mRightEye.setSteer(mEyeSteerSmooth);
	}
	const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
	//const quint64 sinceStart=now-startTime;
	const quint64 sinceLastTime=now-mLastTime;
	mLastTime=now;
	const auto cycleTime{7.0};
	mCycle+=(static_cast<qreal>(sinceLastTime))/1000.0f;
	mCycle=std::fmod(mCycle,cycleTime);
	const auto lastBlink=mBlink;
	const auto blinkTime{0.2f};
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
		mSquintSteer=m;
	} else if(but & Qt::LeftButton) {
		mLowerLidSteer=m;
	} else if(but & Qt::RightButton) {
		mUpperLidSteer=m;
	}
	mEyeSteer=m * 0.1f;


	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	if(lastPress.x()>0) {
		mLeftEye.setExpression(mLowerLidSteer,mUpperLidSteer,mSquintSteer);
	} else {
		mRightEye.setExpression(mLowerLidSteer,mUpperLidSteer,mSquintSteer);
	}
}



void EyesWidget::leaveEvent(QEvent *)
{
	OC_METHODGATE();
	//mEyeSteer=QVector2D(0,0);
	updateSteerFromGlobal();
}


void EyesWidget::resizeEvent(QResizeEvent *)
{
	OC_METHODGATE();
	// TODO: Limit frequency in a smart way
	updateIrisImage();
}
