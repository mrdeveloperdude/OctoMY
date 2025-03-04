#include "TestAsyncStore.hpp"

#include "store/AsyncStore.hpp"
#include "BackendMock.hpp"
#include "FrontendMock.hpp"

#include "store/ASEventType.hpp"
#include "AsyncStoreTester.hpp"
#include "utility/concurrent/Concurrent.hpp"

#include <QList>
#include <QThread>
#include <QtConcurrentMap>

#include <functional>


// Type made to emulate non-trivial template parameter
struct EventDataType {
	int a;
	int b;
	QString c;

	EventDataType(int a=-1, int b=-1, QString c="not set")
		: a(a)
		, b(b)
		, c(c)
	{

	}

	QString toString() const
	{
		return QString("EventDataType(a=%1, b=%2, c=%3)").arg(a).arg(b).arg(c);
		//return "lol";
	}
	operator QString() const
	{
		return toString();
	}
	operator QString()
	{
		return toString();
	}

};


static ASEventType indexedASEventType(int index)
{
	return static_cast<ASEventType>(static_cast<int>(AS_EVENT_NONE) + (index%static_cast<int>(AS_EVENT_COMPLETE - AS_EVENT_NONE)));
}


static ASEventType randomASEventType()
{
	return indexedASEventType(utility::random::qrand());
}


void TestAsyncStore::testConcurrent()
{

	AsyncStoreTester<EventDataType> ast("testConcurrent.json", true, EventDataType(1,1,"1"), false, EventDataType(0,0,"0"), EventDataType(3,3,"3"));
	AsyncStoreTester<EventDataType> *astp=&ast;

	ast.configure();
	ast.activate(true);
	struct AsyncTestOperation {
		int index;
		ASEventType type;
		EventDataType data;
		AsyncTestOperation(int index=utility::random::qrand())
			: index(index)
			, type(indexedASEventType(index))
			, data(utility::random::qrand()%990+10, utility::random::qrand()%990+10, QString::number(utility::random::qrand()%990+10))
		{
		}

		QString toString() const
		{
			return QString("AsyncTestOperation(index=%1, type=%2, data=%3)").arg(index).arg(ASEventTypeToString(this->type)).arg(data);
		}
		operator QString() const
		{
			return toString();
		}
	};

	const int operationCount = 100;

	// Create a list containing test operations
	QList<AsyncTestOperation> operations;
	for (int i = 0; i < operationCount; ++i) {
		operations.append(AsyncTestOperation(i));
	}

	std::function<void(AsyncTestOperation&)> applyOperation = [=](AsyncTestOperation &op) {
		qDebug() << "Applying operation "<<op<<" @" << utility::concurrent::currentThreadID();
		//AsyncTestOperation
		switch(static_cast<quint8>(op.type)) {
		case(AS_EVENT_CLEAR): {
			astp->store.clear().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::clear().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_GENERATE): {
			astp->store.generate().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::generate().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_GET): {
			astp->store.get().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::get().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_LOAD): {
			astp->store.load().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::load().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_SAVE): {
			astp->store.save().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::save().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_SET): {
			astp->store.set(op.data).onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::set().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_STATUS): {
			astp->store.status().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::status().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_SYNCHRONIZE): {
			astp->store.synchronize().onFinished([](ASEvent<EventDataType> &d) {
				Q_UNUSED(d);
				qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() @"<<utility::concurrent::currentThreadID();
			});
		}
		break;
		case(AS_EVENT_EXISTS): {

			const bool ex=astp->store.fileExists();
			qDebug().nospace().noquote()<<"AsyncStoreTest::fileExists() was "<<ex<<" @"<<utility::concurrent::currentThreadID();
		}
		break;
		case(AS_EVENT_NONE): {
			// Do nothing
			//qDebug().nospace().noquote()<<"AsyncStoreTest:: NOOP @"<<utility::concurrent::currentThreadID();
		}
		break;
		case(AS_EVENT_COMPLETE):

		default: {
			qDebug()<<" MOVE ALONG NOTHING TO SEE HERE: "<<ASEventTypeToString(op.type);
		}
		break;


		}
	};

	QtConcurrent::blockingMap(operations, applyOperation);

	ast.activate(false);

	//ast.store.synchronize().waitForFinished();
	qDebug()<<"JOURNAL: "<<ast.store.journal();
	qDebug()<<"testConcurrent() done @"<<utility::concurrent::currentThreadID();
}


