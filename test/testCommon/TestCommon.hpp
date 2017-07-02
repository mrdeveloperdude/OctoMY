#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include <QTest>


class TestCommon:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


// External helpers

QByteArray randomByteArray(int size);
QImage randomImage(int w=512, int h=512 );
QByteArray imageToByteArray(QImage &image );
QByteArray randomImageByteArray(int w=512, int h=512 );
QByteArray randomJPEGByteArray(int w=512, int h=512, int q=85 );



#endif // TESTCOMMON_HPP
