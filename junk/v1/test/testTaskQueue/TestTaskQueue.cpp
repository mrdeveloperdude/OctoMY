#include "TestTaskQueue.hpp"

#include "utility/TaskQueue.hpp"
#include "utility/Utility.hpp"


#include <QtConcurrent/QtConcurrent>

enum TQ_TEST_TYPE {
	TQTT_NOISE, TQTT_SLEEP, TQTT_CT
};


static quint32 testWorkload(TQ_TEST_TYPE type, quint32 seed, quint32 num)
{
	Q_UNUSED(type);
	switch (type) {
	case TQTT_NOISE: {
		quint32 mNoise=0, mCarry=0, mSeed=seed, mAcc=0;


		for(quint32 i=0; i<num; ++i) {
			mNoise = mSeed;
			mNoise >>= 3;
			mNoise ^= mSeed;
			mCarry = mNoise & 1;
			mNoise >>= 1;
			mSeed >>= 1;
			mSeed |= (mCarry << 30);
			mAcc += ((mNoise>0x8888888)?1:0);
		}
		return mAcc;

	}



	case TQTT_SLEEP: {
		QThread::msleep(qrand()%3);
		//qDebug()<<"Running task: "<<i<<" from :"<<utility::currentThreadID();

	}

	break;
	default:
		break;
	}

	return 0;
}

void TestTaskQueue::test()
{

	const int taskCount=10000, maxThreadCount=taskCount/10, timesToRepeatTest=100;

	for(TQ_TEST_TYPE testType=TQTT_NOISE; testType < TQTT_CT; testType=static_cast<TQ_TEST_TYPE>(testType + 1)) {
		QVector<int> doneStatuses(taskCount), *doneStatusesPtr=&doneStatuses;
		for(int j=0; j<timesToRepeatTest; ++j) {

			//qDebug()<<"DOING BATCH "<<j<<" of "<<times;

			for(int v:doneStatuses) {
				QVERIFY(v==j);
			}

			typedef std::function<void()> CallbackType;
			TaskQueue<CallbackType> tq, *ptq=&tq;

			for(int i=0; i<taskCount; ++i) {
				tq.put([i, doneStatusesPtr, testType] {
					testWorkload(testType, (qrand()%100000)+1,(qrand()%100000)+1);
					(*doneStatusesPtr)[i]++;
				});
			}

			QThreadPool tp;
			tp.setMaxThreadCount(maxThreadCount);

			for(int i=0; i<maxThreadCount; ++i) {
				QtConcurrent::run(&tp, [i, ptq] {
					Q_UNUSED(i);
					//qDebug()<<"RUNNABLE-"<<i<<" STARTED";
					ptq->runAll();
					//qDebug()<<"RUNNABLE-"<<i<<" DONE";
				});
			}

			//qDebug()<<"WAITING FOR BATCH "<<j<<" of "<<times<<" TO COMPLETE";
			tp.waitForDone();
			qDebug()<<"BATCH "<<j<<" of "<<timesToRepeatTest<<" DONE";
			for(int v:doneStatuses) {
				QVERIFY(v==(j+1));
			}

		}
	}
}


OC_TEST_MAIN(test, TestTaskQueue)

