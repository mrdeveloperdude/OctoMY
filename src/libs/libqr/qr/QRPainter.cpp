#include "QRPainter.hpp"

//#include "qr/nayuki/QrCode.hpp"
#include "qr/QrGeneratorFactory.hpp"

void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
{
	auto generator = getQrGenerator();
	generator->append(data);
	auto code = generator->generate();
	//char *str=data.toUtf8().data(); 	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	//const int s=qr.size>0?qr.size:1;
	const auto s = code->width();
	const double w=sz.width();
	const double h=sz.height();
	const double aspect=w/h;
	const double size=((aspect>1.0)?h:w);
	const double scale=size/(s+2);
	painter.setPen(Qt::NoPen);
	painter.setBrush(fg);
	for(int y=0; y<s; y++) {
		for(int x=0; x<s; x++) {
			//const int color = qr.getModule(x, y);  // 0 for white, 1 for black
			const int color = code->module(x, y) ? 1 : 0;  // 0 for white, 1 for black
			if(0x0!=color) {
				const double rx1=(x+1)*scale, ry1=(y+1)*scale;
				QRectF r(rx1, ry1, scale, scale);
				painter.drawRects(&r,1);
			}
		}
	}
}



QString asciiQR(const QString &data, QString on, QString off)
{
	//char *str=data.toUtf8().data();
	//qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	//const int s=qr.size>0?qr.size:1;
	auto generator = getQrGenerator();
	generator->append(data);
	auto code = generator->generate();
	const auto s = code->width();
	QString out="";
	for(int y=0; y<s; y++) {
		out = out + "\n";
		for(int x=0; x<s; x++) {
			//const int color = qr.getModule(x, y);  // 0 for white, 1 for black
			//out = out + (0x0==color?off:on);
			out += code->module(x, y) ? off:on;
		}
	}
	return out;
}


