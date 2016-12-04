#include "SyncContext.hpp"
#include "ISyncParameter.hpp"

#include <QDateTime>
#include <QDataStream>



SyncContext::SyncContext(quint64 rtt, quint64 now)
	: mNow(0)
	, mRTT(rtt)
	, mPayloadSize(0)
	, mAverageRoundTripTime(0)

{
	update(now);
}

// The output of bytes() and hasPendingSync() should never change between calls to update(). This is the only way to have mandate mechanism of comms to work properly
// TODO: Look at architectual changes that will improve this interface , as it can be considered a flaw
void SyncContext::update(quint64 ts)
{
	if(0==ts) {
		ts=QDateTime::currentMSecsSinceEpoch();
	}
	mNow=ts;
	// Recalculate state
	mPayloadSize=0;
	int index=0;
	for(QVector<ISyncParameter *>::const_iterator it=mParams.begin(), e=mParams.end(); it!=e; ++it) {
		if((*it)->hasPendingSync()) {
			mSyncBits.setBit(index, true);
			mPayloadSize+=(*it)->bytes();
		}
		if((*it)->hasPendingAck()) {
			mAckBits.setBit(index, true);
		}
		index++;
	}
	mPayloadSize+=(sizeof(quint32)+((mSyncBits.size()+7)/8));
	mPayloadSize+=(sizeof(quint32)+((mAckBits.size()+7)/8));
}

quint64 SyncContext::roundTripTime() const
{
	return mRTT;
}

void SyncContext::setRoundTripTime(quint64 rtt)
{
	mRTT=rtt;
}

quint64 SyncContext::now() const
{
	return mNow;
}

QString SyncContext::toString() const
{
	QString out="SyncContext(avgRTT="+QString::number(mAverageRoundTripTime)+", RTT="+QString::number(mRTT)+"):\n";
	for(QVector<ISyncParameter *>::const_iterator it=mParams.begin(), e=mParams.end(); it!=e; ++it) {
		out+=(*it)->toString()+"\n";
	}
	return out;
}



quint16 SyncContext::bytes() const
{
	return mPayloadSize;
}

bool SyncContext::hasPendingSyncs() const
{
	return (mSyncBits.count(true) > 0);
	return false;
}



bool SyncContext::hasPendingAcks() const
{
	return (mAckBits.count(true) > 0);
	return false;
}





QDataStream &SyncContext::operator<<(QDataStream &ds)
{
	int bytes=0;
	//Current clock for estimating RTT
	ds << mNow;
	bytes+=sizeof(mNow);
	//Sync bits
	ds << mSyncBits;
	bytes+=(sizeof(quint32)+((mSyncBits.size()+7)/8));
	const int num=mSyncBits.size();
	for(int i=0; i<num; ++i) {
		if(mSyncBits.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				if(param->hasPendingSync()) {
					//Actual parameter data
					param->operator <<(ds);
					bytes+=param->bytes();
				} else {
					qWarning()<<"ERROR: A little bird was lying 1";
				}
			} else {
				qWarning()<<"ERROR: A little bird was lying 2";
			}
		}
	}
	//Ack bits
	ds << mAckBits;
	bytes+=(sizeof(quint32)+((mAckBits.size()+7)/8));
	//Compare counted bytes with estimated bytes as a precaution
	if(bytes!=mPayloadSize) {
		qWarning()<<"ERROR: bytes "<<bytes<<" vs. "<<mPayloadSize<<" during sync was bad";
	}
	return ds;
}


QDataStream &SyncContext::operator>>(QDataStream &ds)
{
	int bytes=0;
	//Current clock for estimating RTT
	quint64 now=0;
	ds << now;
	const qint64 rttSample=mNow-now;
	mAverageRoundTripTime=(mAverageRoundTripTime*9+rttSample)/10;
	bytes+=sizeof(now);
	// Sync bits
	QBitArray syncs;
	ds >> syncs;
	const int numSyncs=syncs.size();
	bytes+=numSyncs;
	for(int i=0; i<numSyncs; ++i) {
		if(syncs.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				// Actual parameter data
				param->operator >>(ds);
				bytes+=param->bytes();
			} else {
				qWarning()<<"ERROR: A little bird was lying 3";
			}
		}
	}
	//Ack bits
	QBitArray acks;
	ds << acks;
	const int numAcks=syncs.size();
	bytes+=numAcks;
	for(int i=0; i<numAcks; ++i) {
		if(acks.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				// Ack
				param->ack();
			} else {
				qWarning()<<"ERROR: A little bird was lying 4";
			}
		}
	}
	/*
	//Compare counted bytes with estimated bytes as a precaution
	if(bytes!=mPayloadSize) {
		qWarning()<<"ERROR: bytes "<<bytes<<" vs. "<<mPayloadSize<<" during ack was bad";
	}
	*/
	return ds;

}
