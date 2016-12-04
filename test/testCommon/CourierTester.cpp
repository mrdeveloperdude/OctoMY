#include "CourierTester.hpp"

#include "../../libs/libcore/comms/couriers/Courier.hpp"
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>

#include <QTest>




CourierTester::CourierTester(Courier *fromCourier, Courier *toCourier)
	: QObject(nullptr)
	, fromCourier(fromCourier)
	, toCourier(toCourier)
	, done(false)
{
}

void CourierTester::onTestInit()
{
	onTestInitImp();
	QVERIFY(nullptr!=fromCourier);
	QVERIFY(nullptr!=toCourier);
}


void CourierTester::onTestDeInit()
{
	qDebug()<<"TEST DONE, waiting for final show-off...";
	QTest::qWait(3000);
	onTestDeInitImp();
	qApp->processEvents();
}


void CourierTester::onToReceiving()
{
	if(!fromStreams.isEmpty()) {
		const int count=fromStreams.size();
		const int index=qrand()%count;
		QByteArray *ba=fromStreams.takeAt(index);
		QVERIFY(nullptr!=ba);
		const qint64 sz=ba->size();
		quint16 received=0;
		{
			QBuffer buf(ba);
			buf.open(QBuffer::ReadOnly);
			QDataStream stream(&buf);
			received=toCourier->dataReceived(stream, sz);
			buf.close();
		}
		onToReceivingImp();
		delete ba;
		ba=nullptr;
		//qDebug()<<" to rec "<<received<<" bytes vs "<<sz;
	} else {
		//qDebug()<<"NO-OP 1";
	}
}

void CourierTester::onFromReceiving()
{
	if(!toStreams.isEmpty()) {
		const int count=toStreams.size();
		QVERIFY(count>0);
		const int index=qrand()%count;
		QByteArray *ba=toStreams.takeAt(index);
		QVERIFY(nullptr!=ba);
		const qint64 sz=ba->size();
		quint16 received=0;
		{
			QBuffer buf(ba);
			buf.open(QBuffer::ReadOnly);
			QDataStream stream(&buf);
			received=fromCourier->dataReceived(stream, sz);
			buf.close();
		}
		delete ba;
		ba=nullptr;
		onFromReceivingImp();
		//qDebug()<<" from rec "<<received<<" bytes vs "<<sz;
	} else {
		// qDebug()<<"NO-OP 2";
	}
}

void CourierTester::onToSend()
{
	QByteArray *ba=new QByteArray;
	QVERIFY(nullptr!=ba);
	quint16 sent=0;
	{
		QBuffer buf(ba);
		buf.open(QBuffer::WriteOnly);
		QDataStream stream(&buf);
		sent=toCourier->sendingOpportunity(stream);
		buf.close();
	}
	onToSendImp();
	//qDebug()<<" to sendop "<<sent<<" bytes ("<<ba->size()<<")";
	toStreams<<ba;
}

void CourierTester::onFromSend()
{
	QByteArray *ba=new QByteArray;
	QVERIFY(nullptr!=ba);
	quint16 sent=0;
	{
		QBuffer buf(ba);
		buf.open(QBuffer::WriteOnly);
		QDataStream stream(&buf);
		sent=fromCourier->sendingOpportunity(stream);
		buf.close();
	}
	onFromSendImp();
	//qDebug()<<" from sendop "<<sent<<" bytes ("<<ba->size()<<")";
	fromStreams<<ba;
}


void CourierTester::onTestInitImp()
{

}


void CourierTester::onTestDeInitImp()
{

}

void CourierTester::onToReceivingImp()
{
}

void CourierTester::onFromReceivingImp()
{
}

void CourierTester::onToSendImp()
{
}

void CourierTester::onFromSendImp()
{
}

void CourierTester::test()
{
	onTestInit();
	while(!done) {
		//qDebug()<<"";		qDebug()<<"######################################## LOOP FROM="<<fromStreams.size()<<" TO="<<toStreams.size();
		// Give each party completely random chance of TX/RX to simulate some pretty bad network conditions
		const int chance=qrand()%5;
		switch(chance) {
		case(0): {
			onFromSend();
		}
		break;
		case(1): {
			onToSend();
		}
		break;
		case(2): {
			onToReceiving();
		}
		break;
		case(3): {
			onFromReceiving();
		}
		break;
		default:
		case(4):	{
			const int chance2=(qrand()%6);
			if(0==chance2) {
				// qDebug()<<"from remove";
			} else if(0==chance2) {
				// qDebug()<<"to remove";
			} else {
				// qDebug()<<"NO-OP 3";
			}
		}
		break;
		}
		//QVERIFY(false);

		//fromCourier->printSendingSummary("FROM");
		//	toCourier->printSendingSummary("TO");

	}
	onTestDeInit();
}
