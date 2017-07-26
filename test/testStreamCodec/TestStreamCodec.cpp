#include "TestStreamCodec.hpp"

#include "../testCommon/TestCommon.hpp"

#include "video/codec/StreamCodec.hpp"
#include "video/codec/widgets/StreamCodecWidget.hpp"

#include "video/codec/ExpLUT.hpp"

#include <QPainter>
#include <QRect>

void TestStreamCodec::test()
{
	StreamCodec codec, *codec_p=&codec;
	StreamCodecWidget stream, *stream_p = &stream;
	stream.setWindowTitle("Stream Codec Test Utility");
	stream.show();
	const int w=1024,h=w;





	const auto fmt=QImage::Format_ARGB32;

	quint8 sc=5;
	QImage im(0xff*sc,0xff*sc,fmt);
	ExpLUT lut;
	{
		QPainter p(&im);
		p.setPen(Qt::NoPen);
		p.setBrush(Qt::red);
		for(quint8 i=0; i<0xff; ++i) {
			p.drawRect(QRect(i*sc,0,sc,lut[i]*sc));
		}
	}

	stream_p->setImage(im,0);

	connect(&stream, &StreamCodecWidget::trigger, this, [=]() {

		qDebug()<<"TRIGGER";
		QImage src=randomImage(w,h);

		QImage dst=codec_p->decode(codec_p->encode(src));
		stream_p->setImage(src,0);
		stream_p->setImage(dst,1);

	} );
	while(stream.isVisible()) {
		qApp->processEvents();
	}

}


QTEST_MAIN(TestStreamCodec)
