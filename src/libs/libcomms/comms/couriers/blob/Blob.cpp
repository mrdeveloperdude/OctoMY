#include "Blob.hpp"

#include "BlobChunk.hpp"

#include "uptime/MethodGate.hpp"

#include "utility/string/String.hpp"
#include "utility/time/HumanTime.hpp"

#include <QDateTime>
#include <QDebug>


Blob::Blob(const QString name, const quint16 id, const QByteArray data, const quint32 chunkSize)
	: mName(name)
	, mID(id)
	, mData(data)
	, mChunkSize(chunkSize)
	, mTotalIndecies(  static_cast<int> ( (static_cast<unsigned int>(mData.size())/mChunkSize) + ((static_cast<unsigned int>(mData.size())%mChunkSize)>0?1:0) ) )
{
	OC_METHODGATE();
	qDebug()<<"Created blob with name='"<<mName<<"', id="<<mID<<" dataSize="<<utility::string::humanReadableSize(mData.size())<<" chunkSize="<<utility::string::humanReadableSize(mChunkSize)<<", totalIndecies="<<mTotalIndecies;
}


Blob::~Blob()
{
	OC_METHODGATE();
}



QString Blob::name()const
{
	OC_METHODGATE();
	return mName;
}

quint16 Blob::id()const
{
	OC_METHODGATE();
	return mID;
}

QByteArray Blob::data(int index)const
{
	OC_METHODGATE();
	QByteArray out=mData.mid( index* static_cast<int>(mChunkSize) , static_cast<int>(mChunkSize) );
	return out;
}


char *Blob::dataRef(int index)
{
	OC_METHODGATE();
	char *out=mData.data();
	if(nullptr!=out) {
		char *ret=(out+static_cast<unsigned int>(index)*mChunkSize);
		return ret;
	}
	return nullptr;
}




QByteArray Blob::data()const
{
	OC_METHODGATE();
	return mData;
}


quint64 Blob::dataSize()const
{
	OC_METHODGATE();
	return static_cast<quint64>(mData.size());
}

quint32 Blob::chunkSize() const
{
	OC_METHODGATE();
	return mChunkSize;
}


void Blob::setName(QString name)
{
	OC_METHODGATE();
	//TODO: Scrutinize name
	mName=name;
}

