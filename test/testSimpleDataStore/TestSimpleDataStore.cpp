#include "TestSimpleDataStore.hpp"

#include "store/DataStore.hpp"
#include "store/SimpleDataStore.hpp"

#include "utility/data/Data.hpp"

class SimpleDataStoreTester: public SimpleDataStore
{
private:
	QVariantMap mMyMap;

public:
	explicit SimpleDataStoreTester();
	virtual ~SimpleDataStoreTester() Q_DECL_OVERRIDE;

	// SimpleDataStore interface
public:
	virtual bool fromMap(QVariantMap data) Q_DECL_OVERRIDE;
	virtual QVariantMap toMap() Q_DECL_OVERRIDE;
	virtual bool fromDefault() Q_DECL_OVERRIDE;

};


////////////////////////////////////////////////////////////////////////////////


SimpleDataStoreTester::SimpleDataStoreTester()
	: SimpleDataStore()
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
		QSharedPointer<SimpleDataStoreTester> sdsp(OC_NEW SimpleDataStoreTester);
		SimpleDataStoreTester &sds(*sdsp);
		Q_ASSERT(nullptr!=sdsp);
		sds.configure(fn);
		sdsp->activate(true);
		sds.fromDefault();
		QCOMPARE(sds.filename(), fn);
		QCOMPARE(sds.fileExists(), false);
		sds.fromMap(data);
		sds.save([=](QSharedPointer<SimpleDataStore> msds, bool ok) {
			qDebug()<<"Save callback called for store=" <<fn<<" with ok="<<ok;
			QCOMPARE(sdsp, msds);
			// This will happen AFTER event processing so we can't do it
			// QCOMPARE(sdsp->fileExists(), true);
		});
		sds.activate(false);
		qDebug()<<"Waiting for dtor;";
	}
	qDebug()<<"Dtor should have run by now.";

	qDebug()<<" -- Load data store and verify that the data is loaded correctly";
	{
		//SimpleDataStoreTester sds, *sdsp=&sds;
		QSharedPointer<SimpleDataStoreTester> sdsp(OC_NEW SimpleDataStoreTester);
		SimpleDataStoreTester &sds(*sdsp);
		Q_ASSERT(nullptr!=sdsp);
		sds.configure(fn);
		sdsp->activate(true);
		QCOMPARE(sds.filename(), fn);
		QCOMPARE(sds.fileExists(), true);
		auto preMap=sds.toMap();
		QVERIFY(!utility::data::mapIsIn(preMap, data));
		sds.load([=](QSharedPointer<SimpleDataStore> msds, bool ok) {
			qDebug()<<"Load callback called for store=" <<fn<<" with ok="<<ok;
			QCOMPARE(sdsp, msds);
			// This will happen AFTER event processing so we can't do it
			// QCOMPARE(sdsp->fileExists(), true);
			auto postMap=sdsp->toMap();
			QVERIFY(utility::data::mapIsIn(postMap, data));
		});
		sds.activate(false);
	}
	qDebug()<<" -- Done";
}


OC_TEST_MAIN(test, TestSimpleDataStore)
