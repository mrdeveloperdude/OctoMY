#include "TestLocalAddressList.hpp"

#include "address/LocalAddressList.hpp"

#include "comms/NetworkAddress.hpp"

static QMap<QString, QString>  dumbMap;
static const QString dumbString("LocalAddressList{NOT CONFIGURED}");

static void ensureLALState(LocalAddressList &lal
						   , quint16 port
						   , QHostAddress address
						   , NetworkAddress networkAddress
						   , bool isUpdateNeeded
						   , bool isActivated
						   , QString string
						   , QMap<QString, QString>  map
						   , bool skipString=false
						   , bool skipMap=false
						  )
{
	qDebug()<<"LOLBOB port="<<port<<", address="<<address<<", networkAddress="<<networkAddress;
	QCOMPARE(lal.port(), port);
	QCOMPARE(lal.currentAddress(), address);
	QCOMPARE(lal.currentNetworkAddress(), networkAddress);
	QCOMPARE(lal.isUpdateNeeded(), isUpdateNeeded);
	QCOMPARE(lal.isActivated(), isActivated);
	if(!skipString) {
		QCOMPARE(lal.toString(), string);
	}
	if(!skipMap) {
		QCOMPARE(lal.toMap(), map);
	}
}

static void ensureLALIsDumb(LocalAddressList &lal)
{
	ensureLALState(lal, 0, QHostAddress(), NetworkAddress(), false, false, "LocalAddressList{NOT CONFIGURED}", dumbMap);
}

void TestLocalAddressList::test()
{
	dumbMap[QString("NOT CONFIGURED")]="NOT CONFIGURED";
	const QHostAddress unavailable("8.8.8.8");
	LocalAddressList lal;
	// Before we configure it all selectors should reply defaults, even if we try to set values
	ensureLALIsDumb(lal);
	lal.setPort(1234);
	ensureLALIsDumb(lal);
	lal.setCurrent(unavailable, 4321);
	ensureLALIsDumb(lal);
	QCOMPARE(lal.updateIfNeeded(), false);
	ensureLALIsDumb(lal);

	// After we configure it all selectors should reply real values that reflect what we have set
	lal.configure(2345);
	auto addr=lal.currentAddress();
	auto naddr=lal.currentNetworkAddress();
	ensureLALState(lal, 2345, addr, naddr, false, false, dumbString, dumbMap, true, true);
	lal.setPort(2346);
	naddr.setPort(2346);
	ensureLALState(lal, 2346, addr, naddr, false, false, dumbString, dumbMap, true, true);

	addr=unavailable;
	naddr.fromString(unavailable.toString());
	auto curAddr=lal.currentAddress();
	auto curNAddr=lal.currentNetworkAddress();
	// Should not have changed since we tried to change to invalid address
	lal.setCurrent(unavailable, 4321);
	curNAddr.setPort(4321);
	QCOMPARE(lal.currentAddress(), curAddr);
	QCOMPARE(lal.currentNetworkAddress(), curNAddr);
	QCOMPARE(lal.port(), 4321);
	quint16 port=6789;
	for(auto valid:lal){
		addr=valid;
		naddr.setIP(valid);
		qDebug()<<"ROL: valid="<<valid<<", addr="<<addr<<", naddr="<<naddr<<", port="<<port;
		// Should change since we are changing to known valid addresses
		lal.setCurrent(addr, port);
		naddr.setPort(port);
		QCOMPARE(lal.currentAddress(), addr);
		QCOMPARE(lal.currentNetworkAddress(), naddr);
		QCOMPARE(lal.port(), port);
		port+=123;
	}
	QCOMPARE(lal.isActivated(), false);
	lal.activate(true);
	QCOMPARE(lal.isActivated(), true);
	lal.activate(false);
	QCOMPARE(lal.isActivated(), false);
}


OC_TEST_MAIN(test, TestLocalAddressList)

