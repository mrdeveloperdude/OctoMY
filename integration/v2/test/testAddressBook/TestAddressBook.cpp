#include "TestAddressBook.hpp"

#include "discovery/AddressBook.hpp"

#include "address/AddressList.hpp"


#include "Keys_test.hpp"

void TestAddressBook::test()
{
	const TestKeys tk;
	const QString ip="10.0.0.1";
	const quint16 port=8123;
	const QString filename="TestNodeAssociateStore.json";


	QVariantMap keyMap;

	keyMap["privateKey"]="";//Use empty private key on purpose, as associates will not have it (it is included above for completeness only)
	keyMap["publicKey"]=tk.pubKeyPEM;
	AddressList addressList;
	addressList.merge(NetworkAddress(QHostAddress(ip),port));
	QVariantMap assMap;
	assMap["key"]=keyMap;
	const NodeType type=TYPE_AGENT;
	const NodeRole role=ROLE_AGENT;
	const QString pin="12345";
	const QString pin2="54321";
	QStringList trusts;
	trusts <<"trust-one";
	trusts <<"trust-two";
	assMap["role"]=nodeRoleToString(role);
	assMap["type"]=nodeTypeToString(type);
	assMap["addressList"]=addressList.toVariantList();
	assMap["trusts"]=trusts;

	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
	QVERIFY(nullptr!=ass);

	QCOMPARE(ass->id(),tk.keyID);
	QCOMPARE(ass->type(),type);
	QCOMPARE(ass->role(),role);
	QVERIFY(ass->isValidForClient(true));


	qDebug()<<"ORIG:" <<addressList.toString();
	qDebug()<<"COPY:" <<ass->addressList().toString();
	QCOMPARE(ass->addressList().size(), addressList.size());

	QCOMPARE(ass->trusts(), trusts);

	QStringList t=ass->trusts();
	qDebug()<<"TRUSTS: "<<t;

	Key k=ass->key();
	qDebug()<<"PUBKEY: "<<k.pubKey();
	qDebug()<<"PRIVKEY: "<<k.key();
	qDebug()<<"ALL: "<<k.toString();
	QCOMPARE(k.isValid(false),false);
	QVERIFY(!ass->isValidForClient(false));
	QVERIFY(!ass->isValidForServer());


	const QStringList &pins=ass->pins();
	QCOMPARE(pins.size(),0);
	ass->addPin(pin);
	const QStringList &pins2=ass->pins();
	QCOMPARE(pins2.size(),1);
	QStringList l1;
	l1<<pin;
	QCOMPARE(pins2,l1);
	ass->addPin(pin2);
	const QStringList &pins3=ass->pins();
	QCOMPARE(pins3.size(),2);
	QStringList l2;
	l2<<pin;
	l2<<pin2;
	QCOMPARE(pins3,l2);
	ass->clearPins();
	const QStringList &pins4=ass->pins();
	QCOMPARE(pins4.size(),0);

	//void addTrust(QString trust);
	//////////////////////////////////

	QString ta1="trust-added-one";
	QVERIFY(!ass->hasTrust(ta1));
	QVERIFY(ass->hasTrust(trusts[0]));
	QVERIFY(ass->hasTrust(trusts[1]));
	ass->addTrust(ta1);
	QVERIFY(ass->hasTrust(ta1));
	QVERIFY(ass->hasTrust(trusts[0]));
	QVERIFY(ass->hasTrust(trusts[1]));
	ass->removeTrust(ta1);
	QVERIFY(!ass->hasTrust(ta1));
	QVERIFY(ass->hasTrust(trusts[0]));
	QVERIFY(ass->hasTrust(trusts[1]));

	QVariantMap map=ass->toVariantMap();
	qDebug()<<"MAP: "<<map;

	QSharedPointer<Associate> ass1b(OC_NEW Associate());
	QVERIFY(nullptr!=ass1b);

	//bool operator==(const NodeAssociate &o) const;
	////////////////////////////////////////////////////


	QVERIFY(!(*ass1b == *ass));

	//bool operator!=(const NodeAssociate &o) const;
	////////////////////////////////////////////////////

	QVERIFY(*ass1b != *ass);

	//void fromVariantMap(const QVariantMap map);
	//////////////////////////////////////////////////

	ass1b->fromVariantMap(map);


	//QString toString();
	//////////////////////

	qDebug()<<ass->toString();
	qDebug()<<ass1b->toString();

	// TODO: Fix the operator== const issue in Key.*
	//QCOMPARE(*ass1b, *ass);

	//QVERIFY(*ass1b == *ass);

	//QVERIFY(!(*ass1b != *ass));

	//NetworkAddress &publicAddress();
	////////////////////////////////////////////////////

	//NetworkAddress &localAddress();
	////////////////////////////////////////////////////

	//QBluetoothAddress &bluetoothAddress();
	////////////////////////////////////////////////////

	//quint64 lastSeen() const;
	////////////////////////////////////////////////////

	QFile file(filename);
	if(file.exists()) {
		file.remove();
	}

	QVERIFY(!file.exists());

	//Create an empty store and add data to it before saving
	{
		AddressBook *store=OC_NEW AddressBook();
		QVERIFY(nullptr!=store);

		store->configure(filename);
		/*
		store->bootstrap(false, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store->isReady());
		QVERIFY(!store->isInProgress());
		QVERIFY(!store->hasError());
		*/
		QVERIFY(!store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));

		store->upsertAssociate(ass);

		QVERIFY(store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));

		QSharedPointer<Associate> ass2=store->associateByID(tk.keyID);

		QVERIFY(nullptr!=ass2);
		QCOMPARE(ass,ass2);

		QSharedPointer<Associate> ass3=store->removeAssociate(tk.keyID);

		QVERIFY(nullptr!=ass3);
		QCOMPARE(ass,ass2);
		QCOMPARE(ass2,ass3);
		QCOMPARE(ass3->id(),tk.keyID);

		QSharedPointer<Associate> ass4=store->removeAssociate(tk.keyID);

		QVERIFY(nullptr==ass4);

		QSharedPointer<Associate> ass5=store->associateByID(tk.keyID);

		QVERIFY(nullptr==ass5);

		store->upsertAssociate(ass);

		QVERIFY(store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));

		// Implicitly saves data
		delete store;
		store=nullptr;
	}

	//Create a new store, this time expect the file to exist from last step (load only)
	{
		AddressBook *store2=OC_NEW AddressBook();
		QVERIFY(nullptr!=store2);
		store2->configure(filename);

		/*
		store2->bootstrap(true, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store2->isReady());
		QVERIFY(!store2->isInProgress());
		QVERIFY(!store2->hasError());
		*/
		QVERIFY(store2->hasAssociate(tk.keyID));
		QVERIFY(!store2->hasAssociate(tk.keyID_bad));

		QSharedPointer<Associate> ass6=store2->associateByID(tk.keyID);

		QVERIFY(nullptr!=ass6);


		QCOMPARE(ass6->id(),tk.keyID);
		QCOMPARE(ass6->type(),type);
		QCOMPARE(ass6->role(),role);
		QVERIFY(ass6->isValidForClient(true));

		QCOMPARE(ass6->trusts(), trusts);

		QStringList t=ass->trusts();
		qDebug()<<"TRUSTS6: "<<t;

		Key k=ass6->key();
		qDebug()<<"PUBKEY: "<<k.pubKey();
		qDebug()<<"PRIVKEY: "<<k.key();
		qDebug()<<"ALL: "<<k.toString();
		QCOMPARE(k.isValid(false),false);
		QVERIFY(!ass6->isValidForClient(false));
		QVERIFY(!ass6->isValidForServer());

		// Implicitly saves data
		delete store2;
		store2=nullptr;
	}

	if(file.exists()) {
		file.remove();
	}
	QVERIFY(!file.exists());

}



OC_TEST_MAIN(test, TestAddressBook)



/*


*/
