#include "CarSteeringWidget.hpp"
#include "ui_CarSteeringWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/New.hpp"

#include <QPainter>
#include <QTimer>
#include <QMouseEvent>

#include <QtMath>
#include <QDebug>

CarSteeringWidget::CarSteeringWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::CarSteeringWidget)
	, mLastMouseButton(false)
	, mWheelRadius(1.0)
	, mLastAngle(0.0)
	, mLastThrottle(0.0)
	, mAngle(0.0)
	, mThrottle(0.0)
{
	OC_METHODGATE();
	ui->setupUi(this);
	mUpdateTimer.setTimerType(Qt::PreciseTimer);
	mUpdateTimer.setInterval(40);
	mUpdateTimer.setSingleShot(false);
	if(!connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(update()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect ";
	}
	//mUpdateTimer.start();
}

CarSteeringWidget::~CarSteeringWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void CarSteeringWidget::updateState()
{
	OC_METHODGATE();
	mLastAngle=mAngle;
	mLastThrottle=mThrottle;
	if(mLastMouseButton) {
		const QVector2D diff=QVector2D(mLastMousePos-mWheelCenter);
		const qreal dist=static_cast<qreal>(diff.length())-mWheelRadius;
		mThrottle=qBound(-1.0, dist/ static_cast<qreal>(mSz.length()), 1.0);
		mAngle=(M_PI)-atan2(static_cast<qreal>(diff.x()), static_cast<qreal>(diff.y()));
	} else {
		mThrottle=0.0;
	}
	if(!qFuzzyCompare(mLastAngle, mAngle) || !qFuzzyCompare(mLastThrottle, mThrottle)) {
		emit steeringChanged(mThrottle, mAngle);
		update();
	}

}

void CarSteeringWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
//	qDebug()<<"CAR STEERING PAINT";
	QPainter painter;
	painter.begin( this );
	painter.setPen(Qt::SolidLine);
	QPen pen;
	const qreal lw=qMin( static_cast<qreal>(mSz.x()), static_cast<qreal>(mSz.y()))/20.0;
	pen.setWidth(static_cast<int>(lw));
	pen.setCapStyle(Qt::RoundCap);
	pen.setColor(Qt::black);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
	//Draw wheel itself
	const qreal diameter=mWheelRadius*2;
	QRectF r1(static_cast<qreal>(mWheelCenter.x())-mWheelRadius, static_cast<qreal>(mWheelCenter.y())-mWheelRadius, diameter, diameter);
	painter.drawEllipse(r1);
	//Draw throttle bg
	painter.drawLine(QPointF(lw,lw),QPointF(lw, static_cast<qreal>(mSz.y())-lw));
	pen.setColor(mThrottle>0.0?Qt::yellow:Qt::blue);
	pen.setWidth(static_cast<int>(lw/2.0));
	painter.setPen(pen);
	//Draw throttle fg
	const qreal range=(static_cast<qreal>(mSz.y())-lw*2);
	const qreal v=(mThrottle*range);
	painter.drawLine(QPointF(lw, static_cast<qreal>(mSz.y())/2.0-lw-v),QPointF(lw, static_cast<qreal>(mSz.y())/2.0-lw));
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::red);
	static const qreal ofs=M_PI/2.0;
	//Draw red dot on wheel
	QRectF r2( static_cast<qreal>(mWheelCenter.x())+cos(mAngle-ofs)*mWheelRadius-lw/4, static_cast<qreal>(mWheelCenter.y())+sin(mAngle-ofs)*mWheelRadius-lw/4,lw/2,lw/2);
	painter.drawEllipse(r2);
	painter.end();
}





void CarSteeringWidget::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	//mUpdateTimer.stop();
	setMouseTracking(false);
}

void CarSteeringWidget::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	//mUpdateTimer.start(1000/60);
	setMouseTracking(true);
	updateState();
}


void CarSteeringWidget::mousePressEvent(QMouseEvent *e)
{
	OC_METHODGATE();
	mLastMousePos=QVector2D(e->pos());
	mLastMouseButton=(e->buttons() & Qt::LeftButton);
	mLastMousePressPos=mLastMousePos;
	updateState();
}

void CarSteeringWidget::mouseReleaseEvent(QMouseEvent *e)
{
	OC_METHODGATE();
	mLastMousePos=QVector2D(e->pos());
	mLastMouseButton=(e->buttons() & Qt::LeftButton);
	mThrottle=0.0;
	updateState();
}

void CarSteeringWidget::mouseMoveEvent(QMouseEvent *e)
{
	mLastMousePos=QVector2D(e->pos());
	mLastMouseButton=(e->buttons() & Qt::LeftButton);
	updateState();
}



void CarSteeringWidget::leaveEvent(QEvent *)
{
	OC_METHODGATE();
}


void CarSteeringWidget::resizeEvent(QResizeEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	//qDebug()<<"RESIZE: "<<size()<<"vs ."<<event->size();
	mSz=QVector2D(size().width(), size().height());
	mWheelRadius=qMin( static_cast<qreal>(mSz.x()), static_cast<qreal>(mSz.y()) )/2.0;
	mWheelCenter=QVector2D(mSz.x()/2,mSz.y());
	updateState();
}
