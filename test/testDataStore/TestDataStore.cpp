#include "TestDataStore.hpp"

#include "node/DataStore.hpp"

void TestDataStore::test()
{
	// Make sure file does not already exist
	const QString fn="test_datastore.json";
	QFile f(fn);
	if(f.exists()){
		QCOMPARE(f.remove(), true);
	}


	// Create some dummy data
	QVariantMap data;
	{
		data["myInt"]=123;
		data["myFloat"]=123.4;
		data["myString"]="Abc";
		QVariantMap map;
		map["A"]="A";
		map["B"]="B";
		data["myMap"]=map;
	}

	// Create data store on disk, saving the dummy data
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), false);
		ds.set(data);
	}

	// Load data store and verify that the data is loaded correctly
	{
		DataStore ds(fn);
		QCOMPARE(ds.filename(), fn);
		QCOMPARE(ds.fileExists(), true);
		Transaction gts=ds.get();
		gts.waitForFinished();
		QVariantMap g=gts.data();
		QCOMPARE(g, data);
	}

}


OC_TEST_MAIN(test, TestDataStore)

