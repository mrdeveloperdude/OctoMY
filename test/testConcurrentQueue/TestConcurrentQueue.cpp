#include "TestConcurrentQueue.hpp"

#include "utility/ConcurrentQueue.hpp"

#include <QtConcurrent/QtConcurrent>


struct TestItem {
	quint32 index, noise, carry, seed, acc, invocations;


	TestItem(quint32 index=0, quint32 seed=0)
		: index(index)
		, noise(0)
		, carry(0)
		, seed(seed+1337)
		, acc(0)
		, invocations(0)
	{
		for(int i=0; i<100; ++i) {
			makeNoise();
		}
	}

	// Deterministic PRNG
	quint32 makeNoise()
	{
		noise = seed;
		noise >>= 3;
		noise ^= seed;
		carry = noise & 1;
		noise >>= 1;
		seed >>= 1;
		seed |= (carry << 30);
		invocations++;
		return noise;
	}

	// Simulate a varying workload
	void work()
	{
		const quint32 num=1+(makeNoise()%10);
		for(quint32 i=0; i<num; ++i) {
			acc += ((makeNoise()>0x8888888)?1:0);
		}
		//qDebug()<<"Item "<<index<<" Worked for "<<num<<" iterations to produce "<<acc;
	}

	bool operator==(const TestItem &other) const
	{
		return (index == other.index) && (noise == other.noise)&& (carry == other.carry)&& (seed == other.seed)&& (acc == other.acc)&& (invocations == other.invocations);
	}

	bool operator!=(const TestItem &other) const
	{
		return !operator ==(other);
	}

	QString toString()
	{
		return
			QString("TestItem{index=")+QString::number(index)
			+", noise="+QString::number(noise)
			+", carry="+QString::number(carry)
			+", seed="+QString::number(seed)
			+", acc="+QString::number(acc)
			+", invocations="+QString::number(invocations)
			+"}";
	}
};

static QDebug &operator<< (QDebug &d, TestItem &ti )
{
	d.nospace().noquote()<<ti.toString();
	return d.maybeSpace();
}



void TestConcurrentQueue::test()
{
	const int num=2000, numt=10, times=200000;
	int start_seed=0;
	for(int j=0; j < times; ++j) {
		//qDebug()<<"DOING BATCH " << j << " of " << times;

		ConcurrentQueue<TestItem> cq, cqf, *pcq=&cq, *pcfq=&cqf;

		for(int i=0; i<num; ++i) {
			TestItem ti(i, start_seed);
			start_seed++;
			cq.put(ti);
			cqf.put(ti);
		}

		TestItem fasit[num],  *pfasit=fasit;
		TestItem result[num], *presult=result;

		// Calculate the facit
		for(int i=0; i<num; ++i) {
			TestItem tif = cqf.get();
			tif.work();
			fasit[tif.index] = tif;
		}

		QThreadPool tp;
		tp.setMaxThreadCount(numt);

		for(int i=0; i < num; ++i) {
			QtConcurrent::run(&tp, [i, pcq, pcfq, presult, pfasit] {
				//qDebug()<<"RUNNABLE-"<<i<<" STARTED";
				TestItem ti = pcq->get();
				ti.work();
				presult[ti.index] = ti;
				if (pfasit[ti.index] != presult[ti.index]) {
					qDebug()<< "FACIT: "<<pfasit[ti.index]<< " vs. RESULT: "<<presult[ti.index];
				}
				QCOMPARE(presult[ti.index], pfasit[ti.index]);
				//qDebug()<<"RUNNABLE-"<<i<<" DONE";
			});
		}

		//qDebug()<<"WAITING FOR BATCH "<<j<<" of "<<times<<" TO COMPLETE";
		tp.waitForDone();
		qDebug()<<"BATCH "<<j<<" of "<<times<<" DONE";
		for(int i=0; i<num; ++i) {
			if (fasit[i] != result[i]) {
				qDebug()<< "FACIT: "<<fasit[i]<< " vs. RESULT: "<<result[i];
			}
			QCOMPARE(fasit[i], result[i]);
		}

	}
}



OC_TEST_MAIN(test, TestConcurrentQueue)

