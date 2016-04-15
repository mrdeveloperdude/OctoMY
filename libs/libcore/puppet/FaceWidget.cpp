#include "FaceWidget.hpp"

#include <QDateTime>
#include <QPainter>
#include <QDebug>
#include <QVector2D>
#include <QMouseEvent>


Eye::Eye(QVector2D center, qreal slant)
	: center(center)
	, dirty(true)
	, slant(slant)
	, specPos1(-0.35,-0.12)
	, specPos2(-0.25,+0.15)

	, lidBrush("brown")
	, scaleraBrush("white")
	, irisBrush("#2d8ac9")
	, pupilBrush("#0d1e37")
	, specularBrush("#88ffffff")
{

}

/*
x = X cos(a) - Y sin(a)
y = Y cos(a) + X sin(a)
		*/
void generateElipsis(QVector2D &out,qreal rx,qreal ry,qreal ang){
	QVector2D p(cos(ang)*rx,sin(ang)*ry);
	out=p;
}

void Eye::update(){
	if(!dirty){
		return;
	}
	dirty=false;
	scaleraPolygon.clear();
	lowerMask.clear();
	upperMask.clear();
	qreal iblink=1.0-blink;
	qreal overlap=0.3;
	qreal r=0.1;
	qreal rx=(r+0.5*r*squintSteer.x());
	qreal ry=(r+0.5*r*squintSteer.y())*iblink;
	qreal rIrisX=rx*0.4;
	qreal rIrisY=ry*0.4;
	qreal bend=3.0;
	qreal lowerMaskRadius=0.8/(pow(bend,(fabs(lowerLidSteer.y())+1))-(bend-1.0));
	qreal upperMaskRadius=0.8/(pow(bend,(fabs(upperLidSteer.y())+1))-(bend-1.0));
	qreal lowerMaskOverlap=overlap-lowerLidSteer.y();
	QVector2D lowerMaskCenter(lowerLidSteer.x()*lowerMaskRadius,cos(lowerLidSteer.x())*lowerMaskRadius+ry-r*lowerMaskOverlap);
	qreal upperMaskOverlap=overlap+upperLidSteer.y();
	QVector2D upperMaskCenter(upperLidSteer.x()*upperMaskRadius,cos(upperLidSteer.x())*-upperMaskRadius-ry+r*upperMaskOverlap);
	for(qreal i=0;i<M_PI*2;i+=(M_PI/40.0)){
		QVector2D lm=lowerMaskCenter+QVector2D(cos(i)*lowerMaskRadius,sin(i)*lowerMaskRadius);
		QVector2D um=upperMaskCenter+QVector2D(cos(i)*upperMaskRadius,sin(i)*upperMaskRadius);

		{
			QVector2D po(cos(i)*rx,sin(i)*ry);
			QVector2D p(po.x() * cos(slant) - po.y() * sin(slant), po.y() * cos(slant) + po.x() * sin(slant) );

			//generateElipsis(p,rx,ry,i);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			scaleraPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
			lowerMask<< lm.toPointF();
			upperMask<< um.toPointF();
		}
		{
			QVector2D p(cos(i)*rIrisX,sin(i)*rIrisY);
			QVector2D lt=p-lowerMaskCenter;
			QVector2D ut=p-upperMaskCenter;
			pupilPolygon << (lt.length()<lowerMaskRadius?(lt.normalized()*lowerMaskRadius)+lowerMaskCenter:(ut.length()<upperMaskRadius?(ut.normalized()*upperMaskRadius)+upperMaskCenter:p)).toPointF();
		}
	}
}

void Eye::setBlink(qreal bl){
	if(bl!=blink){
		dirty=true;
	}
	blink=bl;
}

void Eye::setExpression(QVector2D ul, QVector2D ll, QVector2D sq){
	upperLidSteer=ul+center;
	lowerLidSteer=ll+center;
	squintSteer=sq;
	dirty=true;
}

void Eye::setSteer(QVector2D ey){
	ey+=center;
	if(ey!=eyeSteer){
		dirty=true;
	}
	eyeSteer=ey;
}


void Eye::paint(QPainter &painter){
	//painter.setBrush(lidBrush);
	//painter.drawPolygon(lowerMask);
	//painter.drawPolygon(upperMask);

	painter.setBrush(scaleraBrush);
	painter.drawPolygon(scaleraPolygon);

	painter.setBrush(irisBrush);
	//painter.drawPolygon(irisPolygon); //TODO: Use this instead
	painter.drawEllipse((eyeSteer-center).toPointF(),0.1*0.5,0.1*0.5);

	painter.setBrush(pupilBrush);
	painter.drawEllipse((eyeSteer-center).toPointF(),0.1*0.25,0.1*0.25);

	painter.setBrush(specularBrush);
	painter.drawEllipse((eyeSteer-center+specPos1*0.1).toPointF(),0.1*0.15,0.1*0.15);
	painter.drawEllipse((eyeSteer-center+specPos2*0.1).toPointF(),0.1*0.08,0.1*0.08);


}


////////////////////////////////////////////////////////////////////////////////



FaceWidget::FaceWidget(QWidget *parent)
	: QWidget(parent)
	, startTime(QDateTime::currentMSecsSinceEpoch())
	, lastTime(0)
	, cycle(0.0)
	, leftEye(QVector2D(-0.2,0),-0.1*M_PI)
	, rightEye(QVector2D(0.2,0),0.1*M_PI)
	, bgBrush("black")
{
	if(!connect(&timer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()))){
		qWarning()<<"ERROR: Could not connect";
	}

}


void FaceWidget::paintEvent(QPaintEvent *){
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	//const quint64 sinceStart=now-startTime;
	const quint64 sinceLastTime=now-lastTime;
	const qreal cycleTime=7.0;
	cycle+=((qreal)sinceLastTime)/1000.0;
	cycle=fmod(cycle,cycleTime);
	qreal lastBlink=blink;
	const qreal blinkTime=0.2;
	if(cycle<blinkTime){
		blink=cycle/blinkTime;
	}
	else{
		blink=0.0;
	}
	if(blink!=lastBlink){
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


void FaceWidget::onUpdateTimer(){
	update();
}



void FaceWidget::hideEvent(QHideEvent *){
	timer.stop();
	setMouseTracking(false);

}

void FaceWidget::showEvent(QShowEvent *){
	timer.start(1000/60);
	setMouseTracking(true);
}


void FaceWidget::mousePressEvent(QMouseEvent *e){
	QVector2D p=QVector2D(e->pos());
	QSize sz=size();
	QVector2D s=QVector2D(sz.width(),sz.height());
	lastPress=(p-(s/2))/s;
}

void FaceWidget::mouseMoveEvent(QMouseEvent *e){
	QVector2D p=QVector2D(e->pos());
	QSize sz=size();
	QVector2D s=QVector2D(sz.width(),sz.height());
	QVector2D m=(p-(s/2))/s;//+lastPress;
	Qt::MouseButtons but=e->buttons();
	if(but & Qt::MiddleButton){
		squintSteer=m;
	}
	else if(but & Qt::LeftButton){
		lowerLidSteer=m;
	}
	else if(but & Qt::RightButton){
		upperLidSteer=m;
	}
	eyeSteer=m*0.1;

	//	qDebug()<<"BUT: "<<but<<" squint "<< squint;
	if(lastPress.x()>0){
		leftEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	}
	else{
		rightEye.setExpression(lowerLidSteer,upperLidSteer,squintSteer);
	}
	leftEye.setSteer(eyeSteer);
	rightEye.setSteer(eyeSteer);
}


