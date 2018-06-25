#include "TestTaskQueue.hpp"

#include "utility/TaskQueue.hpp"
#include "utility/Utility.hpp"


#include <QtConcurrent/QtConcurrent>

void TestTaskQueue::test()
{

	const int num=10000, numt=num/10, times=10000;

	QVector<int> dones(num), *pdones=&dones;
	for(int j=0; j<times; ++j) {
		//qDebug()<<"DOING BATCH "<<j<<" of "<<times;

		for(int v:dones) {
			QVERIFY(v==j);
		}

		typedef std::function<void()> CallbackType;
		TaskQueue<CallbackType> tq, *ptq=&tq;

		for(int i=0; i<num; ++i) {
			tq.put([i, pdones] {
				QThread::msleep(qrand()%3);
				//qDebug()<<"Running task: "<<i<<" from :"<<utility::currentThreadID();
				(*pdones)[i]++;
			});
		}

		QThreadPool tp;
		tp.setMaxThreadCount(numt);

		for(int i=0; i<numt; ++i) {
			QtConcurrent::run(&tp, [i, ptq] {
				//qDebug()<<"RUNNABLE-"<<i<<" STARTED";
				ptq->runAll();
				//qDebug()<<"RUNNABLE-"<<i<<" DONE";
			});
		}

		//qDebug()<<"WAITING FOR BATCH "<<j<<" of "<<times<<" TO COMPLETE";
		tp.waitForDone();
		qDebug()<<"BATCH "<<j<<" of "<<times<<" DONE";
		for(int v:dones) {
			QVERIFY(v==(j+1));
		}

	}
}


OC_TEST_MAIN(test, TestTaskQueue)

