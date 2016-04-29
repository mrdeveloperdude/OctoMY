#include "GaitWidget.hpp"

#include <QPainter>

#include <QMouseEvent>
GaitWidget::GaitWidget(QWidget *parent)
	: QWidget(parent)
	, m_gait(0)
{


}

void GaitWidget::paintEvent(QPaintEvent *){
	QPainter painter(this);
	QString error="";
	const double w=width();
	const double h=height();
	QColor bg("black");
	painter.setBrush(bg);
	painter.setPen(Qt::NoPen);
	painter.drawRect(0,0,w,h);
	if(0!=m_gait){
		m_gait->paint(painter);
	}
}


void GaitWidget::mouseReleaseEvent ( QMouseEvent * event ){
	if(0!=m_gait){
		QRectF r=rect();
		QPointF pos=event->pos();
		m_gait->setDirection(pos.x()-r.width()/2,pos.y()-r.height()/2);
	}
	setFocus();
}


void GaitWidget::mouseMoveEvent(QMouseEvent *event){
	if(0!=m_gait){
		QRectF r=rect();
		QPointF pos=event->pos();
		m_gait->setTarget(pos.x()-r.width()/2,pos.y()-r.height()/2);
	}
}

void GaitWidget::setGait(GaitController <qreal> &gait){
	if(0!=m_gait){
		m_gait->unHookSignals(*this);
	}
	m_gait=&gait;
	m_gait->hookSignals(*this);
}

void GaitWidget::onGaitUpdated(){
	update();
}

void  GaitWidget::keyPressEvent(QKeyEvent *event){
	if(0!=m_gait){
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
	}
}


