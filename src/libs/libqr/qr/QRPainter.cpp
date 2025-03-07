#include "QRPainter.hpp"

//#include "qr/nayuki/QrCode.hpp"
#include "qr/QrGeneratorFactory.hpp"
#include <qstatictext.h>

void paintQR(QPainter &painter, const QSize sz, const QString &data, QColor fg)
{
	auto generator = getQrGenerator();
	generator->append(data);
	auto code = generator->generate();
	//char *str=data.toUtf8().data(); 	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	//const int s=qr.size>0?qr.size:1;
	if(!code.isNull()){
		const auto s = code->width();
		const auto w=sz.width();
		const auto h=sz.height();
		const auto aspect=w/h;
		const auto size=((aspect>1.0)?h:w);
		const auto scale=size/(s+2);
		painter.setPen(Qt::NoPen);
		painter.setBrush(fg);
		for(int y=0; y<s; y++) {
			for(int x=0; x<s; x++) {
				//const int color = qr.getModule(x, y);  // 0 for white, 1 for black
				const auto color = code->module(x, y) ? 1 : 0;  // 0 for white, 1 for black
				if(0x0!=color) {
					const auto rx1=(x+1)*scale, ry1=(y+1)*scale;
					QRectF r(rx1, ry1, scale, scale);
					painter.drawRects(&r,1);
				}
			}
		}
	}
	else{
		const auto w = sz.width();
		const auto h = sz.height();
		painter.setBrush(Qt::red);
		painter.drawRect(QRect(0, 0, w, h));
		
		QFont font = painter.font();
		font.setBold(true);
		painter.setFont(font);
		painter.setPen(Qt::white);
		
		QStaticText text("Invalid code");
		text.setTextOption(QTextOption(Qt::AlignCenter));
		
		// Calculate the center of the rectangle.
		QPoint center = QRect(0, 0, w, h).center();
		
		// Get the text size and compute top-left position for centering.
		QSizeF textSize = text.size();
		QPoint topLeft(center.x() - int(textSize.width() / 2.0),
					   center.y() - int(textSize.height() / 2.0));
		
		// Draw the static text at the computed position.
		painter.drawStaticText(topLeft, text);

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


