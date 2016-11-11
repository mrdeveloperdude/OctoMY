#include "widgets/QRWidget.hpp"
#include "../libutil/utility/Standard.hpp"

#include "../libqr/QRPainter.hpp"

#include <QPainter>
#include <QDebug>


QRWidget::QRWidget(QWidget *parent)
	: QWidget(parent)
	, data("")
	, m_doubleBuffer(0)
	, fg("black")
	, bg("white")
	, dirty(true)
{
}


void QRWidget::setQRData(QString data)
{
	if(this->data!=data) {
		dirty=true;
		this->data=data;
		this->setToolTip(data);
		update();
	}
}


void QRWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	const double w=width();
	const double h=height();
	const double aspect=w/h;
	const double sz=((aspect>1.0)?h:w);
	const double ox=floor((w-sz)/2);
	const double oy=floor((h-sz)/2);
	//check for resize change
	if ( m_doubleBuffer == 0 ||  dirty || m_doubleBuffer->width() != ox || m_doubleBuffer->height() != oy ) {
		delete m_doubleBuffer;
		m_doubleBuffer = new QPixmap(sz,sz);
		QString error="";
		if(""==data) {
			m_doubleBuffer->fill(Qt::black);
		} else {
			m_doubleBuffer->fill(Qt::white);
			QPainter painter;
			painter.begin(m_doubleBuffer);
			paintQR(painter, size(), data);
			if(error.length()>0) {
				QColor error("red");
				painter.setBrush(error);
				painter.drawRect(0,0,sz,sz);
				qDebug()<<"QR FAIL: "<< error;
			}
			painter.end();
			dirty=false;
		}
	}
	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox,oy  ,sz,sz), *m_doubleBuffer, m_doubleBuffer->rect() );
	oPainter.end();
}
