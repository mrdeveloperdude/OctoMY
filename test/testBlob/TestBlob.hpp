#ifndef TESTBLOB_HPP
#define TESTBLOB_HPP

#include <QTest>


class TestBlob:public QObject{
		Q_OBJECT

	private slots:
		void testBlob();
		void testBlobCourier();

};


#endif // TESTBLOB_HPP
