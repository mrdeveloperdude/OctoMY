#include "BlobFuture.hpp"
#include "BlobCourier.hpp"

#include "../libutil/utility/Standard.hpp"
#include <QObject>

BlobFuture::BlobFuture(QString name="", BlobCourier *courier)
	: mCourier(courier)
	, mName(name)
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

bool BlobFuture::isWinning() const
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


bool BlobFuture::connect(QObject &recepient)
{
	if(nullptr!=mCourier) {
		if(!QObject::connect(mCourier, SIGNAL(blobReceiveComplete(QString)), &recepient, SLOT(onBlobRecevieComplete(QString)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
			return false;
		}
		return true;
	}
	return false;
}
