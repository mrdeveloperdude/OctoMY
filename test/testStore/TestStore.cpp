#include "TestStore.hpp"

#include "Store.hpp"

void TestStore::testVariantToString()
{
	QVariantMap v1;
	v1["lol"]="sexy";
	QVariantMap v2;
	v2["bob"]=v1;
	QCOMPARE(store::vtos(v2), "{\n    \"bob\": {\n        \"lol\": \"sexy\"\n    }\n}\n");
}


void TestStore::test()
{
	store::Context c;
	qDebug()<<"Context is now "<<c;
	const QString orig("Hello");
	store::Key key("apple.iphone.version");
	qDebug().nospace().noquote()<<"Put of '"<<orig<<"' using key '"<<key<<"' started";
	c.put(key, [=](store::Status status){
		qDebug()<<"Put completed with '"<<status<<"'";
	}, orig);
	qDebug()<<"Put done. context is now "<<c;
	qDebug()<<"Get using key '"<<key<<"' started";
	c.get(key, [=](store::Status status, QVariant data){
		qDebug().nospace().noquote()<<"Get completed with '"<<status<<"' and "<<data;
		QCOMPARE(data, orig);
	});
	qDebug()<<"Get done. context is now "<<c;
}


/*
if (variant.canConvert<QVariantList>()) {
	QSequentialIterable iterable = variant.value<QSequentialIterable>();
	// Can use foreach:
	foreach (const QVariant &v, iterable) {
		qDebug() << v;
	}
	// Can use C++11 range-for:
	for (const QVariant &v : iterable) {
		qDebug() << v;
	}
	// Can use iterators:
	QSequentialIterable::const_iterator it = iterable.begin();
	const QSequentialIterable::const_iterator end = iterable.end();
	for ( ; it != end; ++it) {
		qDebug() << *it;
	}
}




				QAssociativeIterable map = pointer->value<QAssociativeIterable>();
				QAssociativeIterable::const_iterator it=map.find(part.name);
				if(map.end()!=it){
					auto lol=*it;
					pointer=lol;
				}
				else{
					map[part.name]=QVariantMap();
					it=map.find(part.name);
					if(map.end()!=it){
						pointer=it.value();
					}
					else{
						return Status(QString("Failed to insert default for key %1").arg(part.name));
					}
				}

*/

/*
QVariantMap *map=qvariant_cast<QVariantMap *>(pointer);
auto it=map->find(part.name);
if(map->end()!=it){
	pointer=&it.value();
}
else{
	map[part.name]=QVariantMap();
	it=map.find(part.name);
	if(map.end()!=it){
		pointer=it.value();
	}
	else{
		return Status(QString("Failed to insert default for key %1").arg(part.name));
	}
}


 QVariant &v = map["some key"]; Q_ASSERT(!v.isNull()); Q_ASSERT(v.userType() == QMetaType::VariantMap); QVariantMap &innerMap = *reinterpret_cast<QVariantMap *>(v.data()); innerMap.mutate();


*/



OC_TEST_MAIN(test, TestStore)

