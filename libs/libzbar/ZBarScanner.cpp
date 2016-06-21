#include "ZBarScanner.hpp"


#include <QDebug>

#include "zbar.h"


ZBarScanner::ZBarScanner()
	:
	  //	: zimage(0)
	  //, zscanner(0)
	  sc(0)
	, lw(-1)
	, lh(-1)
{

}

int c=0;


/* adapted from v4l2 spec */
#define fourcc(a, b, c, d)                      \
	((uint32_t)(a) | ((uint32_t)(b) << 8) |     \
	((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

int ZBarScanner::scan(const QVideoFrame &frame){
	int found=0;

	if(0==sc){
		sc=new zbar::ImageScanner;
	}
	/*
	QVideoFrame cloneFrame(frame);
	cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
	const int w=cloneFrame.width(), h=cloneFrame.height();
	const uchar *b=cloneFrame.bits();
	if(w!=lw || h!=lh){
		qDebug()<<"CHANGE DETECTED "<<lw<<"x"<<lh<<" -> "<<w<<"x"<<h;
		if(0!=zimage){
			zbar_image_destroy(zimage);
			zimage=0;
		}
		lw=w;
		lh=h;
	}
	if(0==zimage){
		zimage = zbar_image_create();
		zbar_image_set_format(zimage, *(unsigned long*)"Y800");
		zbar_image_set_size(zimage, w,h);
		//size_t bloblen = (w*h); unsigned char *blob = (unsigned char *)malloc(bloblen);
		//zbar_image_set_data(zimage, blob, bloblen, zbar_image_free_data);
	}
	if(0==zscanner){
		zscanner=zbar_image_scanner_create ();
	}
	if(0!=zimage){
		if(0!=zscanner){
			const QImage image(b, w,h, QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
			cloneFrame.unmap();
			const QImage imageGray=image.convertToFormat(QImage::Format_Grayscale8);
			size_t bloblen = (w*h);
			zbar_image_set_data(zimage, imageGray.bits(), bloblen, 0);
			const int found=zbar_scan_image (zscanner, zimage);
			qDebug()<<"ZBAR FOUND "<<found;
		}
	}
	*/
	if(0!=sc){
		const int w=frame.width();
		const int h=frame.height();
		QVideoFrame cloneFrame(frame);
		cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
		const QImage image(cloneFrame.bits(), w,h, QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
		cloneFrame.unmap();
		const QImage imageSmall=image.scaled(QSize(400,400),Qt::KeepAspectRatio);
		const QImage imageConverted=imageSmall.convertToFormat(QImage::Format_Grayscale8);
		/*
		bool ok=imageConverted.save(QString("/tmp/last_%1.png").arg(c++));
		if(!ok){
			qWarning()<<"ERTROR: could not save image";
		}
		*/
		zbar::QZBarImage im(imageConverted);
		//zbar::Image im2=im.convert(fourcc('G','R','E','Y'));
		//zbar::Image im(w,h,"Y800",frame.bits(),w*h);
		//unsigned long f=im.get_format();
		found=sc->scan(im);
		if(found>0){
			zbar::SymbolSet ss=sc->get_results();
			for(zbar::SymbolIterator b=ss.symbol_begin(), e=ss.symbol_end(); b!=e;++b){
				auto sym=*b;
				QString str=QString::fromStdString(sym.get_data());
				qDebug()<<"SCANNED STRING: "<<str;
			}
		}
		qDebug()<<"ZSCANNING FRAME "<<w<<"x"<<h<<"@"<<frame.pixelFormat()<< " found "<<found;
	}

	return found;
}
