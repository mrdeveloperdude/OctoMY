#include "TestStore.hpp"

#include "Store.hpp"

void TestStore::test()
{
	store::Context c;
	qDebug()<<"Context is now "<<c;
	QString data("Hello");
	store::Key key("apple.iphone.version");
	qDebug().nospace().noquote()<<"Put of '"<<data<<"' using key '"<<key<<"' started";
	c.put(key, [=](store::Status status){
		qDebug()<<"Put completed with '"<<status<<"'";
	}, data);
	qDebug()<<"Put done. context is now "<<c;
	qDebug()<<"Put using key '"<<key<<"' started";
	c.get(key, [=](store::Status status, QVariant data){
		qDebug().nospace().noquote()<<"Get completed with '"<<status<<"' and "<<data;
	});
	qDebug()<<"Get done. context is now "<<c;

	QVariantMap v1;
	v1["lol"]="sexy";
	QVariantMap v2;
	v2["bob"]=v1;

	qDebug()<<"VTOS TEST: "<<store::vtos(v2);
}


OC_TEST_MAIN(test, TestStore)

