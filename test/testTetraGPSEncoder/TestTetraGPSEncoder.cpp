#include "TestTetraGPSEncoder.hpp"

#include "basic/TetraGPSEncoder.hpp"
// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestTetraGPSEncoder::test(){
	TetraGPSEncoder enc;
	QString str=enc.generate(60.384545, 5.456664, 5.0);
	qDebug()<<"ENC="<<str;
}



QTEST_MAIN(TestTetraGPSEncoder)
