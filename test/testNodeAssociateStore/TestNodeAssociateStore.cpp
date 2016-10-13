#include "TestNodeAssociateStore.hpp"

#include "discovery/NodeAssociateStore.hpp"



// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestNodeAssociateStore::test(){
	//http://travistidwell.com/jsencrypt/demo/
	const QString key="-----BEGIN RSA PRIVATE KEY-----\n"
					  "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\n"
					  "-----END RSA PRIVATE KEY-----\n";

	const QString pubKey="-----BEGIN PUBLIC KEY-----\n"
						 "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAhfwrwmldif7ReZEqrMOd\n"
						 "rDNQfgexYttfWjfHyReQzKiolDUrbVLBlSAGk/3R6MKfLjm+dHSJAQRyfp8daJMg\n"
						 "YsYU3NQildb3mnjb3pevJC9DtSnX59aR9xyAuU5Ohm1LU0LcmL3S8p002YRL3U/0\n"
						 "6g1ZDuW2UQU8uuvx8AtVnHN4IKhbHLCOaxdHJ8vItvVJ3wTAXMsG+iOWCFEzej/s\n"
						 "6nN3qhNhQ3W0jWpC4d+Z6VIwUNWMq5Zz1HThZ9ZoSPYUIDivjhRaQtN5zrZyPjOz\n"
						 "47r5etO4Rc07JCe7Gdvh6lx8coxA1DyAPVM6WEVcvfSucVVokd7lzvLcO3n41m/t\n"
						 "WfRZsUkDR0MCdlRFycQ9LwF6ae/d/OD+vkZ4q0sobdwjqvZ85FUsIivxDACCpZHX\n"
						 "t6tgLO74g0eWYRmfsq8JEEtTW0gjI7vpXoRsC2N0KWDAVN2Cw7f8qtneU9LyVDvx\n"
						 "J69K0tn5nBj5aZFVVoDpaLLCy2SmGSf7x4HqUeE9daah1J9YFs7TMq7wZ9Cx/F2P\n"
						 "pT8OvSPpX4uq1OLma5T0A0BxbnxJkmQxFPQJo0yXTgybY+GV82JRHvhAOmObNomv\n"
						 "UgDsyr52AnWOI1ilJcX8EPSR7Jc5gLow8++j+SBrcZDDJ+fAIvmNO2a8wZjVWT+G\n"
						 "GvVy7USArMzrGfjWcX90ugUCAwEAAQ==\n"
						 "-----END PUBLIC KEY-----\n";
	const QString id="BC68E2093C30E393159B404B5A39F1AB7CE9E9FA452CD0F2C81563BB19F10DE5";
	const QString badId="BAD8E2093C30E393159B404B5A39F1AB7CE9E9FA452CD0F2C81563BB19F10DE5";
	const QString ip="127.0.0.1";
	const quint16 port=8123;
	const QString filename="TestNodeAssociateStore.json";


	QVariantMap keyMap;

	keyMap["privateKey"]="";//Use empty private key on purpose, as associates will not have it (it is included above for completeness only)
	keyMap["publicKey"]=pubKey;
	QVariantMap localAddrMap;
	localAddrMap["ip"]=ip;
	localAddrMap["port"]=port;
	QVariantMap publicAddrMap;
	publicAddrMap["ip"]=ip;
	publicAddrMap["port"]=port;
	QVariantMap assMap;
	assMap["key"]=keyMap;
	const DiscoveryType type=TYPE_AGENT;
	const DiscoveryRole role=ROLE_AGENT;
	const QString pin="12345";
	const QString pin2="54321";
	QStringList trusts;
	trusts <<"trust-one";
	trusts <<"trust-two";
	assMap["role"]=DiscoveryRoleToString(role);
	assMap["type"]=DiscoveryTypeToString(type);
	assMap["publicAddress"]=publicAddrMap;
	assMap["localAddress"]=localAddrMap;
	assMap["trusts"]=trusts;

	QSharedPointer<NodeAssociate> ass(new NodeAssociate(assMap));

	QCOMPARE(ass->id(),id);
	QCOMPARE(ass->type(),type);
	QCOMPARE(ass->role(),role);
	QVERIFY(ass->isValidForClient(true));

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

	ClientSignature sig=ass->toClientSignature();

	qDebug()<<"SIG: "<<sig.toString();

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


	QVariantMap map=ass->toVariantMap();
	qDebug()<<"MAP: "<<map;

	//void fromVariantMap(const QVariantMap map);
	//////////////////////////////////////////////////

	//QString toString();
	//////////////////////

	//bool operator==(const NodeAssociate &o) const;
	////////////////////////////////////////////////////

	//bool operator!=(const NodeAssociate &o) const;
	////////////////////////////////////////////////////

	//NetworkAddress &publicAddress();
	////////////////////////////////////////////////////

	//NetworkAddress &localAddress();
	////////////////////////////////////////////////////

	//QBluetoothAddress &bluetoothAddress();
	////////////////////////////////////////////////////

	//quint64 lastSeen() const;
	////////////////////////////////////////////////////

	QFile file(filename);
	if(file.exists()){
		file.remove();
	}

	//Create an empty store and add data to it before saving
	{
		NodeAssociateStore *store=new NodeAssociateStore(filename);
		QVERIFY(nullptr!=store);
		store->bootstrap(false, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store->isReady());
		QVERIFY(!store->isInProgress());
		QVERIFY(!store->hasError());

		QVERIFY(!store->hasParticipant(id));
		QVERIFY(!store->hasParticipant(badId));

		store->setParticipant(ass);

		QVERIFY(store->hasParticipant(id));
		QVERIFY(!store->hasParticipant(badId));

		QSharedPointer<NodeAssociate> ass2=store->getParticipant(id);

		QVERIFY(nullptr!=ass2);
		QCOMPARE(ass,ass2);

		QSharedPointer<NodeAssociate> ass3=store->removeParticipant(id);

		QVERIFY(nullptr!=ass3);
		QCOMPARE(ass,ass2);
		QCOMPARE(ass2,ass3);
		QCOMPARE(ass3->id(),id);

		QSharedPointer<NodeAssociate> ass4=store->removeParticipant(id);

		QVERIFY(nullptr==ass4);

		QSharedPointer<NodeAssociate> ass5=store->getParticipant(id);

		QVERIFY(nullptr==ass5);

		store->setParticipant(ass);

		QVERIFY(store->hasParticipant(id));
		QVERIFY(!store->hasParticipant(badId));

		delete store;
		store=nullptr;
	}

	//Create a new store, this time expect the file to exist from last step (load only)
	{
		NodeAssociateStore *store2=new NodeAssociateStore(filename);
		QVERIFY(nullptr!=store2);
		store2->bootstrap(true, false); // Leave async test to TestKeyStore and TestAsyncStore

		QVERIFY(store2->isReady());
		QVERIFY(!store2->isInProgress());
		QVERIFY(!store2->hasError());

		QVERIFY(store2->hasParticipant(id));
		QVERIFY(!store2->hasParticipant(badId));

		QSharedPointer<NodeAssociate> ass6=store2->getParticipant(id);

		QVERIFY(nullptr!=ass6);


		QCOMPARE(ass6->id(),id);
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

		delete store2;
		store2=nullptr;
	}

	if(file.exists()){
		file.remove();
	}


}



QTEST_MAIN(TestNodeAssociateStore)



/*


*/
