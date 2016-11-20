#include "BlobCourier.hpp"

#include "Blob.hpp"

const quint16 BlobCourier::BLOB_CHUNK_SIZE=480;

BlobCourier::BlobCourier(QObject *parent)
	: Courier("Blob",(Courier::FIRST_USER_ID+4), parent)
{

}


bool BlobCourier::submitBlob(QString name, QByteArray data, qreal probability)
{
	mBlobs<<(new Blob(name, data, BLOB_CHUNK_SIZE, probability));
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
