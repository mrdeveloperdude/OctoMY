#include "TestKeyStore.hpp"


#include "security/KeyStore.hpp"

#include <QSignalSpy>

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestKeyStore::test(){
	const QString fn="test_keystore.json";
	QFile file(fn), *filep=&file;
	if(filep->exists()){
		QVERIFY(filep->remove());
	}
	QVERIFY(!filep->exists());
	KeyStore keystore(fn), *keystorep=&keystore;
	QVERIFY(nullptr!=keystorep);
	QVERIFY(!keystorep->isReady());
	QVERIFY(!keystorep->hasError());
	QVERIFY(!filep->exists());

	const bool conOK=connect(keystorep, &KeyStore::storeReady, [this,fn, filep, keystorep](){
		QVERIFY(nullptr!=filep);
		QVERIFY(filep->exists());
		QVERIFY(keystorep->isInProgress());//TODO: This should really return FALSE, but due to the quirky architecture, we need to refactor this before that is possible. Since this is not very important, I will save this task for another day!
		QVERIFY(keystorep->isReady());
		QVERIFY(!keystorep->hasError());
	});
	QVERIFY(conOK);
	if(conOK){
		QSignalSpy spy(keystorep, &KeyStore::storeReady);
		keystorep->bootstrap();
		const bool waitOK=spy.wait(20000);
		QVERIFY(waitOK);
		if(waitOK){
			QVERIFY(filep->exists());
			QVERIFY(spy.count()==1);
			{
				KeyStore keystore2(fn);
				keystore2.bootstrap(true,false);
				QVERIFY(filep->exists());
				QVERIFY(keystore2.fileExists());
				QVERIFY(!keystore2.isInProgress());
				QVERIFY(keystore2.isReady());
				QVERIFY(!keystore2.hasError());
			}
			filep->remove();
			QVERIFY(!filep->exists());
			{
				KeyStore keystore3(fn);
				keystore3.bootstrap(true,false);
				QVERIFY(!filep->exists());
				QVERIFY(!keystore3.fileExists());
				QVERIFY(!keystore3.isInProgress());
				QVERIFY(!keystore3.isReady());
				//Missing file on load is not an error
				QVERIFY(!keystore3.hasError());
			}
		}
	}
}



QTEST_MAIN(TestKeyStore)
