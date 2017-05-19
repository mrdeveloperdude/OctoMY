#include "BlobChunk.hpp"


#include "Blob.hpp"

#include <QDebug>

SendingBlobChunk::SendingBlobChunk(SendingBlob *blob, quint32 index)
	: mBlob(blob)
	, mIndex(index)
{

}

SendingBlobChunk::SendingBlobChunk()
	: SendingBlobChunk(nullptr, -1)
{

}

SendingBlobChunk & SendingBlobChunk::operator= ( const SendingBlobChunk & other)
{
	// check for self-assignment
	if(&other == this) {
		return *this;
	}
	mBlob=other.mBlob;
	mIndex=other.mIndex;
	return *this;
}


QString SendingBlobChunk::id()const
{
	if(nullptr==mBlob) {
		return "NULL";
	}
	return mBlob->name()+"."+QString::number(mIndex);
}

QByteArray SendingBlobChunk::data() const
{
	if(nullptr!=mBlob) {
		return mBlob->data(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk 1";
	}
	return QByteArray();
}


quint32 SendingBlobChunk::index()const
{
	return mIndex;
}

bool SendingBlobChunk::isValid()const
{
	return (nullptr!=mBlob) && (mIndex>=0);
}


bool SendingBlobChunk::isSent()const
{
	if(nullptr!=mBlob) {
		return mBlob->isSent(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk 2";
	}
	return false;
}

bool SendingBlobChunk::isAcknowleged()const
{
	return mBlob->isAcknowleged(mIndex);
}


void SendingBlobChunk::setSent()
{
	if(nullptr!=mBlob) {
		mBlob->setSent(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk 3";
	}
}




void SendingBlobChunk::setAcknowleged()
{
	if(nullptr!=mBlob) {
		mBlob->setAcknowleged(mIndex);
	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk 4";
	}
}





//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////





ReceivingBlobChunk::ReceivingBlobChunk(ReceivingBlob *blob, quint32 index)
	: mBlob(blob)
	, mIndex(index)
{

}

ReceivingBlobChunk::ReceivingBlobChunk()
	: ReceivingBlobChunk(nullptr, -1)
{

}



QString ReceivingBlobChunk::id()const
{
	if(nullptr==mBlob) {
		return "NULL";
	}
	return mBlob->name()+"."+QString::number(mIndex);
}


quint32 ReceivingBlobChunk::index()const
{
	return mIndex;
}




bool ReceivingBlobChunk::isValid()const
{
	return (nullptr!=mBlob) && (mIndex>=0);
}


bool ReceivingBlobChunk::setReceived(char *data, quint32 size)
{
	if(nullptr!=mBlob) {
		mBlob->setReceived(mIndex);
		char *it=mBlob->dataRef(mIndex);
		if(nullptr!=it) {
			for(quint32 i=0; i<size; i++) {
				it[i]=data[i];
			}
		} else {
			qWarning()<<"ERROR: data ref was null";
		}
		return true;

	} else {
		qWarning()<<"ERROR: Trying to use invalid chunk 5";
	}
	return false;
}


