#include "TestSyncParameter.hpp"



#include "../libcore/comms/SyncParameter.hpp"



template <typename T>
class SyncTester
{
private:

	quint64 mNow;
	const quint64 mRTT;
	SyncContext mCTX;
	bool mUseFakeTime;

public:

	SyncTester(quint64 now, quint64 rtt)
		: mNow(now)
		, mRTT(rtt)
		, mCTX(mRTT, mNow)
		, mUseFakeTime(0!=mNow)
	{
		//TODO: IS this cheating?
		mNow=mCTX.now();
		qDebug()<<"STARTED SYNC TESTER WITH now="<<mNow<<" and rtt="<<mRTT<<" ("<< (mUseFakeTime?"FAKE":"CLOCK")<< ")";
	}

	//Pass some time
	void passTime(quint64 ms)
	{
		if(mUseFakeTime) {
			qDebug()<<"FAKE-WAITING "<<ms<<" ms";
			mNow+=ms;
			mCTX.updateNow(mNow);
		} else {
			qDebug()<<"CLOCK-WAITING "<<ms<<" ms";
			QTest::qWait(ms);
			mCTX.updateNow();
			//TODO: IS this cheating?
			mNow=mCTX.now();
		}

	}

	void doTest()
	{
		const quint64 tooLong=mRTT+100;
		const quint64 justRight=mRTT/2;
		qDebug()<<" --- ";
		qDebug()<<" --- Init "<<(mUseFakeTime?"FAKE":"CLOCK");

		QCOMPARE(mCTX.now(), mNow);
		QCOMPARE(mCTX.roundTripTime(), mRTT);

		const T throttleVal=(T)0.0;
		SyncParameter<T> throttleLocal(throttleVal, mCTX);
		SyncParameter<T> throttleRemote(throttleVal, mCTX);
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();

		QCOMPARE(throttleLocal.localTimestamp(), mNow);
		QCOMPARE(throttleLocal.remoteTimestamp(), (quint64)0);

		QCOMPARE(throttleRemote.localTimestamp(), mNow);
		QCOMPARE(throttleRemote.remoteTimestamp(), (quint64)0);

		QCOMPARE(throttleLocal.localValue() , throttleVal);
		QCOMPARE(throttleLocal.remoteValue() , throttleVal);

		QCOMPARE(throttleRemote.localValue() , throttleVal);
		QCOMPARE(throttleRemote.remoteValue() , throttleVal);

		QVERIFY(throttleLocal.isLocalConfirmed());
		QVERIFY(throttleRemote.isLocalConfirmed());

		QVERIFY(!throttleLocal.isRemoteConfirmed());
		QVERIFY(!throttleRemote.isRemoteConfirmed());

		QVERIFY(!throttleLocal.isInSync());
		QVERIFY(!throttleRemote.isInSync());


		qDebug()<<" --- First local change";
		const T throttleValNew=(T)1337.0;
		throttleRemote.setLocalValue(throttleValNew);
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();

		QCOMPARE(throttleLocal.localTimestamp(), mNow);
		QCOMPARE(throttleLocal.remoteTimestamp(), (quint64)0);

		QCOMPARE(throttleRemote.localTimestamp(), mNow);
		QCOMPARE(throttleRemote.remoteTimestamp(), (quint64)0);

		QCOMPARE(throttleLocal.localValue() , throttleVal);
		QCOMPARE(throttleLocal.remoteValue() , throttleVal);

		QCOMPARE(throttleRemote.localValue() , throttleValNew);
		QCOMPARE(throttleRemote.remoteValue() , throttleVal);

		QVERIFY(throttleLocal.isLocalConfirmed());
		QVERIFY(throttleRemote.isLocalConfirmed());

		QVERIFY(!throttleLocal.isRemoteConfirmed());
		QVERIFY(!throttleRemote.isRemoteConfirmed());

		QVERIFY(!throttleLocal.isInSync());
		QVERIFY(!throttleRemote.isInSync());

		qDebug()<<" --- First transfer";
		//Pass some time
		passTime(justRight);
		//This right here is an oversimplification of a successfull UDP tarnsfer from remote to local
		throttleLocal.setRemoteValue(throttleRemote.localValue());
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();


		QVERIFY(throttleLocal.remoteTimestamp() <= mNow + justRight);

		QVERIFY(throttleRemote.localTimestamp() <= mNow + justRight);
		QCOMPARE(throttleRemote.remoteTimestamp() , (quint64)0);

		QCOMPARE(throttleLocal.localValue() , throttleVal);
		QCOMPARE(throttleLocal.remoteValue() , throttleValNew);

		QCOMPARE(throttleRemote.localValue() , throttleValNew);
		QCOMPARE(throttleRemote.remoteValue() , throttleVal);


		QVERIFY(throttleLocal.isLocalConfirmed());
		QVERIFY(throttleRemote.isLocalConfirmed());

		QVERIFY(throttleLocal.isRemoteConfirmed());
		QVERIFY(!throttleRemote.isRemoteConfirmed());

		QVERIFY(throttleLocal.isInSync());
		QVERIFY(!throttleRemote.isInSync());


		qDebug()<<" --- First wait";
		passTime(tooLong);
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();


		QVERIFY(!throttleLocal.isLocalConfirmed());
		QVERIFY(!throttleRemote.isLocalConfirmed());

		QVERIFY(!throttleLocal.isRemoteConfirmed());
		QVERIFY(!throttleRemote.isRemoteConfirmed());

		QVERIFY(!throttleLocal.isInSync());
		QVERIFY(!throttleRemote.isInSync());


		qDebug()<<" --- Second transfer";
		throttleLocal.setRemoteNoChange();
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();

		qDebug()<<" --- Third transfer";
		passTime(tooLong);
		throttleRemote.setRemoteValue(throttleLocal.localValue());
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();

		qDebug()<<" --- Fourth transfer";
		throttleLocal.setRemoteNoChange();
		throttleRemote.setRemoteNoChange();
		qDebug()<<throttleLocal.toString();
		qDebug()<<throttleRemote.toString();

		//TODO: local timestamp should NOT expire like remote. find a better logic for it and align tests towards that
	}
};


void TestSyncParameter::test()
{

	SyncTester<qreal> syncTestRealClock(0,100);
	syncTestRealClock.doTest();
	SyncTester<qreal> syncTestRealFake(1000,100);
	syncTestRealFake.doTest();
	SyncTester<quint32> syncTestUInt32Clock(0,100);
	syncTestUInt32Clock.doTest();
	SyncTester<quint32> syncTestUInt32Fake(1000,100);
	syncTestUInt32Fake.doTest();
}


QTEST_MAIN(TestSyncParameter)
