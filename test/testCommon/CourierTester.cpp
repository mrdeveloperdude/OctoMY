#include "CourierTester.hpp"

#include "../libcore/comms/couriers/Courier.hpp"
#include "../libutil/utility/Utility.hpp"

#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

#include <QTest>




CourierTester::CourierTester(Courier *fromCourier, Courier *toCourier, QString fromName, QString toName)
	: QObject(nullptr)
	, mFromCourier(fromCourier)
	, mToCourier(toCourier)
	, mFromName(fromName)
	, mToName(toName)
	, mDone(false)
	, mRoundCounter(0)
{

	mChanceNames<<mFromName+" - SEND   "
				<<mFromName+" - RECEIVE"
				<<mFromName+" - DROP   "
				<<mFromName+" - CORRUPT"
				<<mToName+" - SEND   "
				<<mToName+" - RECEIVE"
				<<mToName+" - DROP   "
				<<mToName+" - CORRUPT"
				;
}

void CourierTester::setProfile(CourierTesterProfile &p)
{
	mProfile=p;
}

void CourierTester::sleepRandom()
{
	const quint64 ms=(mProfile.sleepTimeMin())+(qrand()%mProfile.sleepTimeRange());
	if(ms>0) {
		qDebug()<<"WAITING "<<utility::humanReadableElapsedMS(ms);
		QTest::qWait(ms);
	}
}

void CourierTester::onTestInit()
{
	qDebug()<<"STARTING TEST ------------------------";
	QVERIFY(nullptr!=mFromCourier);
	QVERIFY(nullptr!=mToCourier);
	onTestInitImp();
	quint64 now=QDateTime::currentMSecsSinceEpoch();
	qDebug()<<"--- INITIAL UPDATE "<<mFromName;
	mFromCourier->update(now);
	qDebug()<<"--- INITIAL UPDATE "<<mToName;
	mToCourier->update(now);
	mRoundCounter=0;
}


void CourierTester::onTestDeInit()
{
	qDebug()<<"ENDING TEST ------------------------";
	onTestDeInitImp();
}

void CourierTester::onTestRoundStart()
{
	mRoundCounter++;
	sleepRandom();
	onTestRoundStartImp();
}


void CourierTester::onTestRoundEnd()
{
	qDebug()<<"--- ROUND END -----------";
	qDebug()<<"--- UPDATE "<<mFromName;
	quint64 now=QDateTime::currentMSecsSinceEpoch();
	mFromCourier->update(now);
	qDebug()<<"--- UPDATE "<<mToName;
	mToCourier->update(now);
	qDebug()<< "CourierTester stats: mFromStreams="<<mFromStreams.size()<<", mToStreams="<<mToStreams.size()<<"";
	onTestRoundEndImp();
}



QByteArray *CourierTester::fetchFromStream()
{
	const int count=mFromStreams.size();
	//QVERIFY(count>0);
	int index=0;
	switch(mProfile.order()) {
	default:
	case(FIFO): {
		index=0;
	}
	break;
	case(LIFO): {
		index=count-1;
	}
	break;
	case(RANDOM): {
		index=(qrand()%count);
	}
	break;
	}
	QByteArray *ba=mFromStreams.takeAt(index);
	//QVERIFY(nullptr!=ba);
	return ba;
}


QByteArray *CourierTester::fetchToStream()
{
	const int count=mToStreams.size();
	//QVERIFY(count>0);
	int index=0;
	switch(mProfile.order()) {
	default:
	case(FIFO): {
		index=0;
	}
	break;
	case(LIFO): {
		index=count-1;
	}
	break;
	case(RANDOM): {
		index=(qrand()%count);
	}
	break;
	}
	QByteArray *ba=mToStreams.takeAt(index);
	//QVERIFY(nullptr!=ba);
	return ba;
}

void CourierTester::onToReceiving()
{
	if( mProfile.betrayMandateReceiveActive() || mToCourier->mandate().receiveActive ) {
		if(!mFromStreams.isEmpty()) {
			QByteArray *ba=fetchFromStream();
			QVERIFY(nullptr!=ba);
			const qint64 sz=ba->size();
			quint16 received=0;
			{
				QBuffer buf(ba);
				buf.open(QBuffer::ReadOnly);
				QDataStream stream(&buf);
				received=mToCourier->dataReceived(stream, sz);
				buf.close();
			}
			onToReceivingImp();
			delete ba;
			qDebug()<<mToName<<" RECEIVED  "<<received<<" bytes vs "<<sz << " via buffer "<<ba;
			ba=nullptr;
		} else {
			qDebug()<<"NO DATA TO RECEIVE FOR " << mToName<< ", SKIPPING";
		}
	} else {
		qDebug()<<"No betrayal or no send active";
	}
}


