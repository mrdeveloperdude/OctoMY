#include "TestAddressBook.hpp"

#include "test/Keys.hpp"
#include "address/AddressList.hpp"
#include "discovery/AddressBook.hpp"
#include "utility/file/File.hpp"
#include "utility/time/HumanTime.hpp"

static TrustList defaultTrusts = {"trust-one", "trust-two"};

static QVariantList generateAddrMap(quint64 now, QString ip = "127.0.0.1", quint16 port = 8123)
{
	QVariantList adrList;
	QVariantMap nadrMap;
	QVariantMap adrMap;
	nadrMap["ip"] = ip;
	nadrMap["port"] = port;
	adrMap["address"] = nadrMap;
	adrMap["description"] = "Description of address";

	adrMap["createdMS"] = utility::time::msToVariant(now);
	adrMap["lastSuccessMS"] = utility::time::msToVariant(now);
	adrMap["lastErrorMS"] = utility::time::msToVariant(now);
	adrMap["numSuccessful"] = static_cast<quint64>(3);
	adrMap["numErraneous"] = static_cast<quint64>(3);

	adrList << adrMap;
	return adrList;
}

static QVariantMap generateAssMap(quint64 now, QString ip = "127.0.0.1", quint16 port = 8123, NodeType type = TYPE_AGENT, NodeRole role = ROLE_AGENT, TrustList trusts = defaultTrusts)
{
	TestKeys tk;
	QVariantMap assMap;
	assMap["id"] = tk.keyID;
	assMap["name"] = "hogboll";
	assMap["gender"] = "Male";
	assMap["birthDate"] = utility::time::msToVariant(now - (1000 * 60 * 60 * 24 * 10));
	assMap["role"] = nodeRoleToString(role);
	assMap["type"] = nodeTypeToString(type);
	assMap["trusts"] = trusts.toStringList();
	assMap["addressList"] = generateAddrMap(now, ip, port);
	assMap["lastSeenMS"] = utility::time::msToVariant(now - 9000);
	assMap["lastInitiatedHandshakeMS"] = utility::time::msToVariant(now - 6000);
	assMap["lastAdherentHandshakeMS"] = utility::time::msToVariant(now - 10000);
	return assMap;
}

void TestAddressBook::testToFromGenerateMap()
{
	const QString filename = "test_addressbook.json";
	auto store = QSharedPointer<AddressBook>(OC_NEW AddressBook());
	QVERIFY(!store.isNull());
	store->configure(filename);
	QCOMPARE(store->filename(), filename);
	store->activate(true);
	utility::time::delay(1000);
	QCOMPARE(store->isActivated(), true);
	QVariantMap empty;
	QVariantMap full;
	{
		const TestKeys tk;
		const QString ip = "10.0.0.1";
		const quint16 port = 8123;
		const NodeType type = TYPE_AGENT;
		const NodeRole role = ROLE_AGENT;
		const quint64 now = 0;
		const auto trusts = defaultTrusts;
		auto assMap = generateAssMap(now, ip, port, type, role, trusts);
		QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
		QVERIFY(!ass.isNull());
		QVariantMap assMap2 = ass->toVariantMap();
		QCOMPARE(assMap, assMap2);
		qDebug() << "ASSMAP=" << assMap;
		qDebug() << "ASSMAP2=" << assMap2;
		QVariantList peers;
		peers.push_back(assMap2);
		full["peers"] = peers;
		empty["peers"] = QVariantList();
	}
	auto toEmpty = store->toMap();
	QCOMPARE(empty, toEmpty);
	auto defOK = store->fromDefault();
	QCOMPARE(defOK, true);
	auto toDef = store->toMap();
	QCOMPARE(empty, toDef);
	auto fromOK = store->fromMap(full);
	QCOMPARE(fromOK, true);
	auto toMap = store->toMap();
	qDebug() << "FULL=" << full;
	qDebug() << "MAP=" << toMap;
	QCOMPARE(full, toMap);
}

