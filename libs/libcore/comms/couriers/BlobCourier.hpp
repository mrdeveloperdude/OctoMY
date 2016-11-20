#ifndef BLOBCOURIER_HPP
#define BLOBCOURIER_HPP

#include "Courier.hpp"

#include <QList>
#include <QDataStream>
#include <QByteArray>
#include <QString>

class Blob;

////////////////////////////////////////////////////////////////////////////

/*!
 * \brief The BlobCourier class is a courier that works with commschannel to
 * send and receive Blobs.
 */

class BlobCourier: public Courier
{
	Q_OBJECT
public:
	static const quint16 BLOB_CHUNK_SIZE;

	QList<Blob *> mBlobs;
public:
	explicit BlobCourier(QObject *parent=nullptr);

public:

	bool submitBlob(QString name, QByteArray data, qreal probability=0.5);

	// Courier interface
public:
	//Let the CommChannel know what we want
	CourierMandate mandate() override;

	//Override to act on sending opportunity.
	//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) override;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;
};

#endif // BLOBCOURIER_HPP
