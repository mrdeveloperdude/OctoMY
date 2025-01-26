#ifndef STREAMCODEC_HPP
#define STREAMCODEC_HPP

#include <QByteArray>
#include <QImage>

int rgb2lum(QRgb rgb);

class StreamCodec
{
public:
	StreamCodec();

public slots:

	QByteArray encode(QImage src);
	QImage decode(QByteArray src);

};

#endif
// STREAMCODEC_HPP

