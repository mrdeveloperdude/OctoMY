#include "BlobFuture.hpp"

BlobFuture::BlobFuture(QString name)
	: mName(name)
	, mReason("No reason")
	, mWin(false)
{

}


void BlobFuture::fail(QString reason)
{
	mReason=reason;
	mWin=false;
}

void BlobFuture::win()
{
	mReason="";
	mWin=true;
}

const bool BlobFuture::isWinning() const
{
	return mWin;
}

const QString BlobFuture::reason() const
{
	return mReason;
}


const QString BlobFuture::name() const
{
	return mName;
}
