#include "Graphics.hpp"

#include "uptime/MethodGate.hpp"

#include "utility/random/Random.hpp"

#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QGraphicsPixmapItem>

#include <QPainter>
#include <QBuffer>
#include <QImageWriter>

namespace utility
{

namespace graphics
{



QRgb randomColor( )
{
	return QColor::fromHslF(utility::random::dfrand(), utility::random::dfrand(), utility::random::dfrand(), utility::random::dfrand() ).rgba();
}


QImage randomImage(int w, int h)
{
	QImage image( w,h, QImage::Format_ARGB32);
	image.fill(Qt::green);
	{
		QPainter painter(&image);
		painter.setPen(Qt::NoPen);
		const int rr=(qMin(w,h)/16);
		const int area=(w*h)/rr;

		for(int i=0; i<area; ++i) {
			const QColor c(qrand()%255, qrand()%255, qrand()%255,127);
			const int r=qrand()%rr;
			const int x=qrand()%w;
			const int y=qrand()%h;
			painter.setBrush(c);
			painter.drawEllipse(x-r,y-r,r*2,r*2);
		}
	}
	return image;
}


QByteArray imageToByteArray(QImage &image )
{
	const uchar *bits=image.bits();
	QByteArray ba(reinterpret_cast<const char *>(bits), image.byteCount());
	return ba;
}


QByteArray randomImageByteArray(int w, int h)
{
	QImage image=randomImage(w,h);
	QByteArray ba=imageToByteArray(image);
	return ba;
}


QByteArray randomJPEGByteArray(int w, int h, int q)
{
	QImage image=randomImage(w,h);
	QBuffer bu;
	bu.open(QBuffer::WriteOnly);
	QImageWriter iw;
	iw.setFormat("jpeg");
	iw.setDevice(&bu);
	iw.setOptimizedWrite(true );
	iw.setQuality(q);
	iw.setCompression(9);
	iw.setProgressiveScanWrite(true );
	iw.write( image );
	bu.close();
	QByteArray ba=bu.buffer();
	//utility::byteArrayToFile("random_test.jpeg",ba );
	//qDebug()<<"Random JPEG image of size "<<w<<"x"<<h<<" was "<< utility::humanReadableSize(ba.size())<< "("<<(qreal)ba.size()/((qreal)w*h) << " bytes/pixel)";
	return ba;

}

}
}
