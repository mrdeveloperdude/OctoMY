#include "TestAsyncStore.hpp"

#include "node/AsyncStore.hpp"
#include "BackendMock.hpp"
#include "FrontendMock.hpp"
#include "AsyncStoreTester.hpp"



void TestAsyncStore::testEvents()
{
	typedef int EventDataType;
	BackendMock<EventDataType> backend;
	FrontendMock<EventDataType> frontend;
	AsyncStore<EventDataType> store(backend, frontend);
	ASEventType type = AS_EVENT_GET;

	for (int i=0; i<1000; ++i) {
		qDebug()<<"LOL "<<i;
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

}



void TestAsyncStore::testFileExists()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", true, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		//QVERIFY(ast.);
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			Q_UNUSED(a);
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});

		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}

void TestAsyncStore::testGenerate()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", false, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			Q_UNUSED(a);
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}

void TestAsyncStore::testSaveChange()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", true, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			Q_UNUSED(a);
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});

		ast.store.set(666).onFinished([](ASEvent<TestType> &a) {
			Q_UNUSED(a);
			qDebug().nospace().noquote()<<"AsyncStoreTest::set().onFinished() "<<utility::currentThreadID();
		});

		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}



void TestAsyncStore::testFails()
{

}


OC_TEST_MAIN(test, TestAsyncStore)

