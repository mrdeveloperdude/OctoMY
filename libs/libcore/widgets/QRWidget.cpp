#include "widgets/QRWidget.hpp"
#include "../libutil/utility/Standard.hpp"

#include "../libqr/QrCode.hpp"

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
	const double size=((aspect>1.0)?h:w);
	const double ox=floor((w-size)/2);
	const double oy=floor((h-size)/2);
	//check for resize change
	if ( m_doubleBuffer == 0 ||  dirty || m_doubleBuffer->width() != ox || m_doubleBuffer->height() != oy ) {
		delete m_doubleBuffer;
		m_doubleBuffer = new QPixmap(size,size);
		QString error="";
		if(""==data) {
			m_doubleBuffer->fill(QColor("#00000000"));
		} else {
			m_doubleBuffer->fill(QColor("#FFFFFF"));
			QPainter painter;
			painter.begin(m_doubleBuffer);
			qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(data.toStdString().c_str(), qrcodegen::QrCode::Ecc::LOW);
			const int s=qr.size>0?qr.size:1;
			const double scale=size/(s+2);
			//painter.setBrush(bg);
			painter.setPen(Qt::NoPen);
			//painter.drawRect(ox,oy,size,size);
			painter.setBrush(fg);
			for(int y=0; y<s; y++) {
				for(int x=0; x<s; x++) {
					const int color = qr.getModule(x, y);  // 0 for white, 1 for black
					if(0x0!=color) {
						const double rx1=(x+1)*scale, ry1=(y+1)*scale;
						QRectF r(rx1, ry1, scale, scale);
						painter.drawRects(&r,1);
					}
				}
			}
			if(error.length()>0) {
				QColor error("red");
				painter.setBrush(error);
				painter.drawRect(0,0,size,size);
				qDebug()<<"QR FAIL: "<< error;
			}
			painter.end();
			dirty=false;
		}
	}
	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox,oy  ,size,size), *m_doubleBuffer, m_doubleBuffer->rect() );
	oPainter.end();
}