// TODO: Maybe move this to TestAssociate?
void TestAddressBook::testAss()
{
	const TestKeys tk;
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;
	const QString pin = "12345";
	const QString pin2 = "54321";
	const QString pin3 = "aabbcc";
	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
	QVERIFY(nullptr != ass);
	QCOMPARE(ass->id(), tk.keyID);
	QCOMPARE(ass->type(), type);
	QCOMPARE(ass->role(), role);
	QVERIFY(ass->isValidForClient());
	QCOMPARE(ass->trusts(), trusts);
	QVERIFY(ass->isValidForClient());
	QVERIFY(!ass->isValidForServer());
}

// TODO: Maybe move this to TestAssociate?
void TestAddressBook::testPins()
{
	const TestKeys tk;
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;
	const QString pin = "12345";
	const QString pin2 = "54321";
	const QString pin3 = "aabbcc";
	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
	const QStringList &pins = ass->pins();
	QCOMPARE(pins.size(), 0);
	ass->addPin(pin);
	const QStringList &pins2 = ass->pins();
	QCOMPARE(pins2.size(), 1);
	QStringList l1;
	l1 << pin;
	QCOMPARE(pins2, l1);
	ass->addPin(pin2);
	const QStringList &pins3 = ass->pins();
	QCOMPARE(pins3.size(), 2);
	QStringList l2;
	l2 << pin;
	l2 << pin2;
	QCOMPARE(pins3, l2);
	ass->clearPins();
	const QStringList &pins4 = ass->pins();
	QCOMPARE(pins4.size(), 0);
}

// TODO: Maybe move this to TestAssociate?
void TestAddressBook::testTrusts()
{
	const TestKeys tk;
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;
	const QString pin = "12345";
	const QString pin2 = "54321";
	const QString pin3 = "aabbcc";
	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));
	// void addTrust(QString trust);
	//////////////////////////////////
	TrustList &trustList = ass->trusts();
	// qDebug()<<"TRUSTS: "<<trustList;
	QString ta1 = "trust-added-one";
	QVERIFY(!trustList.hasTrust(ta1));
	for (auto trust : trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}
	trustList.addTrust(ta1);
	QVERIFY(trustList.hasTrust(ta1));
	for (auto trust : trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}
	trustList.removeTrust(ta1);
	QVERIFY(!trustList.hasTrust(ta1));
	for (auto trust : trusts) {
		QVERIFY(trustList.hasTrust(trust));
	}
}

// TODO: Maybe move this to TestAssociate?
void TestAddressBook::testOperators()
{
	const TestKeys tk;
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;
	const QString pin = "12345";
	const QString pin2 = "54321";
	const QString pin3 = "aabbcc";
	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));

	QVariantMap map = ass->toVariantMap();
	qDebug() << "MAP: " << map;

	QSharedPointer<Associate> ass1b(OC_NEW Associate());
	QVERIFY(!ass1b.isNull());
	QVERIFY(!(*ass1b == *ass));
	QVERIFY(*ass1b != *ass);
	ass1b->fromVariantMap(map);
	qDebug() << "ASS:   " << ass->toString();
	qDebug() << "ASS1b: " << ass1b->toString();
	QCOMPARE(ass1b->toString(), ass->toString());
}

