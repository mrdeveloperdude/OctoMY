#include "TestSimpleDataStore.hpp"

#include "node/DataStore.hpp"

#include "utility/Utility.hpp"


class MySimpleDataStore: public SimpleDataStore
{
private:
	QVariantMap mMyMap;

public:
	explicit MySimpleDataStore(QString filename="");
	virtual ~MySimpleDataStore();

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data) Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
};


MySimpleDataStore::MySimpleDataStore(QString filename)
	: SimpleDataStore(filename)
{
	OC_METHODGATE();
}

MySimpleDataStore::~MySimpleDataStore()
{
	OC_METHODGATE();
}

bool MySimpleDataStore::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	mMyMap=data;
	return true;
}

QVariantMap MySimpleDataStore::toMap()
{
	OC_METHODGATE();
	return mMyMap;
}


////////////////////////////////////////////////////////////////////////////////


void TestSimpleDataStore::test()
{
	qDebug()<<" -- Make sure file does not already exist";
	const QString fn("test_simpledatastore.json");
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
		MySimpleDataStore sds(fn), *sdsp=&sds;
		QCOMPARE(sds.filename(), fn);
		QCOMPARE(sds.fileExists(), false);
		sds.fromMap(data);
		sds.save([=](SimpleDataStore &msds, bool ok) {
			qDebug()<<"Save callback called for store=" <<msds.filename()<<" with ok="<<ok;
			QCOMPARE(sdsp, &msds);
			QCOMPARE(sdsp->fileExists(), true);
		});
	}

	qDebug()<<" -- Load data store and verify that the data is loaded correctly";
	{
		MySimpleDataStore sds(fn), *sdsp=&sds;
		QCOMPARE(sds.filename(), fn);
		QCOMPARE(sds.fileExists(), true);
		auto preMap=sds.toMap();
		QVERIFY(!utility::mapIsIn(preMap, data));
		sds.load([=](SimpleDataStore &msds, bool ok) {
			qDebug()<<"Load callback called for store=" <<msds.filename()<<" with ok="<<ok;
			QCOMPARE(sdsp, &msds);
			QCOMPARE(sdsp->fileExists(), true);
			auto postMap=sdsp->toMap();
			QVERIFY(utility::mapIsIn(postMap, data));
		});
	}
	qDebug()<<" -- Done";
}

OC_TEST_MAIN(test, TestSimpleDataStore)
