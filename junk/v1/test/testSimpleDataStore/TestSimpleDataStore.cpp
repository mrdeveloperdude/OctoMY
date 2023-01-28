#include "TestSimpleDataStore.hpp"

#include "node/DataStore.hpp"

#include "node/SimpleDataStore.hpp"

#include "utility/Utility.hpp"


class SimpleDataStoreTester: public SimpleDataStore
{
private:
	QVariantMap mMyMap;

public:
	explicit SimpleDataStoreTester(QString filename="");
	virtual ~SimpleDataStoreTester() Q_DECL_OVERRIDE;

	// SimpleDataStore interface
public:
	virtual bool fromMap(QVariantMap data) Q_DECL_OVERRIDE;
	virtual QVariantMap toMap() Q_DECL_OVERRIDE;
	virtual bool fromDefault() Q_DECL_OVERRIDE;

};

////////////////////////////////////////////////////////////////////////////////

SimpleDataStoreTester::SimpleDataStoreTester(QString filename)
	: SimpleDataStore(filename)
{
	OC_METHODGATE();
}

SimpleDataStoreTester::~SimpleDataStoreTester()
{
	OC_METHODGATE();
}

bool SimpleDataStoreTester::fromMap(QVariantMap data)
{
	OC_METHODGATE();
	mMyMap=data;
	return true;
}

QVariantMap SimpleDataStoreTester::toMap()
{
	OC_METHODGATE();
	QVariantMap data=mMyMap;
	return data;
}


bool SimpleDataStoreTester::fromDefault()
{
	OC_METHODGATE();
	mMyMap=QVariantMap();
	mMyMap["default"]="generated";
	return true;
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
		data["myInt"]=static_cast<quint32>(123);
		data["myFloat"]=static_cast<double>(123.4);
		data["myString"]="Abc";
		QVariantMap map;
		map["A"]="A";
		map["B"]="B";
		data["myMap"]=map;
	}

	qDebug()<<" -- Create data store on disk, saving the dummy data";
	{
		SimpleDataStoreTester sds(fn), *sdsp=&sds;
		sdsp->setInitialized(sdsp);
		sds.fromDefault();
		QCOMPARE(sds.filename(), fn);
		QCOMPARE(sds.fileExists(), false);
		sds.fromMap(data);
		/*
		sds.save([=](SimpleDataStoreTester &msds, bool ok) {
			qDebug()<<"Save callback called for store=" <<msds.filename()<<" with ok="<<ok;
			QCOMPARE(sdsp, &msds);
			QCOMPARE(sdsp->fileExists(), true);
		});
		*/
		sds.setInitialized<SimpleDataStoreTester>(nullptr);
		qDebug()<<"Waiting for dtor;";
	}
	qDebug()<<"Dtor should have run by now.";

	qDebug()<<" -- Load data store and verify that the data is loaded correctly";
	{
		SimpleDataStoreTester sds(fn), *sdsp=&sds;
		sdsp->setInitialized(sdsp);
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
		sds.setInitialized<SimpleDataStoreTester>(nullptr);
	}
	qDebug()<<" -- Done";
}

OC_TEST_MAIN(test, TestSimpleDataStore)
