#include "TestKeyStore.hpp"


#include "security/KeyStore.hpp"
#include "security/Key.hpp"
#include "TestKeys.hpp"
#include <QSignalSpy>

void TestKeyStore::testInit()
{

	QFile file(fileName), *filep=&file;
	// Make sure we don't have an existing file already
	if(filep->exists()) {
		QVERIFY(filep->remove());
	}
	// Initialize the keystore, verifying generation switch works both on and off
	QVERIFY(!filep->exists());
	{
		KeySecurityPolicy quickAndInsecurePolicy(128, QCryptographicHash::Md5);
		KeyStore keystoreA(fileName, false, quickAndInsecurePolicy), *keystorep=&keystoreA;
		QVERIFY(nullptr!=keystorep);
		//keystore.store().setSynchronousMode(true);
		qDebug()<<"A-1. "<< keystoreA.store().journal();
		QVERIFY(!filep->exists());
		QVERIFY(!keystoreA.ready());
		keystoreA.waitForSync();
		qDebug()<<"A-2. "<< keystoreA.store().journal();
		QVERIFY(!filep->exists());
		QVERIFY(!keystoreA.ready());
		keystoreA.setBootstrapEnabled(true);
		keystoreA.clear();
		qDebug()<<"A-3. "<< keystoreA.store().journal();
		QVERIFY(!filep->exists());
		QVERIFY(!keystoreA.ready());
		keystoreA.waitForSync();
		qDebug()<<"A-4. "<< keystoreA.store().journal();
		QVERIFY(filep->exists());
		QVERIFY(keystoreA.ready());
		keystoreA.waitForSync();
		qDebug()<<"A-5. "<< keystoreA.store().journal();
	}
	// Now we expect the file to exist and the store to load it on initialization
	QVERIFY(filep->exists());
	{
		KeyStore keystoreB(fileName);
		qDebug()<<"B-1. "<< keystoreB.store().journal();
		QVERIFY(filep->exists());
		QVERIFY(!keystoreB.ready());
		keystoreB.waitForSync();
		qDebug()<<"B-2. "<< keystoreB.store().journal();
		QVERIFY(filep->exists());
		QVERIFY(keystoreB.ready());
		QVERIFY(keystoreB.fileExists());
	}
	// Now we expect the file not to exist
	QVERIFY(filep->remove());
	QVERIFY(!filep->exists());
	{
		KeyStore keystoreC(fileName);
		qDebug()<<"C-1. "<< keystoreC.store().journal();
		QVERIFY(!filep->exists());
		QVERIFY(!keystoreC.fileExists());
		qDebug()<<"C-2. "<< keystoreC.store().journal();
	}
}




void TestKeyStore::testInsertRemove()
{
	const TestKeys tk;
	QVariantMap keyMap;
	keyMap["privateKey"]=""; // Use empty private key on purpose, as associates will not have it (it is included above for completeness only)
	keyMap["publicKey"]=tk.pubKeyPEM;
	Key key(keyMap, true);
	QCOMPARE(key.id(), tk.keyID);


	QFile file(fileName), *filep=&file;
	if(filep->exists()) {
		QVERIFY(filep->remove());
	}
	QVERIFY(!filep->exists());
	//KeySecurityPolicy quickAndInsecurePolicy(128, QCryptographicHash::Md5);
	KeyStore keystore(fileName, true), *keystorep=&keystore;
	QVERIFY(nullptr!=keystorep);
	QVERIFY(!filep->exists());
	keystore.waitForSync();
	QVERIFY(filep->exists());

	QVERIFY(!keystore.hasPubKeyForID(tk.keyID));
	keystore.setPubKeyForID(tk.pubKeyPEM);
	QVERIFY(keystore.hasPubKeyForID(tk.keyID));
	auto k2=keystore.pubKeyForID(tk.keyID);
	QVERIFY(nullptr != k2);
	QCOMPARE(*k2, key);

	const QByteArray raw="This is some text to be signed and verified";
	const QByteArray bad="This is some text that will fail verification";


	auto lkey=keystore.localKey();
	QVERIFY(!lkey.isNull());
	const QByteArray sign = lkey->sign(raw);

	QVERIFY(lkey->verify(raw, sign));
	QVERIFY(!lkey->verify(bad, sign));

	QVERIFY(keystore.hasPubKeyForID(tk.keyID));
	keystore.clear();
	QVERIFY(keystore.hasPubKeyForID(tk.keyID));
	keystore.waitForSync();
	QVERIFY(!keystore.hasPubKeyForID(tk.keyID));
}



OC_TEST_MAIN(test, TestKeyStore)


