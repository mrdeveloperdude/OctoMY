#include "Blob.hpp"

#include "BlobChunk.hpp"


#include "../libutil/utility/Utility.hpp"

#include <QDateTime>


Blob::Blob(const QString name, const quint16 id, const QByteArray data, const quint32 chunkSize)
	: mName(name)
	, mID(id)
	, mData(data)
	, mChunkSize(chunkSize)
	, mTotalIndecies(  (mData.size()/mChunkSize) + ((mData.size()%mChunkSize)>0?1:0))
{
	qDebug()<<"Created blob with name='"<<mName<<"', id="<<mID<<" dataSize="<<utility::humanReadableSize(mData.size())<<" chunkSize="<<utility::humanReadableSize(mChunkSize)<<", totalIndecies="<<mTotalIndecies;
}


Blob::~Blob()
{
}



QString Blob::name()const
{
	return mName;
}

quint16 Blob::id()const
{
	return mID;
}

QByteArray Blob::data(int index)const
{
	QByteArray out=mData.mid(index*mChunkSize, mChunkSize);
	return out;
}


char *Blob::dataRef(int index)
{
	char *out=mData.data();
	if(nullptr!=out) {
		char *ret=(out+index*mChunkSize);
		return ret;
	}
	return nullptr;
}




QByteArray Blob::data()const
{
	return mData;

}


quint64 Blob::dataSize()const
{
	return mData.size();
}

quint32 Blob::chunkSize() const
{
	return mChunkSize;
}


void Blob::setName(QString name)
{
	//TODO: Scrutinize name
	mName=name;
}

int Blob::numTotal() const
{
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
	, mLastAcknowlegedTime(0)
	, mLastResendTime(0)
	, mIsSent(mTotalIndecies)
	, mIsAcknowleged(mTotalIndecies)
{
	qDebug()<<" This is a sending blob with additional parameters priority="<<mPriority<<", maxInTransit="<<mMaxInTransit;
}



SendingBlob::~SendingBlob()
{
}



bool SendingBlob::isDone()const
{
	return (mIsAcknowleged.count(true)>=mTotalIndecies ) && mNameAcknowleged;
}



qreal SendingBlob::sendProgress()const
{
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
	return mIsSent.count(true);
}

int SendingBlob::numAcknowleged() const
{
	return mIsAcknowleged.count(true);
}




int SendingBlob::firstUnsent() const
{
	int i=0;
	for(const int sz=mIsSent.size(); (i<sz) && mIsSent.testBit(i); ++i);
	return i-1;
}



int SendingBlob::firstUnacknowleged() const
{
	int i=0;
	for(const int sz=mIsAcknowleged.size(); (i<sz) && mIsAcknowleged.testBit(i); ++i);
	return i-1;
}



SendingBlobChunk SendingBlob::findNextSendingChunk()
{
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
			const quint64 now=QDateTime::currentMSecsSinceEpoch();
			const quint64 BLOB_CHUNK_TIMEOUT=10;//TTL
			//Time for resend?
			qint64 diff=(now-BLOB_CHUNK_TIMEOUT)-mLastResendTime;
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
			return SendingBlobChunk(this, nextInTransit);
		}
	}
	// Default: Go away
	return SendingBlobChunk();
}


// Chunk data //////////////////////////////////////////

bool SendingBlob::isSent(int index)const
{
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
	if(index<0) {
		qWarning()<<"ERROR: trying to set send for index < 0: "<<index;
		return;
	}
	if(index>=mIsSent.size()) {
		qWarning()<<"ERROR: trying to set send for index > size: "<<index<<" vs. "<<mIsSent.size();
		return;
	}
	mIsSent.setBit(index);
	mLastResendTime=QDateTime::currentMSecsSinceEpoch();
}


void SendingBlob::setAcknowleged(int index)
{
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
	return mIsSent;
}


QBitArray SendingBlob::isAcknowleged()
{
	return mIsAcknowleged;
}


SendingBlobChunk SendingBlob::chunk(int index)
{
	if(index<0) {
		return SendingBlobChunk();
	}
	if(index>=mTotalIndecies) {
		return SendingBlobChunk();
	}
	return SendingBlobChunk(this,index);
}



// Name //////////////////////////////////////////


bool SendingBlob::isNameSent()
{
	return mNameSent;

}


bool SendingBlob::isNameAcknowleged()
{
	return mNameAcknowleged;

}


void SendingBlob::setNameSent(bool v)
{
	mNameSent=v;
}




void SendingBlob::setNameAcknowleged(bool v)
{
	mNameAcknowleged=v;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



ReceivingBlob::ReceivingBlob(const QString name, const quint16 id, const quint32 chunkSize, const quint32 byteSize)
	: Blob(name, id, QByteArray(byteSize, 0x00), chunkSize)
	, mIsReceived(mTotalIndecies)
	, mLastReceivedTime(0)
	, mNameReceived(false)

{
	qDebug()<<"Created receiving blob with mName='"<<mName<<"' with mChunkSize="<<mChunkSize<< ", mData.size="<<mData.size()<< "("<<byteSize<<")";
}




ReceivingBlob::~ReceivingBlob()
{
}




ReceivingBlobChunk ReceivingBlob::chunk(int index)
{
	if(index<0) {
		return ReceivingBlobChunk();
	}
	if(index>=mTotalIndecies) {
		return ReceivingBlobChunk();
	}
	return ReceivingBlobChunk(this,index);
}



bool ReceivingBlob::isDone()const
{
	return (mIsReceived.count(true)>=mTotalIndecies ) && mNameReceived;
}



bool ReceivingBlob::isReceived(int index)const
{
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
	if(index<0) {
		qWarning()<<"ERROR: trying to set received for index < 0: "<<index;
		return;
	}
	if(index>=mIsReceived.size()) {
		qWarning()<<"ERROR: trying to set received for index > size: "<<index<<" vs. "<<mIsReceived.size();
		return;
	}
	mIsReceived.setBit(index);
	mLastReceivedTime=QDateTime::currentMSecsSinceEpoch();
}

QBitArray ReceivingBlob::isReceived() const
{
	return mIsReceived;
}







// Name //////////////////////////////////////////


bool ReceivingBlob::isNameReceived()
{
	return mNameReceived;

}




void ReceivingBlob::setNameReceived(bool v)
{
	mNameReceived=v;
}




int ReceivingBlob::numReceived() const
{
	return mIsReceived.count(true);
}
