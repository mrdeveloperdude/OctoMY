#include "TestConcurrentQueue.hpp"

#include "utility/ConcurrentQueue.hpp"

#include <QtConcurrent/QtConcurrent>


class TestItem
{
private:

	quint32 mIndex, mNoise, mCarry, mSeed, mAcc, mInvocations;


public:

	TestItem(quint32 index=0, quint32 seed=0)
		: mIndex(index)
		, mNoise(0)
		, mCarry(0)
		, mSeed(seed+1337)
		, mAcc(0)
		, mInvocations(0)
	{
		for(int i=0; i<100; ++i) {
			makeNoise();
		}
	}


private:

	// Deterministic PRNG
	quint32 makeNoise()
	{
		mNoise = mSeed;
		mNoise >>= 3;
		mNoise ^= mSeed;
		mCarry = mNoise & 1;
		mNoise >>= 1;
		mSeed >>= 1;
		mSeed |= (mCarry << 30);
		mInvocations++;
		return mNoise;
	}


public:


	quint32 index() const
	{
		return mIndex;
	}

	// Simulate a varying workload
	void work()
	{
		const quint32 num=1+(makeNoise()%10);
		for(quint32 i=0; i<num; ++i) {
			mAcc += ((makeNoise()>0x8888888)?1:0);
		}
		//qDebug()<<"Item "<<index<<" Worked for "<<num<<" iterations to produce "<<acc;
	}



	bool operator==(const TestItem &other) const
	{
		return (mIndex == other.mIndex) && (mNoise == other.mNoise)&& (mCarry == other.mCarry)&& (mSeed == other.mSeed)&& (mAcc == other.mAcc)&& (mInvocations == other.mInvocations);
	}

	bool operator!=(const TestItem &other) const
	{
		return !operator ==(other);
	}


	QString toString() const
	{
		return
			QString("TestItem{index=")+QString::number(mIndex)
			+", noise="+QString::number(mNoise)
			+", carry="+QString::number(mCarry)
			+", seed="+QString::number(mSeed)
			+", acc="+QString::number(mAcc)
			+", invocations="+QString::number(mInvocations)
			+"}";
	}
};

static QDebug &operator<< (QDebug &d, TestItem &ti )
{
	d.nospace().noquote()<<ti.toString();
	return d.maybeSpace();
}





////////////////////////////////////////////////////////////////////////////////


void TestConcurrentQueue::test()
{
	const int itemsPerQueue=2000, maxThreadCount=10, timesToRepeatTest=200000;
	quint32 start_seed=0;
	for(int j=0; j < timesToRepeatTest; ++j) {
		//qDebug()<<"DOING BATCH " << j << " of " << times;

		ConcurrentQueue<TestItem> itemQueue, itemQueFacit, *itemQueuePtr=&itemQueue, *itemQueFacitPtr=&itemQueFacit;

		for(quint32 i=0; i<itemsPerQueue; ++i) {
			TestItem ti(i, start_seed);
			start_seed++;
			itemQueue.put(ti);
			itemQueFacit.put(ti);
		}

		TestItem fasit[itemsPerQueue],  *pfasit=fasit;
		TestItem result[itemsPerQueue], *presult=result;

		// Calculate the facit
		for(int i = 0; i<itemsPerQueue; ++i) {
			TestItem tif = itemQueFacit.get();
			tif.work();
			fasit[tif.index()] = tif;
		}

		QThreadPool tp;
		tp.setMaxThreadCount(maxThreadCount);

		for(quint32 i = 0; i < itemsPerQueue; ++i) {
			QtConcurrent::run(&tp, [i, itemQueuePtr, itemQueFacitPtr, presult, pfasit] {
				Q_UNUSED(i);
				Q_UNUSED(itemQueFacitPtr);
				//qDebug()<<"RUNNABLE-"<<i<<" STARTED";
				TestItem ti = itemQueuePtr->get();
				ti.work();
				presult[ti.index()] = ti;
				if (pfasit[ti.index()] != presult[ti.index()])
				{
					qDebug()<< "FACIT: "<<pfasit[ti.index()]<< " vs. RESULT: "<<presult[ti.index()];
				}
				QCOMPARE(presult[ti.index()], pfasit[ti.index()]);
				//qDebug()<<"RUNNABLE-"<<i<<" DONE";
			});
		}

		//qDebug()<<"WAITING FOR BATCH "<<j<<" of "<<times<<" TO COMPLETE";
		tp.waitForDone();
		qDebug()<<"BATCH "<<j<<" of "<<timesToRepeatTest<<" DONE";
		for(int i = 0; i<itemsPerQueue; ++i) {
			if (fasit[i] != result[i]) {
				qDebug()<< "FACIT: "<<fasit[i]<< " vs. RESULT: "<<result[i];
			}
			QCOMPARE(fasit[i], result[i]);
		}

	}
}



OC_TEST_MAIN(test, TestConcurrentQueue)

