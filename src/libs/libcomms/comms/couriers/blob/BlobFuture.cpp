#include "BlobFuture.hpp"
#include "BlobCourier.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include <QObject>

BlobFuture::BlobFuture(QString name="", BlobCourier *courier=nullptr)
: mCourier(courier)
, mName(name)
, mReason("No reason")
, mWin(false)
{
	OC_METHODGATE();
}


void BlobFuture::fail(QString reason)
{
	OC_METHODGATE();
	mReason=reason;
	mWin=false;
}

void BlobFuture::win()
{
	OC_METHODGATE();
	mReason="";
	mWin=true;
}

bool BlobFuture::isWinning() const
{
	OC_METHODGATE();
	return mWin;
}

const QString BlobFuture::reason() const
{
	OC_METHODGATE();
	return mReason;
}


const QString BlobFuture::name() const
{
	OC_METHODGATE();
	return mName;
}


bool BlobFuture::connect(QObject &recepient)
{
	OC_METHODGATE();
	if(nullptr!=mCourier) {
		if(!QObject::connect(mCourier, SIGNAL(blobReceiveComplete(QString)), &recepient, SLOT(onBlobRecevieComplete(QString)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
			return false;
		}
		return true;
	}
	return false;
}
