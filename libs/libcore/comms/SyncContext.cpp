#include "SyncContext.hpp"
#include "ISyncParameter.hpp"

#include <QDateTime>
#include <QDataStream>



SyncContext::SyncContext(quint64 rtt, quint64 now)
	: mNow(0)
	, mRTT(rtt)
	, mIdealSendingPayloadSize(0)
	, mLastSendingPayloadSize(0)
	, mLastReceivingPayloadSize(0)
	, mAverageRoundTripTime(0)

{
}


void SyncContext::flush(quint64 ts)
{
	for(QVector<ISyncParameter *>::const_iterator it=mParams.begin(), e=mParams.end(); it!=e; ++it) {
		(*it)->flush();
	}
	mSyncBits.fill(false);
	update(ts);
}

// The output of bytesSent(), hasPendingSync() and hasPendingAcks() should never change between calls to update(). This is the only way to have mandate mechanism of comms to work properly
// TODO: Look at architectual changes that will improve this interface , as it might be considered a flaw
void SyncContext::update(quint64 ts)
{
	if(mAckBits.size()<1) {
		qWarning()<<"ERROR: ack bits missing";
		return;
	}
	if(mSyncBits.size()<1) {
		qWarning()<<"ERROR: sync bits missing";
		return;
	}
	if(0==ts) {
		ts=QDateTime::currentMSecsSinceEpoch();
	}
	mNow=ts;
	// Recalculate state
	const quint16 oldPayloadSize=mIdealSendingPayloadSize;
	mIdealSendingPayloadSize=0;
	int index=0;
	int pendingSyncsAdded=0;
	int pendingAcksAdded=0;
	// Add size of now
	mIdealSendingPayloadSize+=(sizeof(mNow));
	//Detect new parameters requesting sync/ack
	for(QVector<ISyncParameter *>::const_iterator it=mParams.begin(), e=mParams.end(); it!=e; ++it) {
		if((*it)->hasPendingSync()) {
			//qDebug().nospace().noquote()<<" + add sync bit "<<index;
			mSyncBits.setBit(index, true);
			pendingSyncsAdded++;
		}
		if((*it)->hasPendingAck()) {
			//qDebug().nospace().noquote()<<" + add ack bit "<<index;
			mAckBits.setBit(index, true);
			pendingAcksAdded++;
		}
		index++;
	}
	//Add size for data of all active sync parameters
	const int num=mSyncBits.size();
	for(int i=0; i<num; ++i) {
		if(mSyncBits.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				if(param->hasPendingSync()) {
					const quint16 b=param->bytes();
					//qDebug().nospace().noquote()<<" + sync bytes "<<i<<" ("<<b<<")";
					mIdealSendingPayloadSize+=b;
				} else {
					qWarning()<<"ERROR: Sync bit "<<i<<" was on while parameter had no pending sync";
				}
			} else {
				qWarning()<<"ERROR: nullptr = param for sync bit "<<i;
			}
		}
	}
	// Add size of ack bits
	mIdealSendingPayloadSize+=(sizeof(quint32));
	mIdealSendingPayloadSize+=((mAckBits.size()+7)/8);
	// Add size of sync bits
	mIdealSendingPayloadSize+=(sizeof(quint32));
	mIdealSendingPayloadSize+=((mSyncBits.size()+7)/8);
	//qDebug().nospace().noquote()<<"SYNC CONTEXT UPDATE AT "<<ts<< ": idealSize="<<mIdealSendingPayloadSize<<"(old="<<oldPayloadSize<<"), pendingSyncs="<<mSyncBits<< "(+"<<pendingSyncsAdded<<", ct="<<mSyncBits.count(true)<<"/"<<mSyncBits.size()<<"), pendingAcks="<<mAckBits<<"(+"<<pendingAcksAdded<<", ct="<<mAckBits.count(true)<<"/"<<mAckBits.size()<<")";
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

QDebug &SyncContext::toDebug(QDebug &d) const
{
	d.nospace() << "SyncContext(avgRTT="<<QString::number(mAverageRoundTripTime)<<", RTT="<<QString::number(mRTT)<<", syncBits="<<mSyncBits<<", ackBits="<<mAckBits<<"):\n";
	for(QVector<ISyncParameter *>::const_iterator it=mParams.begin(), e=mParams.end(); it!=e; ++it) {
		ISyncParameter &sp=*(*it);
		d.nospace() << sp<<"\n";
	}

	return d.maybeSpace();
}



quint16 SyncContext::bytesSent() const
{
	return mLastSendingPayloadSize;
}


quint16 SyncContext::bytesSentIdeal() const
{
	return mIdealSendingPayloadSize;
}


quint16 SyncContext::bytesRead() const
{
	return mLastReceivingPayloadSize;
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




QDebug &operator<<(QDebug &d, const SyncContext &sc)
{
	return sc.toDebug(d);
}



QDataStream &SyncContext::receive(QDataStream &ds)
{
	int bytes=0;
	//Current clock for estimating RTT
	ds >> mNow;
	bytes+=sizeof(mNow);
	//Sync bits
	QBitArray syncs;
	ds >> syncs;
	const int numSyncs=syncs.size();
	bytes+=(sizeof(quint32)+((numSyncs+7)/8));
	for(int i=0; i<numSyncs; ++i) {
		if(syncs.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				// Actual parameter data
				ds >> *param;
				bytes+=param->bytes();
			} else {
				qWarning()<<"ERROR: nullptr = param";
			}
		}
	}
	//Ack bits
	QBitArray acks;
	ds >> acks;
	const int numAcks=acks.size();
	bytes+=(sizeof(quint32)+((numAcks+7)/8));
	for(int i=0; i<numAcks; ++i) {
		if(acks.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				// Ack
				param->ack();
				acks.setBit(i,false);
			} else {
				qWarning()<<"ERROR: Paramter "<< i<<" was null while trying to ack";
			}
		}
	}
	mLastReceivingPayloadSize=bytes;
	return ds;
}



QDataStream &SyncContext::send(QDataStream &ds)
{
	int bytes=0;
	//Current clock for estimating RTT
	quint64 now=0;
	const qint64 rttSample=mNow-now;
	mAverageRoundTripTime=(mAverageRoundTripTime*9+rttSample)/10;
	ds << mNow;
	bytes+=sizeof(mNow);
	// Sync bits
	ds << mSyncBits;
	const int numSyncs=mSyncBits.size();
	bytes+=(sizeof(quint32)+((numSyncs+7)/8));
	for(int i=0; i<numSyncs; ++i) {
		if(mSyncBits.testBit(i)) {
			ISyncParameter *param=mParams.at(i);
			if(nullptr!=param) {
				if(param->hasPendingSync()) {
					//Actual parameter data
					ds << *param;
					bytes+=param->bytes();
				} else {
					qWarning()<<"ERROR: Sync bit was on while parameter had no pending sync";
				}
			} else {
				qWarning()<<"ERROR: A little bird was lying 3";
			}
		}
	}
	//Ack bits
	ds << mAckBits;
	const int numAcks=mAckBits.size();
	bytes+=(sizeof(quint32)+((numAcks+7)/8));
	//Compare counted bytes with estimated bytes as a precaution
	if(bytes!=mIdealSendingPayloadSize) {
		qWarning()<<"ERROR: actual="<<bytes<<" vs. ideal="<<mIdealSendingPayloadSize<<" bytes sent";
	}
	mLastSendingPayloadSize=bytes;
	return ds;

}
