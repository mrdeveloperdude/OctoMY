#include "TestQr.hpp"

#include "qr/native/Code.hpp"
#include "qr/native/Encoder.hpp"
#include "qr/nayuki/QrCode.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


void TestQr::testBytesToBits(){
	QByteArray input("dfgsdfgdfgsdfg");
	QString binaryString = "0110010001100110011001110111001101100100011001100110011101100100011001100110011101110011011001000110011001100111";
	QBitArray expected(binaryString.length());
	for (int i = 0; i < binaryString.length(); ++i) {
		expected.setBit(i, binaryString[i] == '1');  // Set true for '1', false for '0'
	}
	const auto output = bytesToBits(input);
	qDebug()<<"      in:"<<input;
	qDebug()<<"     out:"<<output;
	qDebug()<<"expected:"<<expected;
	QCOMPARE(output, expected);
}

void TestQr::test()
{
	Q_INIT_RESOURCE(icons);
	QString testInput("TestInputData!!!");
	
	
	auto native_encoder = OC_NEW Encoder();
	QCOMPARE(native_encoder->begin(), true);
	QCOMPARE(native_encoder->appendAutodetect(testInput), true);
	const auto native_code = native_encoder->end();
	if(!native_code){
		qDebug() << native_encoder->errorString();
	}
	QCOMPARE_NE(native_code, QSharedPointer<Code>(nullptr) );
	
	
	char *str=testInput.toUtf8().data();
	qrcodegen::QrCode nayuki_code = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::LOW);
	
	const auto s = native_code->width();
	QCOMPARE(s, nayuki_code.size);
	
	for(int y=0; y<s; y++) {
		for(int x=0; x<s; x++) {
			const auto nayuki_color = 1 == nayuki_code.getModule(x, y);  // 0 for white, 1 for black
			const auto native_color = native_code->module(x, y);
			qDebug() << ( nayuki_color == native_color? "[EQUAL]" : "[     ]")  << nayuki_color << native_color;
		}
	}
	//test::utility::waitForUIEnd(de);
}





OC_TEST_MAIN(test, TestQr)



