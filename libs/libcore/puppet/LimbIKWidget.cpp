#include "LimbIKWidget.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPointF>

class LimbIKWidget;

struct IKLimb{

		LimbIKWidget *m_parent;
		bool m_lift=false;

		QPointF m_origin;
		QPointF m_target;

		qreal coxaAngle =0.0;
		qreal femurAngle =0.0;
		qreal tibiaAngle =0.0;


		explicit IKLimb(LimbIKWidget *parent)
			:m_parent(parent)
		{

		}

		void toggleLift(bool l){
			m_lift=l;

		}
		void setTarget(qreal x,  qreal y){
			m_target.setX(x);
			m_target.setY(y);
		}
		void paint(QPainter &painter){

			IK();
			//QPointF origo(w2,h2);
			QPen base(QBrush(Qt::NoBrush),0.02);

			QPen white(base);
			white.setWidth(0.04);
			white.setColor(QColor(m_lift?"white":"gray"));

			QPen green(base);
			green.setWidth(0.04);
			green.setColor(QColor("green"));

			QPen purple(base);
			purple.setWidth(0.01);
			purple.setColor(QColor("purple").darker());

			QPen teal(base);
			teal.setWidth(0.01);
			teal.setColor(QColor("teal"));

			QPen blue(base);
			blue.setWidth(0.01);
			blue.setColor(QColor("blue"));


			painter.setPen(white);
			painter.drawLine(m_origin,m_target);


			const qreal r=0.02;

			painter.setPen(blue);
			painter.drawEllipse(m_target,r,r);


			painter.setPen(green);
			painter.drawEllipse(m_origin,r,r);


			const qreal rw=0.02;
			const qreal PI2=M_PI*3;
			painter.setPen(purple);
			painter.drawRect(QRectF(0,0,rw,-coxaAngle/PI2));
			painter.drawRect(QRectF(rw,0,rw,-femurAngle/PI2));
			painter.drawRect(QRectF(rw*2,0,rw,-tibiaAngle/PI2));

			emit m_parent->IKUpadted();
		}

		//Inspired by https://github.com/TXBDan/BETH_Raspi/blob/master/ik.cpp
		void IK(){
			const qreal STD_LENGTH=0.5;
			const qreal LENGTH_COXA=STD_LENGTH;
			const qreal LENGTH_FEMUR=STD_LENGTH;
			const qreal LENGTH_TIBIA=STD_LENGTH;
			qreal dz=m_lift?STD_LENGTH*0.7:0;
			qreal dx=m_target.x();
			qreal dy=m_target.y();
			qreal CoxaFootDist = sqrt( pow(dy, 2) + pow(dx, 2) );
			qreal IKSW = sqrt( pow(CoxaFootDist-LENGTH_COXA, 2) + pow(dz, 2) );
			qreal IKA1 = atan2( (CoxaFootDist - LENGTH_COXA) , dz );
			qreal IKA2 = acos( (pow(LENGTH_TIBIA, 2) - pow(LENGTH_FEMUR, 2) - pow(IKSW, 2) ) / (-2.0*IKSW*LENGTH_FEMUR) );
			tibiaAngle = acos( (pow(IKSW, 2) - pow(LENGTH_TIBIA, 2) - pow(LENGTH_FEMUR, 2)) / (-2.0*LENGTH_FEMUR*LENGTH_TIBIA) );
			coxaAngle=atan2( dy , dx) ;
			femurAngle= IKA1 + IKA2 ;

		}
};

LimbIKWidget::LimbIKWidget(QWidget *parent)
	: QWidget(parent)
	, limb(new IKLimb(this))
{
	setMouseTracking(true);
}

void LimbIKWidget::mousePressEvent ( QMouseEvent * event ){
	if(0!=limb){
		limb->toggleLift(true);
		update();
	}
	setFocus();
}


void LimbIKWidget::mouseReleaseEvent ( QMouseEvent * event ){
	if(0!=limb){
		limb->toggleLift(false);
		update();
	}
	setFocus();
}


void LimbIKWidget::mouseMoveEvent(QMouseEvent *event){
	if(0!=limb){
		QRectF r=rect();
		QPointF pos=event->pos();
		limb->setTarget((pos.x()-r.width()/2)/r.width(), (pos.y()-r.height()/2)/r.height());
		update();
	}
}



void  LimbIKWidget::keyPressEvent(QKeyEvent *event){
	/*
		const int key=event->key();
		switch(key){
			case(Qt::Key_P):{
					m_gait->togglePause();
					return;
				}
			case(Qt::Key_L):{
					m_gait->toggleSingleLimb();
					return;
				}

			case(Qt::Key_0):
			case(Qt::Key_1):
			case(Qt::Key_2):
			case(Qt::Key_3):
			case(Qt::Key_4):
			case(Qt::Key_5):
			case(Qt::Key_6):
			case(Qt::Key_7):
			case(Qt::Key_8):
			case(Qt::Key_9):{

					m_gait->toggleLimb(key-Qt::Key_0);
					return;
				}
		}
	*/
}




void LimbIKWidget::paintEvent(QPaintEvent *){
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing,true);
	const qreal w=painter.device()->width();
	const qreal h=painter.device()->height();

	QColor bg("black");
	painter.setBrush(bg);
	painter.setPen(Qt::NoPen);
	painter.drawRect(0,0,w,h);

	QPen base(QBrush(Qt::NoBrush),2);

	QPen teal(base);
	teal.setWidth(0.01);
	teal.setStyle(Qt::SolidLine);
	teal.setColor(QColor("teal"));


	painter.translate(w/2,h/2);
	qreal s=qMin(w,h);
	painter.scale(s,s);

	qreal step=0.08;
	painter.setPen(teal);
	for(qreal y=-h;y<h;y+=step){
		painter.drawLine(QPointF(-w,y),QPointF(w,y));
	}
	for(qreal x=-w;x<w;x+=step){
		painter.drawLine(QPointF(x,-h),QPointF(x,h));
	}
	if(0!=limb){
		limb->paint(painter);
	}


}


void LimbIKWidget::getLimbVars(qreal &cox, qreal &fem, qreal &tib ){
	cox=limb->coxaAngle;
	fem=limb->femurAngle;
	tib=limb->tibiaAngle;
}
