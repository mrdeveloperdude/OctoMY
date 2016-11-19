#include "BlobCourier.hpp"

#include "../libutil/utility/Utility.hpp"

#include <QDateTime>


Blob::Blob(const QString name, const QByteArray data, const quint32 chunkSize, const quint8 priority)
	: mName(name)
	, mData(data)
	, mChunkSize(chunkSize)
	, mPriority(priority)
	, mTotalIndecies(  (mData.size()/mChunkSize) + ((mData.size()%mChunkSize)>0?1:0))
	, mMaxInTransit((mTotalIndecies<=10)?(mTotalIndecies>0?1:0):(mTotalIndecies/10)) //10% and at least 1
	, mUnsentIndex(0)
	, mUnacknowlegedIndex(0)
	, mLastAcknowlegedTime(0)
	, mLastResendTime(0)
	, mIsInTransit(mTotalIndecies)
	, mIsAcknowleged(mTotalIndecies)
{
	qDebug()<<"Created blob with mName='"<<mName<<"' with mChunkSize="<<mChunkSize<<", mPriority="<<mPriority<<", mData.size="<<mData.size()<<", mTotalIndecies="<<mTotalIndecies<<", mMaxInTransit="<<mMaxInTransit<<", mIsInTransit="<<mIsInTransit.size()<<", mIsAcknowleged="<<mIsAcknowleged.size();

}

Blob::~Blob()
{
	qDebug()<<"Deleted blob with mName='"<<mName<<"' with mChunkSize="<<mChunkSize<<", mPriority="<<mPriority<<", mData.size="<<mData.size()<<", mTotalIndecies="<<mTotalIndecies<<", mMaxInTransit="<<mMaxInTransit<<", mIsInTransit="<<mIsInTransit.size()<<", mIsAcknowleged="<<mIsAcknowleged.size();
}

QString Blob::name()const
{
	return mName;
}

QByteArray Blob::data(int index)const
{
	QByteArray out=mData.mid(index*mChunkSize, mChunkSize);
	return out;
}


void Blob::update()
{

}

bool Blob::isDone()const
{
	return mIsAcknowleged.count(true)>=mTotalIndecies;
}

bool Blob::isMyTurn()const
{
	static const qreal thresh=pow(0.5,256.0);
	qreal chance=1.0;
	for(quint8 i=0; i<mPriority; ++i) {
		chance*=utility::frand();
	}
	return chance>thresh;
}

qreal Blob::progress()const
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

int Blob::numInTransit() const
{
	return mIsInTransit.count(true);
}

int Blob::numAcknowleged() const
{
	return mIsAcknowleged.count(true);
}



int Blob::numTotal() const
{
	return mTotalIndecies;
}

int Blob::firstUnsent() const
{
	int i=0;
	for(const int sz=mIsInTransit.size(); (i<sz) && mIsInTransit.testBit(i); ++i);
	return i-1;
}

int Blob::firstUnacknowleged() const
{
	int i=0;
	for(const int sz=mIsAcknowleged.size(); (i<sz) && mIsAcknowleged.testBit(i); ++i);
	return i-1;
}


BlobChunk Blob::chunk(int index)
{
	if(index<0) {
		return BlobChunk();
	}
	if(index>=mTotalIndecies) {
		return BlobChunk();
	}
	return BlobChunk(this,index);
}

BlobChunk Blob::nextWorkItem()
{
	const int inTransit=numInTransit();
	const int acknowleged=numAcknowleged();
	qDebug()<<"NEXT: inTransit="<<inTransit<<", acknowleged"<<acknowleged<<", mTotalIndecies"<<mTotalIndecies<<", mMaxInTransit"<<mMaxInTransit<<", mUnsentIndex"<<mUnsentIndex<<", mUnacknowlegedIndex"<<mUnacknowlegedIndex;
	if(acknowleged<mTotalIndecies) {
		int nextInTransit=-1;
		if((inTransit-acknowleged)<=mMaxInTransit) {
			int last=mUnsentIndex;
			mUnsentIndex++;
			if(mUnsentIndex>=mTotalIndecies) {
				mUnsentIndex=firstUnsent();
			}
			nextInTransit=mUnsentIndex;
			qDebug()<<"SUGGESTING TO SEND "<<nextInTransit<<" last="<<last;
		} else {
			const quint64 now=QDateTime::currentMSecsSinceEpoch();
			const quint64 BLOB_CHUNK_TIMEOUT=100;//TTL
			//Time for resend?
			qint64 diff=(now-BLOB_CHUNK_TIMEOUT)-mLastResendTime;
			if(diff>0) {
				//Resend last
				mUnacknowlegedIndex++;
				if(mUnacknowlegedIndex>=mTotalIndecies) {
					mUnacknowlegedIndex=firstUnacknowleged();
				}
				nextInTransit=mUnacknowlegedIndex;
				qDebug()<<"SUGGESTING TO RESEND "<< nextInTransit;
			} else {
				qDebug()<<"TIME NOT RIGHT FOR RESEND YET "<< diff;
			}
		}
		if(nextInTransit > -1) {
			qDebug()<<"RETURNING WITH "<<nextInTransit;
			return BlobChunk(this, nextInTransit);
		}
	}
	// Default: Go away
	return BlobChunk();
}



