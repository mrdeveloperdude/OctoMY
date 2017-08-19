#include "widgets/QRWidget.hpp"
#include "utility/Standard.hpp"

#include "qr/QRPainter.hpp"

#include <QPainter>
#include <QDebug>

#include <qmath.h>

QRWidget::QRWidget(QWidget *parent)
	: QWidget(parent)
	, mData("")
	, mFG("black")
	, mBG("white")
	, mDirty(true)
{
}


void QRWidget::setQRData(QString data)
{
	if(mData!=data) {
		mDirty=true;
		mData=data;
		setToolTip(data);
		update();
	}
}

void QRWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	// If data is not set we simply don't draw the QR code
	if(""==mData) {
		return;
	}
	const int threshold=10;
	const int w=width();
	const int h=height();
	const double aspect=(((double)w)/((double)h));
	const int sz=(aspect>1.0)?h:w;
	const int ox=floor((w-sz)/2);
	const int oy=floor((h-sz)/2);
	//check for resize change / dirty
	const int dbw=mDoubleBuffer.width();
	const int dbh=mDoubleBuffer.height();
	if (  mDirty || (qAbs(dbw-sz)>threshold) || (qAbs(dbh-sz)>threshold) ) {
		mDirty=false;
		mDoubleBuffer=QPixmap(sz,sz);
		if(""==mData) {
			mDoubleBuffer.fill(Qt::black);
		} else {
			mDoubleBuffer.fill(Qt::white);
			QPainter painter(&mDoubleBuffer);
			paintQR(painter, size(), mData);
			emit qrRedraw();
		}
	}

	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox,oy  ,sz,sz), mDoubleBuffer, mDoubleBuffer.rect() );
	oPainter.end();
}


const QPixmap &QRWidget::qrPixmap()
{
	return mDoubleBuffer;
}
