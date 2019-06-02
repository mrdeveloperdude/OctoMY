#include "TestTetraGPSEncoder.hpp"

#include "gps/TetraGPSEncoder.hpp"


void TestTetraGPSEncoder::test()
{
	TetraGPSEncoder enc;
	QString str=enc.generate(60.384545, 5.456664, 5.0);
	qDebug()<<"ENC="<<str;
}


OC_TEST_MAIN(test, TestTetraGPSEncoder)