void TestAsyncStore::testEvents()
{
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	QSharedPointer<BackendMock<EventDataType> > backend= QSharedPointer<BackendMock<EventDataType> >(new BackendMock<EventDataType> () );
	QSharedPointer<FrontendMock<EventDataType> > frontend = QSharedPointer<FrontendMock<EventDataType> >(new FrontendMock<EventDataType> () );
	AsyncStore<EventDataType> store;
	store.configure(backend, frontend);
	store.activate(true);

	for (int i=0; i<100; ++i) {
		ASEventType type = randomASEventType();
		qDebug()<<"testEvents "<<i<<", type: "<<type;
		ASEvent<EventDataType> asev1(store, type);
		QVERIFY(!asev1.isNull());
		ASEvent<EventDataType> asev1n;
		QVERIFY(asev1n.isNull());
		QCOMPARE(asev1, asev1);
		ASEvent<EventDataType> asev2(asev1);
		QCOMPARE(asev1, asev2);
		ASEvent<EventDataType> asev3;
		asev3=asev1;
		QCOMPARE(asev1, asev3);
		QCOMPARE(asev3, asev2);

		ASEvent<EventDataType> *asev4=new ASEvent<EventDataType>();
		*asev4=asev1;
		QCOMPARE(asev1, *asev4);

		ASEvent<EventDataType> *asev5=new ASEvent<EventDataType>();
		*asev5=*asev4;
		QCOMPARE(asev1, *asev5);
		QCOMPARE(*asev4, *asev5);

		delete asev5;
		asev5=nullptr;

		QCOMPARE(asev1, *asev4);

		delete asev4;
		asev4=nullptr;

		ASEvent<EventDataType> asev7;
		{
			ASEvent<EventDataType> asev6;
			QCOMPARE(asev6, asev6);
			asev7 = asev6;
			QCOMPARE(asev6, asev7);
		}

	}
	store.activate(false);

}


