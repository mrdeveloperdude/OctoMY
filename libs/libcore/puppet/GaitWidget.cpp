#include "GaitWidget.hpp"

#include <QPainter>

#include <QMouseEvent>
GaitWidget::GaitWidget(QWidget *parent)
	: QWidget(parent)

{
	if(!connect(&timer,SIGNAL(timeout()),this,SLOT(onUpdateTimer()))){
		qWarning()<<"ERROR: Could not connect";
	}

}

void GaitWidget::onUpdateTimer(){
	gait.update();
	update();
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
	gait.paint(painter);
}


void GaitWidget::mouseReleaseEvent ( QMouseEvent * event ){
	QRectF r=rect();
	QPointF pos=event->pos();
	gait.setDirection(pos.x()-r.width()/2,pos.y()-r.height()/2);
	setFocus();
}


void  GaitWidget::keyPressEvent(QKeyEvent *event){
	const int key=event->key();
	switch(key){
		case(Qt::Key_P):{
				gait.togglePause();
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

				gait.toggleLimb(key-Qt::Key_0);
				return;
			}
	}


}


void GaitWidget::hideEvent(QHideEvent *){
	timer.stop();
}

void GaitWidget::showEvent(QShowEvent *){
	timer.start(1000/60);
}