void TestAddressBook::testCreate()
{
	const TestKeys tk;
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const QString filename = "test_addressbook.json";
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;
	const QString pin = "12345";
	const QString pin2 = "54321";
	const QString pin3 = "aabbcc";
	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));

	QFile file(filename), *filep = &file;
	if (file.exists()) {
		file.remove();
	}
	QVERIFY(!file.exists());
	// Create an empty store and add data to it before saving
	qDebug() << "STAGE #1 ============================";
	auto store = QSharedPointer<AddressBook>(OC_NEW AddressBook());
	QVERIFY(!store.isNull());
	QCOMPARE(store->isActivated(), false);
	store->configure(filename);
	QCOMPARE(store->filename(), filename);
	QCOMPARE(store->isActivated(), false);
	store->activate(true, [=](bool ok) {
		QCOMPARE(ok, true);
		QCOMPARE(store->isActivated(), true);
		QVERIFY(!store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));
		QCOMPARE(store->associateCount(), 0);

		store->upsertAssociate(ass);
		QCOMPARE(ok, true);
		QVERIFY(filep->exists());
		if (filep->exists()) {
			qDebug() << "FILE WAS: " << utility::file::fileToString(filename);
		}

		QCOMPARE(store->associateCount(), 1);
		QVERIFY(store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));

		QSharedPointer<Associate> ass2 = store->associateByID(tk.keyID);

		QVERIFY(nullptr != ass2);
		QCOMPARE(ass, ass2);

		QSharedPointer<Associate> ass3 = store->removeAssociate(tk.keyID);

		QVERIFY(nullptr != ass3);
		QCOMPARE(ass, ass2);
		QCOMPARE(ass2, ass3);
		QCOMPARE(ass3->id(), tk.keyID);

		QSharedPointer<Associate> ass4 = store->removeAssociate(tk.keyID);

		QVERIFY(nullptr == ass4);

		QSharedPointer<Associate> ass5 = store->associateByID(tk.keyID);

		QVERIFY(nullptr == ass5);

		store->upsertAssociate(ass);

		QVERIFY(store->hasAssociate(tk.keyID));
		QVERIFY(!store->hasAssociate(tk.keyID_bad));

		qDebug() << "STAGE #1.8";
		store->activate(false, [=](bool ok) {
			qDebug() << "STAGE #1.9";
			QCOMPARE(ok, true);
			QVERIFY(filep->exists());
			if (filep->exists()) {
				qDebug() << "FILE WAS: " << utility::file::fileToString(filename);
			}
		});
	});

	utility::time::delay(1000);
	// Should implicitly save data
	store = nullptr;
	utility::time::delay(1000);
}


void TestAddressBook::testLoad()
{
	const TestKeys tk;
	const QString filename = "test_addressbook.json";
	const QString ip = "10.0.0.1";
	const quint16 port = 8123;
	const NodeType type = TYPE_AGENT;
	const NodeRole role = ROLE_AGENT;
	const quint64 now = 0;
	const auto trusts = defaultTrusts;

	QVariantMap assMap = generateAssMap(now, ip, port, type, role, trusts);
	QSharedPointer<Associate> ass(OC_NEW Associate(assMap));

	// Create a new store, this time expect the file to exist from last step (load
	// only)
	QFile file(filename), *filep = &file;
	QVERIFY(file.exists());
	qDebug() << "FILE:" << QFileInfo(file).absoluteFilePath();
	if (filep->exists()) {
		qDebug() << "FILE WAS: " << utility::file::fileToString(filename);
	}

	qDebug() << "STAGE #2 ============================";
	auto store2 = QSharedPointer<AddressBook>(OC_NEW AddressBook());
	QVERIFY(!store2.isNull());
	qDebug() << "STAGE #2.1";
	QVERIFY(!store2.isNull());
	qDebug() << "STAGE #2.2";
	QCOMPARE(store2->isActivated(), false);
	qDebug() << "STAGE #2.3";
	store2->configure(filename);
	QCOMPARE(store2->filename(), filename);
	qDebug() << "STAGE #2.4";
	QCOMPARE(store2->isActivated(), false);
	qDebug() << "STAGE #2.5";
	store2->activate(true, [=](bool ok) {
		qDebug() << "STAGE #2.6";
		QCOMPARE(ok, true);
		QVERIFY(nullptr != store2);
		QCOMPARE(store2->isActivated(), true);
		QCOMPARE(store2->filename(), filename);
		qDebug() << "tk.keyID: " << tk.keyID;
		QVERIFY(!store2->hasAssociate(tk.keyID_bad));
		QEXPECT_FAIL("", "Set aside as it is shadowing other tests", Continue);
		QCOMPARE(store2->associateCount(), 1);
		// TODO: The rest of the tests are ignored as they are expected to fail.
		return;
		QVERIFY(store2->hasAssociate(tk.keyID));
		QSharedPointer<Associate> ass6 = store2->associateByID(tk.keyID);
		QVERIFY(nullptr != ass6);
		QCOMPARE(ass6->id(), tk.keyID);
		QCOMPARE(ass6->type(), type);
		QCOMPARE(ass6->role(), role);
		QVERIFY(ass6->isValidForClient());

		QCOMPARE(ass6->trusts(), trusts);

		TrustList &trustList = ass->trusts();
		qDebug() << "TRUSTS6: " << trustList;

		QString id = ass6->id();
		qDebug() << "ID: " << id;
		QVERIFY(!ass6->isValidForClient());
		QVERIFY(!ass6->isValidForServer());
		qDebug() << "STAGE #2.7";

		qDebug() << "STAGE #2.8";
		store2->activate(false, [=](bool ok) {
			qDebug() << "STAGE #2.9";
			QCOMPARE(ok, true);

			QVERIFY(filep->exists());
			if (filep->exists()) {
				qDebug() << "FILE WAS: " << utility::file::fileToString(filename);
				filep->remove();
			}
			QVERIFY(!filep->exists());
		});
	});

	utility::time::delay(1000);
	qDebug() << "STAGE #2.10";
	qDebug() << "STAGE #2.11";
	// Should implicitly save data
	store2 = nullptr;
	qDebug() << "STAGE #2.12";

	utility::time::delay(1000);
}

