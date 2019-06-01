#include "CommsTester.hpp"

#include "random/RNG.hpp"
#include "../common_test/mock/MockCourier.hpp"


CommsTester::CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, KeyStore &keyStore, AddressBook &peers, QObject *parent)
	: QObject(parent)
	, mName(name)
	, mMyAddress(myAddress)
	, mMyPort(myPort)
	, mBasePort(basePort)
	, mPortRange(portRange)
	, mCarrier()
	, mKeyStore(keyStore)
	, mAssociates(peers)
	, mCc(mCarrier, mKeyStore, mAssociates)
	, mTestCount(testCount)
	, mRng(RNG::sourceFactory("mt"))
{
	QVERIFY(nullptr!=mRng);
	mRng->init(mMyPort);
	QVERIFY(mMyPort>=mBasePort);
	QVERIFY(mMyPort<=mBasePort+mPortRange);
	QVERIFY(connect(&mCc,SIGNAL(commsError(QString)),this,SLOT(onError(QString)),OC_CONTYPE));
	for(quint16 toPort=mBasePort; toPort<mBasePort+mPortRange; ++toPort) {
		if(mMyPort==toPort) {
			continue;
		}
		if(mRng->generateReal2()>0.7 || true) {
			qDebug() << mMyAddress << ":" << mMyPort << " --> " << toPort;
			QString myID="1234";
			//CommsSignature sig(myID, NetworkAddress(mMyAddress, toPort));
			QSharedPointer<MockCourier> tc(OC_NEW MockCourier(mName+"Courier", myID, "This is my humble payload", mCc, mTestCount, mTestCount, this));
			QVERIFY(!tc.isNull());
			mCc.setCourierRegistered(tc, true);
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
	mCc.carrier().setListenAddress(NetworkAddress(mMyAddress, mMyPort));
	mCc.carrier().setStarted(true);
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



