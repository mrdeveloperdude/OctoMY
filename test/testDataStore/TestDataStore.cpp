#include "TestDataStore.hpp"

#include "node/DataStore.hpp"

#include "utility/Utility.hpp"

#include <QVariantMap>

static quint64 sinSleep(qreal phase=0.0, quint64 min=1, quint64 max=50 )
{
	const qreal t=phase+((utility::currentMsecsSinceEpoch<quint64>()*M_PI*2.0)/4000.0);
	const int ms=min+((qSin(t)+1.0)*0.5)*(max-min);
	//qDebug()<<"sinSleep: "<<ms;
	//QThread::msleep(ms);
	return ms;
}

struct TestTask {
	int i;
	explicit TestTask(int i)
		:i(i)
	{
		qDebug()<<" + Item "<<i<<" created";
	}
	virtual ~TestTask()
	{
		qDebug()<<" - Item "<<i<<" deleted";
	}

	void execute()
	{
		const int ms=sinSleep(0.0); //(qSin(utility::currentMsecsSinceEpoch<quint64>()/10000.0)+1.0)*1000;
		qDebug()<<" + Item "<<i<<" start sleeping for "<<ms<<"ms";
		QThread::msleep(ms);
		qDebug()<<" - Item "<<i<<" done sleeping for "<<ms<<"ms";
	}
};

void TestDataStore::testConcurrentQueue()
{
	ConcurrentQueue<QSharedPointer<TestTask> > queue, *queuep = &queue;
	const int num=4;
	const int its=10;
	QFuture<void> futures[num*2];
	auto futuresp = &futures;
	qDebug()<<"Starting "<<num<<" consumers: ";
	for(int i=0; i<num; ++i) {
		//QThread::msleep(500);
		(*futuresp)[i]=QtConcurrent::run([=]() {
			for(int j=0; j<its; ++j) {
				qDebug()<<"Consuming item "<<i<<"-"<<j;
				QSharedPointer<TestTask> task = queuep->get();
				task->execute();
			}
		});
	}
	qDebug()<<"Starting "<<num<<" producers: ";
	for(int i=0; i<num; ++i) {
		(*futuresp)[i+num]=QtConcurrent::run([=]() {
			QThread::msleep(500);
			for(int j=0; j<its; ++j) {
				qDebug()<<"Producing item "<<i<<"-"<<j;
				int ct=0;
				queuep->put(QSharedPointer<TestTask>(new TestTask(ct++)));
				const int ms=sinSleep(M_PI); //(qSin(utility::currentMsecsSinceEpoch<quint64>()/10000.0+M_PI)+1.0)*1000;
				qDebug()<<"production break "<<ms<< "ms";
				QThread::msleep(ms);
			}
		});
	}
	//qDebug()<<"Waiting for "<<num<<" items to finish: ";
	for(int i=0; i<num*2; ++i) {
		futures[i].waitForFinished();
		qDebug()<<" + "<<num<<" finished...";
	}

	QVERIFY(queue.isEmpty());


	qDebug()<<"Done";
}

void TestDataStore::testDataStore()
{
	qDebug()<<" -- Make sure file does not already exist";
	const QString fn="test_datastore.json";
	QFile f(fn);
	if(f.exists()) {
		QCOMPARE(f.remove(), true);
	}


	qDebug()<<" -- Create some dummy data";
	QVariantMap data;
	{
		data["myInt"]=(quint32)123;
		data["myFloat"]=(double)123.4;
		data["myString"]="Abc";
		QVariantMap map;
		map["A"]="A";
		map["B"]="B";
		data["myMap"]=map;
	}
	qDebug()<<" -- Create temporary data store in memory";
	{
		DataStore ds;
	}

	qDebug()<<" -- Create data store on disk, saving the dummy data";
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), false);
		ds.set(data);
		QCOMPARE(ds.fileExists(), false);
		StorageEvent sts=ds.save();
		sts.waitForFinished();
		QCOMPARE(ds.fileExists(), true);
	}

	qDebug()<<" -- Load data store and verify that the data is loaded correctly";
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), true);
		StorageEvent lts=ds.load();
		lts.waitForFinished();
		StorageEvent gts=ds.get();
		gts.waitForFinished();
		QVariantMap g=gts.data();
		qDebug()<<"Orig:   "<<data;
		qDebug()<<"Loaded: "<<g;
		QVERIFY(utility::mapIsIn(g, data));
	}
	qDebug()<<" -- Done";

}

void TestDataStore::testDataStoreSyncEmpty()
{
	qDebug()<<" -- Make sure file does not already exist";
	const QString fn="test_datastore_sync_empty.json";
	QFile f(fn);
	if(f.exists()) {
		QCOMPARE(f.remove(), true);
	}


	qDebug()<<" -- Create some dummy data";
	QVariantMap data;
	{
		data["myInt"]=(quint32)123;
		data["myFloat"]=(double)123.4;
		data["myString"]="Abc";
		QVariantMap map;
		map["A"]="A";
		map["B"]="B";
		data["myMap"]=map;
	}
	qDebug()<<" -- Create temporary data store in memory";
	{
		DataStore ds;
	}

	qDebug()<<" -- Create empty datastore with filename and sync";
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), false);
		StorageEvent se=ds.synchronize();
		QCOMPARE(se.isFinished(), false);
		QCOMPARE(se.type(), STORAGE_EVENT_SYNCHRONIZE);
		se.waitForFinished();
		QCOMPARE(se.isFinished(), true);
		QVariantMap data=se.data();
		QCOMPARE(data.isEmpty(), true);
		QCOMPARE(ds.fileExists(), false);

	}
	qDebug()<<" -- Done";
}



void TestDataStore::testStressDataStore()
{
	const int max=100;
	for(int i=0; i<max; ++i) {
		qDebug()<<" ---------------- ITERATION "<<i<<" of "<< max;
		testDataStore();
	}
}
OC_TEST_MAIN(test, TestDataStore)