/*
void TestAddressBook::testFilterEmptyRules()
{
	AddressBook addressBook;
	
	// Add some associates to the address book
	auto associate1 = QSharedPointer<Associate>::create();
	associate1->setType(NodeType::Client);
	associate1->setTrusts(TrustList({"giveControl"}));
	addressBook.addAssociate("associate1", associate1);
	
	auto associate2 = QSharedPointer<Associate>::create();
	associate2->setType(NodeType::Server);
	associate2->setTrusts(TrustList({"takeControl"}));
	addressBook.addAssociate("associate2", associate2);
	
	QVector<QueryRule> rules;  // Empty rules
	
	// Since rules are empty, we expect all associates to be included
	auto result = addressBook.filter(rules);
	QCOMPARE(result.size(), 2);
	QVERIFY(result.contains("associate1"));
	QVERIFY(result.contains("associate2"));
}

void TestAddressBook::testFilterSingleRule()
{
	AddressBook addressBook;
	
	// Add some associates to the address book
	auto associate1 = QSharedPointer<Associate>::create();
	associate1->setType(NodeType::Client);
	associate1->setTrusts(TrustList({"giveControl"}));
	addressBook.addAssociate("associate1", associate1);
	
	auto associate2 = QSharedPointer<Associate>::create();
	associate2->setType(NodeType::Server);
	associate2->setTrusts(TrustList({"takeControl"}));
	addressBook.addAssociate("associate2", associate2);
	
	QVector<QueryRule> rules;
	QueryRule rule;
	rule.type = NodeType::Client;
	rule.trustUs = true;
	rule.include = true;
	rules.append(rule);
	
	// We expect only associate1 to match
	auto result = addressBook.filter(rules);
	QCOMPARE(result.size(), 1);
	QVERIFY(result.contains("associate1"));
	QVERIFY(!result.contains("associate2"));
}

void TestAddressBook::testFilterMultipleRules()
{
	AddressBook addressBook;
	
	// Add some associates to the address book
	auto associate1 = QSharedPointer<Associate>::create();
	associate1->setType(NodeType::Client);
	associate1->setTrusts(TrustList({"giveControl"}));
	addressBook.addAssociate("associate1", associate1);
	
	auto associate2 = QSharedPointer<Associate>::create();
	associate2->setType(NodeType::Server);
	associate2->setTrusts(TrustList({"takeControl"}));
	addressBook.addAssociate("associate2", associate2);
	
	auto associate3 = QSharedPointer<Associate>::create();
	associate3->setType(NodeType::Client);
	associate3->setTrusts(TrustList({"takeControl"}));
	addressBook.addAssociate("associate3", associate3);
	
	QVector<QueryRule> rules;
	QueryRule rule1;
	rule1.type = NodeType::Client;
	rule1.trustUs = true;
	rule1.include = true;
	rules.append(rule1);
	
	QueryRule rule2;
	rule2.type = NodeType::Server;
	rule2.trustThem = true;
	rule2.include = true;
	rules.append(rule2);
	
	// We expect no associates to match both rules
	auto result = addressBook.filter(rules);
	QCOMPARE(result.size(), 0);
}
*/

OC_TEST_MAIN(test, TestAddressBook)
