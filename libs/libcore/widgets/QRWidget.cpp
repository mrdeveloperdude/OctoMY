#include "widgets/QRWidget.hpp"
#include "basic/Standard.hpp"

#include "../libqr/qrencode.h"


#include <QPainter>
#include <QDebug>


QRWidget::QRWidget(QWidget *parent) :
	QWidget(parent),data(""){
}


void QRWidget::setQRData(QString data){
	this->data=data;
	update();
}

void QRWidget::paintEvent(QPaintEvent *){
	WWMETHODGATE();
	QPainter painter(this);
	QString error="";
	const double w=width();
	const double h=height();
	const double aspect=w/h;
	const double size=((aspect>1.0)?h:w);
	const double ox=(w-size)/2;
	const double oy=(h-size)/2;
	if(""==data){

	}
	else{
		QRinput *qri=QRinput_new();
		if(0!=qri){
			const int ret=QRinput_append(qri, QR_MODE_8, data.length(), (const unsigned char *)data.toStdString().c_str());
			if(0==ret){
				QRcode *qr=QRcode_encodeInput(qri);
				if(0!=qr){
					QColor fg("black");
					QColor bg("white");
					const int s=qr->width>0?qr->width:1;
					const double scale=size/s;
					painter.setBrush(bg);
					painter.setPen(Qt::NoPen);
					painter.drawRect(ox,oy,size,size);
					painter.setBrush(fg);
					for(int y=0;y<s;y++){
						const int yy=y*s;
						for(int x=0;x<s;x++){
							const int xx=yy+x;
							const unsigned char b=qr->data[xx];
							if(b &0x01){
								const double rx1=x*scale+ox, ry1=y*scale+oy;
								QRectF r(rx1, ry1, scale, scale);
								painter.drawRects(&r,1);
							}
						}
					}
					QRcode_free(qr);
					qr=0;
				}
				else{
					error="could not encode QR input: "+QString(strerror(errno));
				}
			}
			else{
				error="could not append QR input: "+QString(strerror(errno));
			}
			QRinput_free(qri);
			qri=0;
		}
		else{
			error="could not create QR input object: "+QString(strerror(errno));
		}
		if(error.length()>0){
			QColor error("red");
			painter.setBrush(error);
			painter.drawRect(ox,oy,size,size);
			qDebug()<<"QR FAIL: "<< error;
		}
	}
}
