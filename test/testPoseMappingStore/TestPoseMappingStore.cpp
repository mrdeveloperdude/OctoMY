#include "TestPoseMappingStore.hpp"


#include "puppet/PoseMappingStore.hpp"
#include "puppet/PoseMapping.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestPoseMappingStore::test()
{

	const QString filename="TestPoseMappingStore.json";

	const quint32 SZ=2;
	const QString NAME1("Sigurd");
	const QString NAME2("Arne");
	const QString NAME3("Bjarne");
	const QString EMPTY;
	const quint32 ZERO=0;
	QSharedPointer<PoseMapping> pm(new PoseMapping(SZ));
	QCOMPARE(pm->size(),SZ);
	QCOMPARE(pm->name(0),EMPTY);
	pm->setName(0,NAME1);
	QCOMPARE(pm->name(0),NAME1);

	// Clear data from last test run
	QFile file(filename);
	if(file.exists()) {
		file.remove();
	}



	//Create an empty store and add data to it before saving
	{
		PoseMappingStore *store=new PoseMappingStore(filename);
		QVERIFY(nullptr!=store);
		store->bootstrap(false, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store->isReady());
		QVERIFY(!store->isInProgress());
		QVERIFY(!store->hasError());

		QSharedPointer<PoseMapping> mapping=store->poseMapping();
		QVERIFY(!mapping.isNull());
		QCOMPARE(mapping->size(),ZERO);


		mapping->resize(SZ);
		QCOMPARE(mapping->size(),SZ);
		QCOMPARE(mapping->name(0),EMPTY);

		mapping->setName(0,NAME2);
		QCOMPARE(mapping->name(0),NAME2);
		QCOMPARE(mapping->name(1),EMPTY);

		mapping->setName(1,NAME3);
		QCOMPARE(mapping->name(1),NAME3);

		// Implicitly saves data
		delete store;
		store=nullptr;
	}

	//Create a new store, this time expect the file to exist from last step (load only)
	{
		PoseMappingStore *store2=new PoseMappingStore(filename);
		QVERIFY(nullptr!=store2);
		store2->bootstrap(true, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store2->isReady());
		QVERIFY(!store2->isInProgress());
		QVERIFY(!store2->hasError());

		//Verify that data from last test is loaded successfully
		QSharedPointer<PoseMapping> mapping=store2->poseMapping();
		QVERIFY(!mapping.isNull());
		QCOMPARE(mapping->size(),SZ);
		QCOMPARE(mapping->name(0),NAME2);
		QCOMPARE(mapping->name(1),NAME3);

		// Implicitly saves data
		delete (PoseMappingStore *)store2;
		store2=nullptr;
	}

	if(file.exists()) {
		file.remove();
	}

}



QTEST_MAIN(TestPoseMappingStore)
