#ifndef ZBARSCANNER_HPP
#define ZBARSCANNER_HPP

#include <QVideoFrame>

namespace zbar{
	class ImageScanner;
}

/*

namespace zbar{
	struct zbar_image_s;
	struct zbar_image_scanner_s;
}
*/
//struct zbar_image_s;
//struct zbar_image_scanner_s;

class ZBarScanner{
	private:
//		zbar_image_s *zimage;
	//	zbar_image_scanner_s *zscanner;
		zbar::ImageScanner *sc;
		int lw,lh;


	public:
		ZBarScanner();

		int scan(const QVideoFrame &frame);
};

#endif // ZBARSCANNER_HPP