int Blob::numTotal() const
{
	OC_METHODGATE();
	return mTotalIndecies;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



SendingBlob::SendingBlob(const QString name, const quint16 id, const quint32 chunkSize, const QByteArray data, const quint8 priority)
	: Blob(name, id, data, chunkSize)
	, mPriority(priority)
	, mMaxInTransit((mTotalIndecies<=10)?(mTotalIndecies>0?1:0):(mTotalIndecies/10)) //10% and at least 1
	, mNameSent(false)
	, mNameAcknowleged(false)
	, mUnsentIndex(0)
	, mUnacknowlegedIndex(0)
//	, mLastAcknowlegedTime(0)
	, mLastResendTime(0)
	, mIsSent(mTotalIndecies)
	, mIsAcknowleged(mTotalIndecies)
{
	OC_METHODGATE();
	qDebug()<<" This is a sending blob with additional parameters priority="<<mPriority<<", maxInTransit="<<mMaxInTransit;
}



SendingBlob::~SendingBlob()
{
	OC_METHODGATE();
}



bool SendingBlob::isDone()const
{
	OC_METHODGATE();
	return (mIsAcknowleged.count(true)>=mTotalIndecies ) && mNameAcknowleged;
}



qreal SendingBlob::sendProgress()const
{
	OC_METHODGATE();
	if(isDone()) {
		return 1.0;
	}
	//For simplicity's sake we asume average completion of inTransit blobs is 50%
	qreal ret=0;//(qreal)mLastAcknowlegedIndex+(qreal)mInTransit.size()/2.0;
	ret/=mTotalIndecies;
	ret=qMin(1.0,ret);
	return ret;
}










int SendingBlob::numSent() const
{
	OC_METHODGATE();
	return mIsSent.count(true);
}

int SendingBlob::numAcknowleged() const
{
	OC_METHODGATE();
	return mIsAcknowleged.count(true);
}




int SendingBlob::firstUnsent() const
{
	OC_METHODGATE();
	int i=0;
	for(const int sz=mIsSent.size(); (i<sz) && mIsSent.testBit(i); ++i);
	return i-1;
}



int SendingBlob::firstUnacknowleged() const
{
	OC_METHODGATE();
	int i=0;
	for(const int sz=mIsAcknowleged.size(); (i<sz) && mIsAcknowleged.testBit(i); ++i);
	return i-1;
}



SendingBlobChunk SendingBlob::findNextSendingChunk()
{
	OC_METHODGATE();
	const int inTransit=numSent();
	const int acknowleged=numAcknowleged();
	//qDebug()<<"NEXT SEND CHUNK: inTransit="<<inTransit<<", acknowleged"<<acknowleged<<", mTotalIndecies"<<mTotalIndecies<<", mMaxInTransit"<<mMaxInTransit<<", mUnsentIndex"<<mUnsentIndex<<", mUnacknowlegedIndex"<<mUnacknowlegedIndex;
	if(acknowleged<mTotalIndecies) {
		int nextInTransit=-1;
		if((inTransit-acknowleged)<=mMaxInTransit) {
			if(mUnsentIndex>=mTotalIndecies) {
				mUnsentIndex=firstUnsent();
			}
			nextInTransit=mUnsentIndex;
			//qDebug()<<"SUGGESTING TO SEND "<<nextInTransit;
			mUnsentIndex++;
		} else {
			const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
			const quint64 BLOB_CHUNK_TIMEOUT=10;//TTL
			//Time for resend?
			qint64 diff=static_cast<qint64>(now-BLOB_CHUNK_TIMEOUT)-static_cast<qint64>(mLastResendTime);
			if(diff>0) {
				//Resend last
				mUnacknowlegedIndex++;
				if(mUnacknowlegedIndex>=mTotalIndecies) {
					mUnacknowlegedIndex=firstUnacknowleged();
				}
				nextInTransit=mUnacknowlegedIndex;
				//qDebug()<<"SUGGESTING TO RESEND "<< nextInTransit;
			} else {
				//qDebug()<<"TIME NOT RIGHT FOR RESEND YET "<< diff;
			}
		}
		if(nextInTransit > -1) {
			//qDebug()<<"RETURNING WITH "<<nextInTransit;
			return SendingBlobChunk(this, static_cast<quint32>(nextInTransit));
		}
	}
	// Default: Go away
	return SendingBlobChunk();
}


// Chunk data //////////////////////////////////////////

bool SendingBlob::isSent(int index)const
{
	OC_METHODGATE();
	if(index<0) {
		return false;
	}
	if(index>=mIsSent.size()) {
		return false;
	}
	return mIsSent.testBit(index);
}


bool SendingBlob::isAcknowleged(int index)const
{
	OC_METHODGATE();
	if(index<0) {
		return false;
	}
	if(index>=mIsAcknowleged.size()) {
		return false;
	}
	return mIsAcknowleged.testBit(index);
}


void SendingBlob::setSent(int index)
{
	OC_METHODGATE();
	if(index<0) {
		qWarning()<<"ERROR: trying to set send for index < 0: "<<index;
		return;
	}
	if(index>=mIsSent.size()) {
		qWarning()<<"ERROR: trying to set send for index > size: "<<index<<" vs. "<<mIsSent.size();
		return;
	}
	mIsSent.setBit(index);
	mLastResendTime=utility::time::currentMsecsSinceEpoch<quint64>();
}


void SendingBlob::setAcknowleged(int index)
{
	OC_METHODGATE();
	if(index<0) {
		qWarning()<<"ERROR: trying to set ack for index < 0: "<<index;
		return;
	}
	if(index>=mIsAcknowleged.size()) {
		qWarning()<<"ERROR: trying to set ack for index > size: "<<index<<" vs. "<<mIsAcknowleged.size();
		return;
	}
	mIsAcknowleged.setBit(index);
}

QBitArray SendingBlob::isSent()
{
	OC_METHODGATE();
	return mIsSent;
}


QBitArray SendingBlob::isAcknowleged()
{
	OC_METHODGATE();
	return mIsAcknowleged;
}


SendingBlobChunk SendingBlob::chunk(int index)
{
	OC_METHODGATE();
	if(index<0) {
		return SendingBlobChunk();
	}
	if(index>=mTotalIndecies) {
		return SendingBlobChunk();
	}
	return SendingBlobChunk(this, static_cast<quint32>(index));
}



// Name //////////////////////////////////////////


bool SendingBlob::isNameSent()
{
	OC_METHODGATE();
	return mNameSent;

}


bool SendingBlob::isNameAcknowleged()
{
	OC_METHODGATE();
	return mNameAcknowleged;
}


void SendingBlob::setNameSent(bool v)
{
	OC_METHODGATE();
	mNameSent=v;
}




void SendingBlob::setNameAcknowleged(bool v)
{
	OC_METHODGATE();
	mNameAcknowleged=v;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



ReceivingBlob::ReceivingBlob(const QString name, const quint16 id, const quint32 chunkSize, const quint32 byteSize)
	: Blob(name, id, QByteArray( static_cast<int>(byteSize), 0x00), chunkSize)
	, mIsReceived(mTotalIndecies)
	, mLastReceivedTime(0)
	, mNameReceived(false)

{
	OC_METHODGATE();
	qDebug()<<"Created receiving blob with mName='"<<mName<<"' with mChunkSize="<<mChunkSize<< ", mData.size="<<mData.size()<< "("<<byteSize<<")";
}




ReceivingBlob::~ReceivingBlob()
{
	OC_METHODGATE();
}




ReceivingBlobChunk ReceivingBlob::chunk(int index)
{
	OC_METHODGATE();
	if(index<0) {
		return ReceivingBlobChunk();
	}
	if(index>=mTotalIndecies) {
		return ReceivingBlobChunk();
	}
	return ReceivingBlobChunk(this, static_cast<quint32>(index) );
}



bool ReceivingBlob::isDone()const
{
	OC_METHODGATE();
	return (mIsReceived.count(true)>=mTotalIndecies ) && mNameReceived;
}



bool ReceivingBlob::isReceived(int index)const
{
	OC_METHODGATE();
	if(index<0) {
		return false;
	}
	if(index>=mIsReceived.size()) {
		return false;
	}
	return mIsReceived.testBit(index);
}



void ReceivingBlob::setReceived(int index)
{
	OC_METHODGATE();
	if(index<0) {
		qWarning()<<"ERROR: trying to set received for index < 0: "<<index;
		return;
	}
	if(index>=mIsReceived.size()) {
		qWarning()<<"ERROR: trying to set received for index > size: "<<index<<" vs. "<<mIsReceived.size();
		return;
	}
	mIsReceived.setBit(index);
	mLastReceivedTime=utility::time::currentMsecsSinceEpoch<quint64>();
}

QBitArray ReceivingBlob::isReceived() const
{
	OC_METHODGATE();
	return mIsReceived;
}



// Name //////////////////////////////////////////


bool ReceivingBlob::isNameReceived()
{
	OC_METHODGATE();
	return mNameReceived;
}


void ReceivingBlob::setNameReceived(bool v)
{
	OC_METHODGATE();
	mNameReceived=v;
}


int ReceivingBlob::numReceived() const
{
	OC_METHODGATE();
	return mIsReceived.count(true);
}