void TestAsyncStore::testGenerationBySync()
{
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	typedef int TestType;
	{
		// Set up test with no data in backend (disk) and and no data in frontend (memory) to trigger generation upon synchronization
		TestType generateData=1337;
		AsyncStoreTester<TestType> ast("testGenerate.json", false, 1234, false, 0, generateData), *astp=&ast;
		ast.configure();
		ast.activate(true);
		QVERIFY(!astp->backend->exists);
		QVERIFY(astp->backend->diskData != generateData);
		QVERIFY(!astp->frontend->exists);
		QVERIFY(astp->frontend->memData != generateData);
		qDebug().nospace().noquote()<<"Sync "<<utility::concurrent::currentThreadID();
		ast.store.synchronize().onFinished([=](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::concurrent::currentThreadID();
			QCOMPARE(a.isSuccessfull(), true);
			QCOMPARE(a.isFinished(), true);
			QCOMPARE(a.isStarted(), true);
			QVERIFY(astp->backend->exists);
			QCOMPARE(astp->backend->diskData, generateData);
			QVERIFY(astp->frontend->exists);
			QCOMPARE(astp->frontend->memData, generateData);
		});
		ast.activate(false);
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend->diskData<< "(" << ast.backend->exists<< "), mem="<< ast.frontend->memData<< " @ "<<utility::concurrent::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::concurrent::currentThreadID();
}


void TestAsyncStore::testLoadingBySync()
{
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	typedef int TestType;
	{
		// Set up test with existing data in backend (disk) and and no data in frontend (memory) to trugger loading upon synchronization
		TestType diskData=1234;
		AsyncStoreTester<TestType> ast("testFileExists.json", true, diskData, false, 0, 1337), *astp=&ast;
		ast.configure();
		ast.activate(true);
		QVERIFY(astp->backend->exists);
		QVERIFY(astp->backend->diskData == diskData);
		QVERIFY(!astp->frontend->exists);
		QVERIFY(astp->frontend->memData != diskData);
		qDebug().nospace().noquote()<<"Sync "<<utility::concurrent::currentThreadID();
		ast.store.synchronize().onFinished([=](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::concurrent::currentThreadID();
			QCOMPARE(a.isSuccessfull(), true);
			QCOMPARE(a.isFinished(), true);
			QCOMPARE(a.isStarted(), true);
			QVERIFY(astp->backend->exists);
			QCOMPARE(astp->backend->diskData, diskData);
			QVERIFY(astp->frontend->exists);
			QCOMPARE(astp->frontend->memData, diskData);
		});
		ast.activate(false);
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend->diskData<< "(" << ast.backend->exists<< "), mem="<< ast.frontend->memData<< " @ "<<utility::concurrent::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::concurrent::currentThreadID();
}


void TestAsyncStore::testSavingBySync()
{
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	typedef int TestType;
	{
		// Set up test with existing data in backend (disk) and and no data in frontend (memory) to trigger saving upon synchronization
		TestType memData=1234;
		AsyncStoreTester<TestType> ast("testSaveChange.json", false, 0, true, memData, 1337), *astp=&ast;
		ast.configure();
		ast.activate(true);
		QVERIFY(!astp->backend->exists);
		QVERIFY(astp->backend->diskData != memData);
		QVERIFY(astp->frontend->exists);
		QVERIFY(astp->frontend->memData == memData);
		qDebug().nospace().noquote()<<"Sync "<<utility::concurrent::currentThreadID();
		ast.store.synchronize().onFinished([=](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::concurrent::currentThreadID();
			QCOMPARE(a.isSuccessfull(), true);
			QCOMPARE(a.isFinished(), true);
			QCOMPARE(a.isStarted(), true);
			QVERIFY(astp->backend->exists);
			QCOMPARE(astp->backend->diskData, memData);
			QVERIFY(astp->frontend->exists);
			QCOMPARE(astp->frontend->memData, memData);
			a.store().set(666).onFinished([](ASEvent<TestType> &a) {
				Q_UNUSED(a);
				qDebug().nospace().noquote()<<"AsyncStoreTest::set().onFinished() "<<utility::concurrent::currentThreadID();
			});

		});
		ast.activate(false);
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend->diskData<< "(" << ast.backend->exists<< "), mem="<< ast.frontend->memData<< " @ "<<utility::concurrent::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::concurrent::currentThreadID();
}


void TestAsyncStore::testFailures()
{
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	qDebug().nospace().noquote()<<"------------------------------------------------------";
	typedef int TestType;
	{
		// Set up test with no data in backend (disk) and and no data in frontend (memory) to trigger generation upon synchronization
		TestType generateData=1337;
		AsyncStoreTester<TestType> ast("testGenerate.json", false, 1234, false, 0, generateData, false, true), *astp=&ast;
		ast.configure();
		ast.activate(true);
		QVERIFY(!astp->backend->exists);
		QVERIFY(astp->backend->diskData != generateData);
		QVERIFY(!astp->frontend->exists);
		QVERIFY(astp->frontend->memData != generateData);
		qDebug().nospace().noquote()<<"Sync "<<utility::concurrent::currentThreadID();
		ast.store.synchronize().onFinished([=](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::concurrent::currentThreadID();
			QCOMPARE(a.isSuccessfull(), true);
			QCOMPARE(a.isFinished(), true);
			QCOMPARE(a.isStarted(), true);
			QVERIFY(!astp->backend->exists);
			QVERIFY(astp->backend->diskData != generateData);
			QVERIFY(astp->frontend->exists);
			QCOMPARE(astp->frontend->memData, generateData);
		});
		ast.activate(false);
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend->diskData<< "(" << ast.backend->exists<< "), mem="<< ast.frontend->memData<< " @ "<<utility::concurrent::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::concurrent::currentThreadID();
}




OC_TEST_MAIN(test, TestAsyncStore)

