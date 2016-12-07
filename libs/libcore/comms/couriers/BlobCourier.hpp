#ifndef BLOBCOURIER_HPP
#define BLOBCOURIER_HPP

#include "Courier.hpp"
#include "BlobFuture.hpp"

#include <QHash>
#include <QDataStream>
#include <QByteArray>
#include <QString>


class SendingBlob;
class ReceivingBlob;

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

	struct Ack {

	private:
		quint16 mBlobID;
		quint16 mChunkID;

	public:

		// Chunk ack
		Ack(quint16 blobID, quint16 chunkID)
			: mBlobID(blobID)
			, mChunkID(chunkID+1)
		{

		}

		// Name ack/default constructor
		Ack(quint16 blobID=0)
			: mBlobID(blobID)
			, mChunkID(0)
		{

		}

		inline bool isNameAck() const
		{
			return 0==mChunkID;
		}

		inline quint16 blobID() const
		{
			return mBlobID;
		}

		inline quint16 chunkID() const
		{
			return mChunkID-1;
		}


		inline void setBlobID(quint16  v)
		{
			mBlobID=v;
		}

		inline void setChunkID(quint16  v)
		{
			mChunkID=v+1;
		}


		inline quint16 bytes() const
		{
			return sizeof(quint16)+sizeof(quint16);
		}

		friend const QDebug &operator<<(QDebug &d, const BlobCourier::Ack &a);

		friend const QDataStream &operator<<(QDataStream &d, const BlobCourier::Ack &a);
		friend const QDataStream &operator>>(QDataStream &d, BlobCourier::Ack &a);
	};


	// Sending data
	QHash<quint16, SendingBlob *> mSendingBlobsById;
	QHash<QString, SendingBlob *> mSendingBlobsByName;
	quint64 mTotalSendingBlobDataSize;

	// Receiving data
	QHash<quint16, ReceivingBlob *> mReceivingBlobsById;
	QHash<QString, ReceivingBlob *> mReceivingBlobsByName;
	quint64 mTotalReceivingBlobDataSize;

	// Acks
	QVector<Ack *> mPendingAcks;

public:
	explicit BlobCourier(QObject *parent=nullptr);

private:


	// Generate new random unused blob ID
	quint16 findFreeSendingBlobID();
	// Select next blob for sending by priority
	quint16 findNextSendingBlobID();

public:

	BlobFuture submitSendingBlob(QString name, QByteArray data, qreal probability=0.5);


	QByteArray dataForSendingBlob(QString name);
	QByteArray dataForReceivingBlob(QString name);

	void freeSendingBlob(QString name);

	quint64 totalSendingDataSize();
	quint64 totalReceivingDataSize();


public:

	void printSendingSummary(QString title="");




signals:

	void blobReceiveComplete(QString name);

	// Courier interface
public:
	//Let the CommChannel know what we want
	CourierMandate mandate() const Q_DECL_OVERRIDE;

	//Override to act on sending opportunity.
	//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) Q_DECL_OVERRIDE;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) Q_DECL_OVERRIDE;
};

const QDataStream &operator<<(QDataStream &d, const BlobCourier::Ack &a);
const QDataStream &operator>>(QDataStream &d, const BlobCourier::Ack &a);

#endif // BLOBCOURIER_HPP
