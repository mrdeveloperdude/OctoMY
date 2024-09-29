#include "CommsTester.hpp"

#include "random/RNG.hpp"
#include "mock/MockCourier.hpp"

#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"


#include <QTest>

CommsTester::CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, QSharedPointer<KeyStore> keyStore, QSharedPointer<AddressBook> peers, QObject *parent)
	: QObject(parent)
	, mName(name)
	, mMyAddress(myAddress)
	, mMyPort(myPort)
	, mBasePort(basePort)
	, mPortRange(portRange)
	, mCarrier(OC_NEW CommsCarrierUDP())
	, mKeyStore(keyStore)
	, mAssociates(peers)
	, mCc(OC_NEW CommsChannel())
	, mTestCount(testCount)
	, mRng(RNG::sourceFactory("mt"))
{
	mCc->configure(mCarrier, mKeyStore, mAssociates);
	QVERIFY(nullptr!=mRng);
	mRng->init(mMyPort);
	QVERIFY(mMyPort>=mBasePort);
	QVERIFY(mMyPort<=mBasePort+mPortRange);
	QVERIFY(connect(mCc.data(), SIGNAL(commsError(QString)), this, SLOT(onError(QString)), OC_CONTYPE));
	for(quint16 toPort=mBasePort; toPort<mBasePort+mPortRange; ++toPort) {
		if(mMyPort==toPort) {
			continue;
		}
		if(mRng->generateReal2()>0.7 || true) {
			qDebug() << mMyAddress << ":" << mMyPort << " --> " << toPort;
			QString myID="1234";
			//CommsSignature sig(myID, NetworkAddress(mMyAddress, toPort));
			auto tc = QSharedPointer<MockCourier> ::create(mName+"Courier", myID, "This is my humble payload", mCc, mTestCount, mTestCount, this);
			QVERIFY(!tc.isNull());
			mCc->registerCourier(tc, true);
		} else {
			qDebug() << mMyAddress << ":" << mMyPort << " --> SKIPPED";
		}
	}
}



void CommsTester::onError(QString e)
{
	qWarning()<<toString()<<"::onError(): "<<e;
}


void CommsTester::onReadyRead()
{
	qDebug()<<toString()<<"::onReadyRead()";
}


void CommsTester::startSendTest()
{
	qDebug()<<"Starting test for "<<toString();
	mCc->carrier()->setListenAddress(NetworkAddress(mMyAddress, mMyPort));
	mCc->carrier()->maintainConnection(true);
}


QString CommsTester::toString()
{
	QString ret=mName;
	ret+="(";
	ret+=mMyAddress.toString();
	ret+=":";
	ret+=QString::number(mMyPort);
	ret+=")";
	return ret;
}



