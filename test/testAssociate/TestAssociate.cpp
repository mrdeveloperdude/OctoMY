#include "TestAssociate.hpp"

#include "address/Associate.hpp"
#include "address/AddressEntry.hpp"

#include "utility/time/HumanTime.hpp"
#include "uptime/New.hpp"

#include "Keys_test.hpp"

static const TestKeys tk;
static const QString ip="127.0.0.1";
static const quint16 port=8123;

static const NodeType type=TYPE_AGENT;
static const NodeRole role=ROLE_AGENT;
static const QString pin="12345";
static const QString pin2="54321";

const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();


static QStringList trusts= {
	"trust-one", "trust-two"
};


static QVariantMap generateMap()
{
	QVariantMap keyMap;

	//keyMap["privateKey"]="";//Use empty private key on purpose, as associates will not have it (it is included above for completeness only)
	keyMap["publicKey"]=tk.pubKeyPEM;
	keyMap["id"]=tk.keyID;

	QVariantMap assMap;
	assMap["key"]=keyMap;

	/*
	QVariantMap localAddrMap;
	localAddrMap["ip"]=ip;
	localAddrMap["port"]=port;
	assMap["localAddress"]=localAddrMap;

	QVariantMap publicAddrMap;
	publicAddrMap["ip"]=ip;
	publicAddrMap["port"]=port;
	assMap["publicAddress"]=publicAddrMap;
	*/


	assMap["name"]="hogboll";
	assMap["gender"]="Male";
	assMap["birthDate"]=utility::time::msToVariant(now-(1000*60*60*24*10));

	assMap["role"]=nodeRoleToString(role);
	assMap["type"]=nodeTypeToString(type);
	assMap["trusts"]=trusts;

	QVariantList adrList;

	QVariantMap nadrMap;
	QVariantMap adrMap;
	nadrMap["ip"]=ip;
	nadrMap["port"]=port;
	adrMap["address"]=nadrMap;
	adrMap["description"]="Description of address";

	adrMap["createdMS"]=utility::time::msToVariant(now);
	adrMap["lastSuccessMS"]=utility::time::msToVariant(now);
	adrMap["lastErrorMS"]=utility::time::msToVariant(now);
	adrMap["numSuccessful"]=static_cast<quint64>(3);
	adrMap["numErraneous"]=static_cast<quint64>(3);

	adrList<<adrMap;
	assMap["addressList"]=adrList;

	assMap["lastSeenMS"]=utility::time::msToVariant(now-9000);
	assMap["lastInitiatedHandshakeMS"]=utility::time::msToVariant(now-6000);
	assMap["lastAdherentHandshakeMS"]=utility::time::msToVariant(now-10000);

	return assMap;
}


void TestAssociate::test()
{
	QVariantMap assMap=generateMap();

	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));

	QCOMPARE(ass->id(), tk.keyID);
	QCOMPARE(ass->type(), type);
	QCOMPARE(ass->role(), role);

	QCOMPARE(ass->key().isValid(true), true);
	QCOMPARE(ass->addressList().isValid(false), true);

	QVERIFY(ass->isValidForClient(true));

	QCOMPARE(ass->trusts(), trusts);
	QStringList t=ass->trusts();
	qDebug()<<"TRUSTS: "<<t;

	QVERIFY(ass->hasTrust("trust-one"));
	QVERIFY(ass->hasTrust("trust-two"));
	QVERIFY(!ass->hasTrust("trust-three"));
	ass->addTrust("trust-three");
	QVERIFY(ass->trusts()!=trusts);
	QVERIFY(ass->hasTrust("trust-one"));
	QVERIFY(ass->hasTrust("trust-two"));
	QVERIFY(ass->hasTrust("trust-three"));
	ass->removeTrust("trust-three");
	QVERIFY(!ass->hasTrust("trust-three"));

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
}


void TestAssociate::testMapConversions()
{
	QVariantMap assMap1=generateMap();

	QSharedPointer<Associate> ass1(OC_NEW Associate(assMap1));

	QCOMPARE(ass1->id(), tk.keyID);
	QCOMPARE(ass1->type(), type);
	QCOMPARE(ass1->role(), role);

	QCOMPARE(ass1->key().isValid(true), true);

	auto key=ass1->key();

	QCOMPARE(key.pubKey(), tk.pubKeyPEM);
	QCOMPARE(key.key(), "");
	auto keyMap=key.toVariantMap(true);
	qDebug()<<"KEYMAP: "<<keyMap;
	QCOMPARE(keyMap["privateKey"], QVariant());
	QCOMPARE(keyMap["publicKey"], tk.pubKeyPEM);

	QCOMPARE(ass1->addressList().isValid(false), true);

	QVERIFY(ass1->isValidForClient(true));

	QVariantMap assMap2=ass1->toVariantMap();

	qDebug()<<"ASSMAP-1: "<<assMap1;
	qDebug()<<"ASSMAP-2: "<<assMap2;
	QCOMPARE(assMap1, assMap2);

	QSharedPointer<Associate> ass2(OC_NEW Associate(assMap2));

	Associate &assRef1=*ass1.data();
	Associate &assRef2=*ass2.data();

	qDebug()<<"ASS-1: "<<assRef1;
	qDebug()<<"ASS-2: "<<assRef2;
	QVERIFY(assRef1 == assRef2);

	/*
	map["addressList"]=mAddressList.toVariantList();
	map["lastSeenMS"]=utility::msToVariant(mLastSeenMS);
	map["lastInitiatedHandshakeMS"]=utility::msToVariant(mLastInitiatedHandshakeMS);
	map["lastAdherentHandshakeMS"]=utility::msToVariant(mLastAdherentHandshakeMS);
	map["birthDate"]=utility::msToVariant(mBirthDate);
	map["key"]=mKey.toVariantMap(true);
	map["role"]=nodeRoleToString(mRole);
	map["type"]=nodeTypeToString(mType);
	map["name"]=mName;
	map["gender"]=mGender;
	//map["pins"]=mPins;//DONT STORE PINS THEY ARE EPHEMERAL
	map["trusts"]=mTrusts;
	*/
}


void TestAssociate::testTimeConversions()
{
	QDateTime dtNow=QDateTime::currentDateTimeUtc();
	quint64 tsNow=utility::time::currentMsecsSinceEpoch<quint64>();
	QVariant vNow;
	vNow=dtNow;
	QVariant vNow2;
	vNow2=QDateTime::fromMSecsSinceEpoch(tsNow, Qt::UTC);
	quint64 tsNow2=vNow.toDateTime().toMSecsSinceEpoch();
	qDebug()<<"dtNow="<<dtNow<<", vNow="<<vNow<<", tsNow="<<tsNow<<", vNow2="<<vNow2<<", tsNow2="<<tsNow2;

	quint64 ms1=utility::time::variantToMs(dtNow);
	QVariant v1=utility::time::msToVariant(ms1);
	QCOMPARE(v1.toDateTime(), dtNow);
	quint64 ms2=utility::time::variantToMs(v1);
	QCOMPARE(ms2, ms2);

}


OC_TEST_MAIN(test, TestAssociate)
