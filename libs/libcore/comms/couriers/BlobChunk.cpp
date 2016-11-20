#include "BlobChunk.hpp"


#include "Blob.hpp"

#include <QDebug>

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
