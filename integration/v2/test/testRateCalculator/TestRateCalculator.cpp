#include "TestRateCalculator.hpp"

#include "utility/time/RateCalculator.hpp"


void TestRateCalculator::test()
{

	{
		RateCalculator rc;
		QCOMPARE(rc.mName, QString("Unnamed") );

		QCOMPARE(rc.mLast,			(quint64)0); // Last time data passed
		QCOMPARE(rc.mCount,			(quint64)0); // Number of packets passed
		QCOMPARE(rc.mBytes,			(quint64)0); // Number of bytes passed
		QCOMPARE(rc.mLastLog,		(quint64)0); // Last time we logged rate
		QCOMPARE(rc.mCountLog,		(quint64)0); // Number of packets passed since last log
		QCOMPARE(rc.mBytesLog,		(quint64)0); // Number of bytes passed since last log
		QCOMPARE(rc.mLogInterval,	(quint64)0); // Interval for logging. 0 means no logging
	}

	const QString name("TestRateCalc");
	const quint64 interval=1000;
	{
		RateCalculator rc(name, interval);
		QCOMPARE(rc.mName, name);

		QCOMPARE(rc.mLast,			(quint64)0); // Last time data passed
		QCOMPARE(rc.mCount,			(quint64)0); // Number of packets passed
		QCOMPARE(rc.mBytes,			(quint64)0); // Number of bytes passed
		QCOMPARE(rc.mLastLog,		(quint64)0); // Last time we logged rate
		QCOMPARE(rc.mCountLog,		(quint64)0); // Number of packets passed since last log
		QCOMPARE(rc.mBytesLog,		(quint64)0); // Number of bytes passed since last log
		QCOMPARE(rc.mLogInterval,	interval); // Interval for logging. 0 means no logging


		const quint64 bytes1=123;
		const quint64 bytes2=321;
		const quint64 bytes3=111;
		{
			const quint64 now=interval/2;

			rc.countPacket(bytes1, now);
			qDebug()<<"RC NOW: "<<rc;

			QCOMPARE(rc.mName,			name);
			QCOMPARE(rc.mLast,			now); // Last time data passed
			QCOMPARE(rc.mCount,			(quint64)1); // Number of packets passed
			QCOMPARE(rc.mBytes,			bytes1); // Number of bytes passed
			QCOMPARE(rc.mLastLog,		(quint64)0); // Last time we logged rate
			QCOMPARE(rc.mCountLog,		(quint64)1); // Number of packets passed since last log
			QCOMPARE(rc.mBytesLog,		bytes1); // Number of bytes passed since last log
			QCOMPARE(rc.mLogInterval,	interval); // Interval for logging. 0 means no logging
		}


		{
			const quint64 now=(interval*9)/10;

			rc.countPacket(bytes2, now);
			qDebug()<<"RC NOW: "<<rc;

			QCOMPARE(rc.mName,			 name);
			QCOMPARE(rc.mLast,			now); // Last time data passed
			QCOMPARE(rc.mCount,			(quint64)2); // Number of packets passed
			QCOMPARE(rc.mBytes,			bytes1 + bytes2); // Number of bytes passed
			QCOMPARE(rc.mLastLog,		(quint64)0); // Last time we logged rate
			QCOMPARE(rc.mCountLog,		(quint64)2); // Number of packets passed since last log
			QCOMPARE(rc.mBytesLog,		bytes1 + bytes2); // Number of bytes passed since last log
			QCOMPARE(rc.mLogInterval,	interval); // Interval for logging. 0 means no logging
		}


		{
			const quint64 now=(interval*3)/2;

			rc.countPacket(bytes3, now);
			qDebug()<<"RC NOW: "<<rc;

			QCOMPARE(rc.mName,			name);
			QCOMPARE(rc.mLast,			now); // Last time data passed
			QCOMPARE(rc.mCount,			(quint64)3); // Number of packets passed
			QCOMPARE(rc.mBytes,			bytes1 + bytes2 + bytes3); // Number of bytes passed
			QCOMPARE(rc.mLastLog,		now); // Last time we logged rate
			QCOMPARE(rc.mCountLog,		(quint64)0); // Number of packets passed since last log
			QCOMPARE(rc.mBytesLog,		(quint64)0); // Number of bytes passed since last log
			QCOMPARE(rc.mLogInterval,	interval); // Interval for logging. 0 means no logging
		}
	}
}


OC_TEST_MAIN(test, TestRateCalculator)