bool Blob::isSent(int index)const
{
	if(index<0) {
		return false;
	}
	if(index>=mIsInTransit.size()) {
		return false;
	}
	return mIsInTransit.testBit(index);
}

bool Blob::isAcknowleged(int index)const
{
	if(index<0) {
		return false;
	}
	if(index>=mIsAcknowleged.size()) {
		return false;
	}
	return mIsAcknowleged.testBit(index);
}

void Blob::setSent(int index)
{
	if(index<0) {
		qWarning()<<"ERROR: trying to set send for index < 0: "<<index;
		return;
	}
	if(index>=mIsInTransit.size()) {
		qWarning()<<"ERROR: trying to set send for index > size: "<<index<<" vs. "<<mIsInTransit.size();
		return;
	}
	mIsInTransit.setBit(index);
	mLastResendTime=QDateTime::currentMSecsSinceEpoch();
}

void Blob::setAcknowleged(int index)
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


QBitArray Blob::isInTransit()
{
	return mIsInTransit;
}

QBitArray Blob::isAcknowleged()
{
	return mIsAcknowleged;
}


////////////////////////////////////////////////////////////////////////////




BlobChunk::BlobChunk(Blob *blob, int index)
	: mBlob(blob)
	, mIndex(index)
{

}

BlobChunk::BlobChunk()
	: BlobChunk(nullptr, -1)
{

}

BlobChunk & BlobChunk::operator= ( const BlobChunk & other)
{
	// check for self-assignment
	if(&other == this) {
		return *this;
	}
	mBlob=other.mBlob;
	mIndex=other.mIndex;
	return *this;
}


QString BlobChunk::id()const
{
	if(nullptr==mBlob) {
		return "NULL";
	}
	return mBlob->name()+"."+QString::number(mIndex);
}

QByteArray BlobChunk::data() const
{
	if(nullptr!=mBlob) {
		return mBlob->data(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk";
	}
	return QByteArray();
}


int BlobChunk::index()const
{
	return mIndex;
}

bool BlobChunk::isValid()const
{
	return mIndex>=0;
}


bool BlobChunk::isSent()const
{
	if(nullptr!=mBlob) {
		return mBlob->isSent(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk";
	}
	return false;
}

bool BlobChunk::isAcknowleged()const
{
	return mBlob->isAcknowleged(mIndex);
}


void BlobChunk::setSent()
{
	if(nullptr!=mBlob) {
		mBlob->setSent(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk";
	}
}

void BlobChunk::setAcknowleged()
{
	if(nullptr!=mBlob) {
		mBlob->setAcknowleged(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk";
	}
}


////////////////////////////////////////////////////////////////////////////

const quint16 BlobCourier::BLOB_CHUNK_SIZE=480;

BlobCourier::BlobCourier(QObject *parent)
	: Courier("Blob",(Courier::FIRST_USER_ID+4), parent)
{

}


bool BlobCourier::submitBlob(QString name, QByteArray data, qreal probability)
{
	mBlobs<<QSharedPointer<Blob>(new Blob(name, data, BLOB_CHUNK_SIZE, probability));
	return true;
}


////////////////// Courier interface

CourierMandate BlobCourier::mandate()
{
	const bool hasUnsentBlobs=false;

	return CourierMandate(BLOB_CHUNK_SIZE,1,100, hasUnsentBlobs);
}

quint16 BlobCourier::sendingOpportunity(QDataStream &ds)
{
	return 0;
}


quint16 BlobCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	return 0;
}
