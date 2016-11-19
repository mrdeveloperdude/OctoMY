#include "BlobCourier.hpp"

#include "../libutil/utility/Utility.hpp"

#include <QDateTime>


Blob::Blob(QString name, QByteArray data, quint8 priority)
	: mName(name)
	, mData(data)
	, mPriority(priority)
	, mTotalIndecies(  (mData.size()/BlobCourier::BLOB_CHUNK_SIZE) + ((mData.size()%BlobCourier::BLOB_CHUNK_SIZE)>0?1:0))
	, mLastAcknowlegedIndex(-1)
{

}

QString Blob::name()
{
	return mName;
}

QByteArray Blob::data(int index)
{
	QByteArray out=mData.mid(index*BlobCourier::BLOB_CHUNK_SIZE,BlobCourier::BLOB_CHUNK_SIZE);
	return out;
}


void Blob::update()
{
	/*
	for(QMap<quint64, QSharedPointer<BlobChunk> >::iterator it=mInTransit.lowerBound(0), e=mInTransit.end(); it!=e; ++it) {
		QSharedPointer<BlobChunk> blob=it.value();
		if(nullptr!=blob) {
			if(!blob->isAcknowleged()) {
				break;
			} else {
				mLastAcknowlegedIndex=blob->index();
				mInTransit.remove(it.key());
			}
		}
	}
	*/
}

bool Blob::isDone()
{
	return mLastAcknowlegedIndex>=mTotalIndecies;
}

bool Blob::isMyTurn()
{
	qreal chance=1.0;
	for(quint8 i=0; i<mPriority; ++i) {
		chance*=utility::frand();
	}
	return chance>0.5;
}

qreal Blob::progress()
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

QSharedPointer<BlobChunk> Blob::nextWorkItem()
{
	/*
	if(mLastAcknowlegedIndex<mTotalIndecies) {
		//Exploit the fact that QMap is sorted on key to extract the eldest chunk in transit
		QMap<quint64, QSharedPointer<BlobChunk> >::iterator eldest=mInTransit.lowerBound(0);
		if(mInTransit.end()!=eldest) {
			const quint64 now=QDateTime::currentMSecsSinceEpoch();
			//Try to keep at most 10% of chunks in transit at all times
			const int maxInTransit=(10*mTotalIndecies/100);
			// We are at our capacity, see if eldest chunk is ready for resending
			if(mInTransit.size()>=maxInTransit) {
				const quint64 BLOB_CHUNK_TIMEOUT=1000*4;// 4 seconds TTL
				// Resend eldest chunk
				QSharedPointer<BlobChunk> blob=eldest.value();
				if(blob->lastSend() + BLOB_CHUNK_TIMEOUT < now) {

					return blob;
				}
			}
			// We are within capacity, fire up another chuck
			else {
				QSharedPointer<BlobChunk> blob=QSharedPointer<BlobChunk>(new BlobChunk(this, eldest.key()+1));
				return blob;
			}
		}
	}
	*/
	// Default: Go away
	return QSharedPointer<BlobChunk>(nullptr);
}



bool Blob::isSent(int index)
{
	return mIsInTransit.testBit(index);
}

bool Blob::isAcknowleged(int index)
{
	return mIsAcknowleged.testBit(index);
}

void Blob::setSent(int index)
{
	mIsInTransit.setBit(index,true);
}

void Blob::setAcknowleged(int index)
{
	mIsAcknowleged.setBit(index,true);
}


////////////////////////////////////////////////////////////////////////////




BlobChunk::BlobChunk(QSharedPointer<Blob> blob, int index)
	: mBlob(blob)
	, mIndex(index)
{

}


QString BlobChunk::id()
{
	return mBlob->name()+"."+QString::number(mIndex);
}

QByteArray BlobChunk::data()
{
	QByteArray out=mBlob->data(mIndex);
	return out;
}


int BlobChunk::index()
{
	return mIndex;
}

bool BlobChunk::isSent()
{
	return mBlob->isSent(mIndex);
}

bool BlobChunk::isAcknowleged()
{
	return mBlob->isAcknowleged(mIndex);
}


void BlobChunk::setSent()
{
	mBlob->setSent(mIndex);
}

void BlobChunk::setAcknowleged()
{
	mBlob->setAcknowleged(mIndex);
}


////////////////////////////////////////////////////////////////////////////

const quint16 BlobCourier::BLOB_CHUNK_SIZE=480;

BlobCourier::BlobCourier(QObject *parent)
	: Courier("Blob",(Courier::FIRST_USER_ID+4), parent)
{

}


bool BlobCourier::submitBlob(QString name, QByteArray data, qreal probability)
{
	mBlobs<<QSharedPointer<Blob>(new Blob(name,data,probability));
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
