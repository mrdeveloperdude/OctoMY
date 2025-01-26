#include "TestHaar.hpp"

#include <QPainter>
#include <QPixmap>

#include "haar/CameraProcessor.hpp"
#include "haar/HaarCascade.hpp"
#include "haar/HaarFeatureExtractor.hpp"
#include "haar/HaarPreprocessor.hpp"
#include "haar/HaarRendering.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

//const auto cascade_fname = QString("/home/leo/Desktop/haarcascade_frontalface_default.xml");
const auto cascade_fname = QString("/home/leo/Desktop/haarcascade_frontalface_alt.xml");
const auto face_test_fname = QString("/home/leo/Desktop/face_test.png");

void TestHaar::testCameraProcessor()
{
	Q_INIT_RESOURCE(icons);
	qDebug()<<"HAAR";
	auto cp = OC_NEW CameraProcessor;
	cp->show();
	QLabel l;
	QImage im(3048, 2048, QImage::Format_RGBA8888);
	HaarLoaderXML loader;
	auto cascade = loader.load(cascade_fname);
	previewHaarCascade(im, *cascade);
	l.setPixmap(QPixmap::fromImage(im));
	l.show();
	test::utility::waitForUIEnd(cp);
}


void TestHaar::testHaarProcessor(){
	HaarLoaderXML loader;
	auto cascade = loader.load(cascade_fname);
	QImage im_in(face_test_fname);
	QImage im_out(im_in.size(), QImage::Format_RGBA8888);
	auto preprocessed = preprocessImage(im_in);
	auto mExtractor = new HaarFeatureExtractor();
	mExtractor->ingest(preprocessed);
	auto features = mExtractor->extract(cascade, 10, 6);
	{
		QPainter painter(&im_out);
		painter.drawImage(0,0, preprocessed);
		//painter.translate(200, 200);
		//drawHaarCascade(painter, *cascade.data());
		painter.setPen(Qt::green);
		//painter.fillRect(0, 0, 100, 100, Qt::green);
		painter.drawText(10,10, QString("Features: %1").arg(features.size()));
		painter.setPen(Qt::red);
		int index{1};
		for(auto feature:features){
			painter.drawLine(10, 10, feature.top(), feature.left());
			painter.drawRect(feature);
			painter.drawText(feature, QString::number(index++));
		}
	}
	QLabel l_in;
	QLabel l_out;
	l_in.setWindowTitle("INPUT IMAGE");
	l_out.setWindowTitle("OUTPUT IMAGE");
	l_in.setPixmap(QPixmap::fromImage(im_in));
	l_out.setPixmap(QPixmap::fromImage(im_out));
	l_in.show();
	l_out.show();
	test::utility::waitForUIEnd(&l_out);
}

OC_TEST_MAIN(test, TestHaar)

