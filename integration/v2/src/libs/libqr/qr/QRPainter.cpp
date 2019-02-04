#include "QRPainter.hpp"

#include "QrCode.hpp"

void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
{
	char *str=data.toUtf8().data();
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	const int s=qr.size>0?qr.size:1;
	const double w=sz.width();
	const double h=sz.height();
	const double aspect=w/h;
	const double size=((aspect>1.0)?h:w);
	const double scale=size/(s+2);
	painter.setPen(Qt::NoPen);
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
}



QString asciiQR(const QString &data, QString on, QString off)
{
	char *str=data.toUtf8().data();
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	const int s=qr.size>0?qr.size:1;
	QString out="";
	for(int y=0; y<s; y++) {
		out = out + "\n";
		for(int x=0; x<s; x++) {
			const int color = qr.getModule(x, y);  // 0 for white, 1 for black
			out = out + (0x0==color?off:on);
		}
	}
	return out;
}


