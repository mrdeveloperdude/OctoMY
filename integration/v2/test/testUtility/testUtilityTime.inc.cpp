#include "TestUtility.hpp"

#include "utility/time/HumanTime.hpp"

template <typename T>
void testTemplatedCurrentMsec(bool shouldSucceed=true)
{
	const int retries=3;
	for(int i=0; i<retries; ++i) {
		const qint64 expected=QDateTime::currentMSecsSinceEpoch();
		const auto actual_raw=utility::time::currentMsecsSinceEpoch<T>();
		const qint64 actual=static_cast<qint64>(actual_raw);
		if( (expected==actual) && shouldSucceed ){
			return;
		}
		//qDebug()<<"expected="<<expected<<", actual="<<actual;
	}
	if(shouldSucceed){
		QFAIL(QString("testTemplatedCurrentMsec failed with shouldSucceed=%1").arg(shouldSucceed).toStdString().c_str());
	}
}

void TestUtility::testTime()
{

	// void delay( int millisecondsToWait );
	qint64 accumulator=0;
	for(int expected=0; expected<100; ++expected) {
		const qint64 start=QDateTime::currentMSecsSinceEpoch();
		utility::time::delay(expected);
		const qint64 end=QDateTime::currentMSecsSinceEpoch(), actual=end-start;
		const qint64 error=qAbs(actual-expected);
		accumulator += error;
	}
	QVERIFY(accumulator < 3);

	testTemplatedCurrentMsec<char>(false);
	testTemplatedCurrentMsec<unsigned char>(false);
	testTemplatedCurrentMsec<short>(false);
	testTemplatedCurrentMsec<unsigned short>(false);
	testTemplatedCurrentMsec<int>(false);
	testTemplatedCurrentMsec<unsigned int>(false);
	testTemplatedCurrentMsec<long int>(false);
	testTemplatedCurrentMsec<unsigned long int>(false);
	testTemplatedCurrentMsec<long long int>(true);
	testTemplatedCurrentMsec<unsigned long long int>(true);

	testTemplatedCurrentMsec<quint8>(false);
	testTemplatedCurrentMsec<qint8>(false);
	testTemplatedCurrentMsec<quint16>(false);
	testTemplatedCurrentMsec<qint16>(false);
	testTemplatedCurrentMsec<quint32>(false);
	testTemplatedCurrentMsec<qint32>(false);
	testTemplatedCurrentMsec<quint64>(true);
	testTemplatedCurrentMsec<qint64>(true);
	//template<typename T> T currentMsecsSinceEpoch();

	// double fsec();

	// quint64 variantToMs(QVariant v);
	// QVariant msToVariant(quint64 ms);


	//template<typename T> T currentMsecsSinceEpoch()


}
