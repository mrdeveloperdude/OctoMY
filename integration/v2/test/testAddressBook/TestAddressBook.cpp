#include "TestAddressBook.hpp"

#include "discovery/AddressBook.hpp"

#include "address/AddressList.hpp"

#include "utility/file/File.hpp"
#include "utility/time/HumanTime.hpp"

#include "Keys_test.hpp"

void TestAddressBook::test()
{
	const TestKeys tk;
	const QString ip="10.0.0.1";
	const quint16 port=8123;
	const QString filename="test_addressbook.json";

	QVariantMap keyMap;

	// Use empty private key on purpose, as associates will not have it (it is included above for completeness only)
	keyMap["privateKey"]="";
	keyMap["publicKey"]=tk.pubKeyPEM;
	AddressList addressList;
	addressList.merge(NetworkAddress(QHostAddress(ip),port));
	QVariantMap assMap;
	assMap["key"]=keyMap;
	assMap["id"]=tk.keyID;
	const NodeType type=TYPE_AGENT;
	const NodeRole role=ROLE_AGENT;
	const QString pin="12345";
	const QString pin2="54321";
	const QString pin3="aabbcc";
	TrustList trusts;
	trusts <<"trust-one";
	trusts <<"trust-two";
	assMap["role"]=nodeRoleToString(role);
	assMap["type"]=nodeTypeToString(type);
	assMap["addressList"]=addressList.toVariantList();
	assMap["trusts"]=static_cast<QStringList>(trusts.toList());
	// NOTE: pins are not stored, hey should be ephemeral
	//QStringList origPins;	origPins << pin3; assMap["pins"]=origPins;

	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
	QVERIFY(nullptr!=ass);

	QCOMPARE(ass->id(), tk.keyID);
	QCOMPARE(ass->type(),type);
	QCOMPARE(ass->role(),role);
	QVERIFY(ass->isValidForClient(true));


	qDebug()<<"ORIG:" <<addressList.toString();
	qDebug()<<"COPY:" <<ass->addressList().toString();
	QCOMPARE(ass->addressList().size(), addressList.size());

	QCOMPARE(ass->trusts(), trusts);

	TrustList &trustList=ass->trusts();
	qDebug()<<"TRUSTS: "<<trustList;

	QString id=ass->id();
	qDebug()<<"ID: "<<id;
	QVERIFY(ass->isValidForClient(false));
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
	QVERIFY(!trustList.hasTrust(ta1));
	for(auto trust:trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}

	trustList.addTrust(ta1);
	QVERIFY(trustList.hasTrust(ta1));
	for(auto trust:trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}

	trustList.removeTrust(ta1);
	QVERIFY(!trustList.hasTrust(ta1));
	for(auto trust:trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}

	QVariantMap map=ass->toVariantMap();
	qDebug()<<"MAP: "<<map;

	QSharedPointer<Associate> ass1b(OC_NEW Associate());
	QVERIFY(!ass1b.isNull());

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

	qDebug()<<"ASS:   "<<ass->toString();
	qDebug()<<"ASS1b: "<<ass1b->toString();

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

	QFile file(filename), *filep=&file;
	if(file.exists()) {
		file.remove();
	}

	QVERIFY(!file.exists());

	//Create an empty store and add data to it before saving
	{
		qDebug()<<"STAGE #1 ============================";
		auto store=QSharedPointer<AddressBook>(OC_NEW AddressBook());
		QVERIFY(!store.isNull());
		QCOMPARE(store->isActivated(), false);
		store->configure(filename);
		QCOMPARE(store->isActivated(), false);
		store->activate(true, [=](bool ok) {
			QCOMPARE(ok, true);
			QCOMPARE(store->isActivated(), true);
//		store->setSynchronousMode(true);

			/*
			store->bootstrap(false, false); // Leave async test to TestKeyStore and TestAsyncStore

			QVERIFY(store->isReady());
			QVERIFY(!store->isInProgress());
			QVERIFY(!store->hasError());
			*/
			QVERIFY(!store->hasAssociate(tk.keyID));
			QVERIFY(!store->hasAssociate(tk.keyID_bad));

			store->upsertAssociate(ass, [=](bool ok) {
				QCOMPARE(ok, true);
				QVERIFY(filep->exists());
				if(filep->exists()) {
					qDebug()<<"FILE WAS: "<<utility::file::fileToString(filename);
				}

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

			});


		});
		qDebug()<<"STAGE #1.8";
		store->activate(false, [=](bool ok) {
			qDebug()<<"STAGE #1.9";
			QCOMPARE(ok, true);

			QVERIFY(filep->exists());
			if(filep->exists()) {
				qDebug()<<"FILE WAS: "<<utility::file::fileToString(filename);
			}

		});

		// Should implicitly save data
		store=nullptr;
	}

	//Create a new store, this time expect the file to exist from last step (load only)
	{
		qDebug()<<"STAGE #2 ============================";
		auto store2=QSharedPointer<AddressBook>(OC_NEW AddressBook());
		QVERIFY(!store2.isNull());
		qDebug()<<"STAGE #2.1";
		QVERIFY(!store2.isNull());
		qDebug()<<"STAGE #2.2";
		QCOMPARE(store2->isActivated(), false);
		qDebug()<<"STAGE #2.3";
		store2->configure(filename);
		qDebug()<<"STAGE #2.4";
		QCOMPARE(store2->isActivated(), false);
		qDebug()<<"STAGE #2.5";
		store2->activate(true, [=](bool ok) {
			qDebug()<<"STAGE #2.6";
			QCOMPARE(ok, true);
			QCOMPARE(store2->isActivated(), true);

			/*
			store2->bootstrap(true, false); // Leave async test to TestKeyStore and TestAsyncStore

			QVERIFY(store2->isReady());
			QVERIFY(!store2->isInProgress());
			QVERIFY(!store2->hasError());
			*/
			qDebug()<<"tk.keyID: "<<tk.keyID;
			QVERIFY(store2->hasAssociate(tk.keyID));
			QVERIFY(!store2->hasAssociate(tk.keyID_bad));

			QSharedPointer<Associate> ass6=store2->associateByID(tk.keyID);

			QVERIFY(nullptr!=ass6);


			QCOMPARE(ass6->id(),tk.keyID);
			QCOMPARE(ass6->type(),type);
			QCOMPARE(ass6->role(),role);
			QVERIFY(ass6->isValidForClient(true));

			QCOMPARE(ass6->trusts(), trusts);

			TrustList &trustList=ass->trusts();
			qDebug()<<"TRUSTS6: "<<trustList;

			QString id=ass6->id();
			qDebug()<<"ID: "<<id;
			QVERIFY(!ass6->isValidForClient(false));
			QVERIFY(!ass6->isValidForServer());
			qDebug()<<"STAGE #2.7";
		});
		qDebug()<<"STAGE #2.8";
		store2->activate(false, [=](bool ok) {
			qDebug()<<"STAGE #2.9";
			QCOMPARE(ok, true);

			QVERIFY(filep->exists());
			if(filep->exists()) {
				qDebug()<<"FILE WAS: "<<utility::file::fileToString(filename);
				filep->remove();
			}
			QVERIFY(!filep->exists());

		});
		qDebug()<<"STAGE #2.10";
		qDebug()<<"STAGE #2.11";
		// Should implicitly save data
		store2=nullptr;
		qDebug()<<"STAGE #2.12";

		utility::time::delay(10000);
	}


}

OC_TEST_MAIN(test, TestAddressBook)
