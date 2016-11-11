#ifndef ZBARSCANNER_HPP
#define ZBARSCANNER_HPP

#include <QStringList>
#include <QPainterPath>

class QPixmap;
class QImage;
class QVideoFrame;
class PixViewer;
namespace zbar
{
class ImageScanner;
}


struct ZScanResult {
	QPainterPath outline;
	QString data;
	QString type;
};

class ZBarScanner
{
private:
	zbar::ImageScanner *sc;
	//Debug viwer
	PixViewer *pv;
	bool mScaleDown;
	bool mConvertToGrayscale;

public:
	ZBarScanner();

	QList<ZScanResult> scan(const QVideoFrame &frame);
	QList<ZScanResult> scan(const QPixmap &frame);
	QList<ZScanResult> scan(const QImage &image);
};

#endif // ZBARSCANNER_HPP