void CourierTester::onToSend()
{
	if(  mProfile.betrayMandateSendActive() || mToCourier->mandate().sendActive  )  {
		QByteArray *ba=new QByteArray;
		QVERIFY(nullptr!=ba);
		quint16 sent=0;
		{
			QBuffer buf(ba);
			buf.open(QBuffer::WriteOnly);
			QDataStream stream(&buf);
			sent=mToCourier->sendingOpportunity(stream);
			buf.close();
		}
		onToSendImp();
		qDebug()<<mToName<<" SENT "<<sent<<" bytes ("<<ba->size()<<") via buffer "<<ba;
		mToStreams<<ba;
	} else {
		qDebug()<<"No betrayal or no send active";
	}
}


void CourierTester::onToDrop()
{
	qDebug()<<"TODO: Implement onToDrop";
}

void CourierTester::onToCorrupt()
{
	qDebug()<<"TODO: Implement onToCorrupt";
}

void CourierTester::onFromReceiving()
{
	if( mProfile.betrayMandateReceiveActive() || mFromCourier->mandate().receiveActive ) {
		if(!mToStreams.isEmpty()) {
			QByteArray *ba=fetchToStream();
			QVERIFY(nullptr!=ba);
			const qint64 sz=ba->size();
			quint16 received=0;
			{
				QBuffer buf(ba);
				buf.open(QBuffer::ReadOnly);
				QDataStream stream(&buf);
				received=mFromCourier->dataReceived(stream, sz);
				buf.close();
			}
			delete ba;
			onFromReceivingImp();
			qDebug()<<mFromName<<" RECEIVED "<<received<<" bytes vs "<<sz << " via buffer "<<ba;
			ba=nullptr;
		} else {
			qDebug()<<"NO DATA TO RECEIVE FOR " << mFromName<< ", SKIPPING";
		}
	} else {
		qDebug()<<"No betrayal or no send active";
	}
}

void CourierTester::onFromSend()
{
	if( mProfile.betrayMandateSendActive() || mFromCourier->mandate().sendActive ) {
		QByteArray *ba=new QByteArray;
		QVERIFY(nullptr!=ba);
		quint16 sent=0;
		{
			QBuffer buf(ba);
			buf.open(QBuffer::WriteOnly);
			QDataStream stream(&buf);
			sent=mFromCourier->sendingOpportunity(stream);
			buf.close();
		}
		onFromSendImp();
		qDebug()<<mFromName<<" SENT  "<<sent<<" bytes ("<<ba->size()<<") via buffer "<<ba;
		mFromStreams<<ba;
	} else {
		qDebug()<<"No betrayal or no send active";
	}
}


void CourierTester::onFromDrop()
{
	qDebug()<<"TODO: Implement onFromDrop";
}

void CourierTester::onFromCorrupt()
{
	qDebug()<<"TODO: Implement onFromCorrupt";
}



void CourierTester::putTitle(int chance)
{
	qDebug()<<"";
	qDebug().noquote()<<"---------------------- ROUND "<<mRoundCounter<<": " << mChanceNames[chance]<< " ------------------ ";



}


int CourierTester::randomStep()
{
	return mProfile.randomStep();
}

void CourierTester::testStep(int step)
{
	putTitle(step);
	onTestRoundStart();
	switch(step) {
	case(0): {
		onFromSend();
	}
	break;
	case(1): {
		onFromReceiving();
	}
	break;
	case(2): {
		onFromDrop();
	}
	break;
	case(3): {
		onFromCorrupt();
	}

	case(4): {
		onToSend();
	}
	break;
	case(5): {
		onToReceiving();
	}
	break;
	case(6): {
		onToDrop();
	}
	break;
	case(7): {
		onToCorrupt();
	}
	break;
	break;
	default: {
		qWarning()<<"ERROR: Unknown step: "<<step;
	}
	break;
	}
	onTestRoundEnd();
}

void CourierTester::testRandomSteps(int steps)
{
	for(int step=0; step<steps; ++step) {
		if(mDone) {
			break;
		}
		testStep(randomStep());
	}
}
