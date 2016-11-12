#include "ZBarScanner.hpp"

#include "../libutil/utility/widgets/PixViewer.hpp"

#include <QVideoFrame>
#include <QPixmap>
#include <QImage>
#include <QDebug>
#include <QPen>
#include <QPainter>

// Depends on Qt for declaring NULL macro, so must come last
#include "zbar.h"


// Draw outlines with dotted red poly-lines
void ZScanResult::paint(QPainter &p)
{
	QPainterPath::Element firstPoint=outline.elementAt(0);
	const qreal dw=3;
	QVector<qreal> dashes;
	dashes << 2 << 4;
	QPen pen(QColor("#ffddcc"), dw, Qt::CustomDashLine, Qt::RoundCap, Qt::RoundJoin);
	pen.setDashPattern(dashes);
	p.save();
	p.setPen(pen);
	p.setBrush(Qt::NoBrush);
	p.setRenderHint(QPainter::Antialiasing);
	const int r=15;
	p.drawPath(outline);
	p.drawEllipse(firstPoint.x-r,firstPoint.y-r,r*2,r*2);
	pen.setColor(QColor("#445577"));
	pen.setDashOffset(3);
	pen.setDashPattern(dashes);
	p.setPen(pen);
	p.drawPath(outline);
	p.drawEllipse(firstPoint.x-r,firstPoint.y-r,r*2,r*2);
	p.restore();
}

ZBarScanner::ZBarScanner()
	: sc(nullptr)
	, pv(nullptr)
	, mScaleDown(false)
	, mConvertToGrayscale(true)
{
	//pv=new PixViewer();
	if(nullptr!=pv) {
		pv->setWindowTitle("zbar grayscale");
	}
}


QList<ZScanResult> ZBarScanner::scan(const QVideoFrame &frame)
{
	const int w=frame.width();
	const int h=frame.height();
	// Qt recommended way to process video frames without disrupting the flow of the video
	QVideoFrame cloneFrame(frame);
	cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
	const QImage image(cloneFrame.bits(), w,h, QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
	cloneFrame.unmap();
	return scan(image);
}



QList<ZScanResult> ZBarScanner::scan(const QPixmap &frame)
{
	return scan(frame.toImage());
}

extern "C" {
	extern int _zbar_verbosity;
}


QList<ZScanResult> ZBarScanner::scan(const QImage &image)
{
	QList<ZScanResult> out;
	if(nullptr==sc) {
		sc=new zbar::ImageScanner;
		if(nullptr!=sc) {
//TODO: Configure scanner here.
			//_zbar_verbosity=127;
		}
	}
	if(nullptr!=sc) {
		QImage imageToUse=image;
		if(mScaleDown) {
			imageToUse=imageToUse.scaled(QSize(640,480),Qt::KeepAspectRatio);
		}
		if(mConvertToGrayscale) {
			imageToUse=imageToUse.convertToFormat(QImage::Format_Grayscale8);
		}
		if(nullptr!=pv) {
			pv->setImage(imageToUse);
			pv->show();
		}
		zbar::QZBarImage im(imageToUse);
		int found=sc->scan(im);
		if(found>0) {
			zbar::SymbolSet ss=sc->get_results();
			for(zbar::SymbolIterator b=ss.symbol_begin(), e=ss.symbol_end(); b!=e; ++b) {
				zbar::Symbol sym=*b;
				ZScanResult res;
				res.data=QString::fromStdString(sym.get_data()).trimmed();
				res.type=QString::fromStdString(sym.get_type_name()).trimmed();
				int ls=sym.get_location_size();
				if(ls>0) {
					sym.get_type_name();
					res.outline.moveTo(sym.get_location_x(0), sym.get_location_y(0));
					for(int i=1; i<ls; ++i) {
						//qDebug()<<" + "<<sym.get_location_x(i)<<"x"<<sym.get_location_y(i);
						res.outline.lineTo(sym.get_location_x(i), sym.get_location_y(i));
					}
					res.outline.lineTo(sym.get_location_x(0), sym.get_location_y(0));
				}
				//qDebug()<<"Z-SCANNED STRING: "<<str;
				qDebug()<<"Z-SYMBOL "<<sym.get_data_length()<<" = "<<res.data<<", "<<res.type<<", "<<ls;
				out<<res;
			}
		} else {

		}
		//auto pf=image.pixelFormat();
//		qDebug()<<"Z-SCANNING FRAME "<<image.width()<<"x"<<image.height() /*<<"@"<<pf*/<< " found "<<found;
	} else {
		qWarning()<<"ERROR: Could not instanciate image scanner";
	}

	return out;
}
