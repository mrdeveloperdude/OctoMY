#include "MockCourier.hpp"

#include "CommsTester.hpp"

#include "QTest"

MockCourier::MockCourier(QString name, QString  dest, QByteArray datagram, QSharedPointer<CommsChannel> comms, const qint32 maxSends , const qint32 maxRecs, CommsTester *parent)
	: Courier(name, Courier::FIRST_USER_ID+313, comms, parent)
	, mCt(parent)
	, mSoFar(0)
	, mDatagram(datagram)
	, mMaxSends(maxSends)
	, mMaxRecs(maxRecs)
	, mSendCount(0)
	, mRecCount(0)
	, mMandate(static_cast<quint16>(mDatagram.size()+4) // payloadSize
			   ,10 // priority
			   ,0 // nextSend. We set nextSend to 0. Basically this meaens "always overdue, send at every opportunity"
			   ,true // receiveActive
			   ,true // sendActive
			  )

{
	setDestination(dest);
}


MockCourier::~MockCourier()
{

}


// Let the CommChannel know what we want
CourierMandate MockCourier::mandate() const
{
	return mMandate;
}


// Override to act on sending opportunity.
// Return number of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 MockCourier::sendingOpportunity(QDataStream &ds )
{
	mSendCount++;
	const qint16 sendsLeft=static_cast<qint16>(mMaxSends-mSendCount);
	const bool done=(sendsLeft<=0);
	if(done) {
		qDebug()<<"MAX SENDS ("<<mMaxSends<<") RECEIVED FOR "<<destination()<<" STOPPING";
		mMandate.sendActive=false;
	}
	ds << mDatagram;
	qDebug()<<"SENDING OPPORTUNITY @ "<<destination()<< " bytes:" <<mMandate.payloadSize<<", "<<(done?"DONE":QString("SENDS LEFT: %1").arg(sendsLeft));
	if(nullptr!=mCt && done) {
		qDebug()<<"EMIT FINISHED --==##==--";
		emit mCt->finished();
	}
	return mMandate.payloadSize;
}


// Override to act on data received
// Return number of bytes actually read.
quint16 MockCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	mRecCount++;
	const qint16 recLeft=static_cast<qint16>(mMaxRecs-mRecCount);
	const bool done=(recLeft<=0);
	if(done) {
		qDebug()<<"MAX RECS ("<<mMaxRecs<<") RECEIVED FOR "<<destination()<<" STOPPING";
		mMandate.receiveActive=false;
	}

	QByteArray in;
	ds>>in;
	qDebug()<<"DATA BYTES RECEIVED="<<availableBytes<<" ("<<in<<")";
	return static_cast<quint16>(in.size()+4);
}


void MockCourier::writeSummary()
{
	qDebug()<<"";
	qDebug()<<" Test Courier Summary for "<<name();
	qDebug()<<"  + SoFar:                "<<mSoFar;
	qDebug()<<"  + mSendCount:           "<<mSendCount<<"/"<<mMaxSends;
	qDebug()<<"  + mRecCount:            "<<mRecCount<<"/"<<mMaxRecs;
}
