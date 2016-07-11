#include "TestKeyStore.hpp"


#include "security/KeyStore.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestKeyStore::test(){
	const QString fn="test_keystore.json";
	QFile file(fn), *filep=&file;
	if(file.exists()){
		QVERIFY(file.remove());
	}
	QVERIFY(!file.exists());
	KeyStore keystore(fn), *keystorep=&keystore;
	QVERIFY(!file.exists());
	connect(keystorep, &KeyStore::keystoreReady, [=](){
		keystorep->bootstrap();
		QVERIFY(filep->exists());
		{
			QVERIFY(filep->exists());
			KeyStore keystore2(fn);
			QVERIFY(filep->exists());
			keystorep->bootstrap();
			QVERIFY(filep->exists());
		}
	});
	//QVERIFY(connect


}



QTEST_MAIN(TestKeyStore)
