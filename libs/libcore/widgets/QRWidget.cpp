#include "widgets/QRWidget.hpp"
#include "../libutil/utility/Standard.hpp"

#include "../libqr/QRPainter.hpp"

#include <QPainter>
#include <QDebug>


QRWidget::QRWidget(QWidget *parent)
	: QWidget(parent)
	, data("")
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
	const int threshold=10;
	const int w=width();
	const int h=height();
	const double aspect=(((double)w)/((double)h));
	const int sz=(aspect>1.0)?h:w;
	const int ox=floor((w-sz)/2);
	const int oy=floor((h-sz)/2);
	//check for resize change / dirty
	const int dbw=m_doubleBuffer.width();
	const int dbh=m_doubleBuffer.height();
	if (  dirty || (qAbs(dbw-sz)>threshold) || (qAbs(dbh-sz)>threshold) ) {
		dirty=false;
		m_doubleBuffer=QPixmap(sz,sz);
		if(""==data) {
			m_doubleBuffer.fill(Qt::black);
		} else {
			m_doubleBuffer.fill(Qt::white);
			QPainter painter(&m_doubleBuffer);
			paintQR(painter, size(), data);
			emit qrRedraw();
		}
	}

	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox,oy  ,sz,sz), m_doubleBuffer, m_doubleBuffer.rect() );
	oPainter.end();
}


const QPixmap &QRWidget::qrPixmap()
{
	return m_doubleBuffer;
}
