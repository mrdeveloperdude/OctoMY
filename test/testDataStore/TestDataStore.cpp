#include "TestDataStore.hpp"

#include "node/DataStore.hpp"

#include "utility/Utility.hpp"

void TestDataStore::test()
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

	qDebug()<<" -- Create data store on disk, saving the dummy data";
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), false);
		ds.set(data);
		QCOMPARE(ds.fileExists(), false);
		Transaction sts=ds.save();
		sts.waitForFinished();
		QCOMPARE(ds.fileExists(), true);
	}

	qDebug()<<" -- Load data store and verify that the data is loaded correctly";
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), true);
		Transaction lts=ds.load();
		lts.waitForFinished();
		Transaction gts=ds.get();
		gts.waitForFinished();
		QVariantMap g=gts.data();
		qDebug()<<"Orig: "<<data;
		qDebug()<<"Loaded: "<<g;
		QVERIFY(utility::mapIsIn(g, data));
	}
	qDebug()<<" -- Done";

}


OC_TEST_MAIN(test, TestDataStore)

