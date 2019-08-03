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


OC_TEST_MAIN(test, TestStore)

