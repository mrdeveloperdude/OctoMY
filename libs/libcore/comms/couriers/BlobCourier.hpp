#ifndef BLOBCOURIER_HPP
#define BLOBCOURIER_HPP

#include "Courier.hpp"


#include <QSharedPointer>
#include <QBitArray>


/*!
 * \brief The Blob class represents a named stream of bytes (Binary Large OBject)
 * to be transferred via BlobCourier.
 *
 */

class BlobChunk;

class Blob
{
private:

	QString mName;
	QByteArray mData;
	quint8 mPriority;

	int mTotalIndecies;
	int mLastAcknowlegedIndex;

	QBitArray mIsInTransit;
	QBitArray mIsAcknowleged;

public:
	explicit Blob(QString name, QByteArray data, quint8 priority);

public:

	QString name();
	QByteArray data(int index);
	void update();
	bool isDone();
	bool isMyTurn();
	qreal progress();
	QSharedPointer<BlobChunk> nextWorkItem();


	bool isSent(int index);
	bool isAcknowleged(int index);
	void setSent(int index);
	void setAcknowleged(int index);

};


////////////////////////////////////////////////////////////////////////////

/*!
 * \brief The BlobChunk class represents one piece of a Blob with metadata about
 * when it was attempted sent last, and wether or not it has been confirmed received.
 */

class BlobChunk
{
private:
	QSharedPointer<Blob> mBlob;
	int mIndex; //NOTE: index is NOT in bytes, but in multiples of BlobCouruier::BLOB_CHUNK_SIZE

public:
	explicit BlobChunk(QSharedPointer<Blob> blob, int offset);

public:

	QString id();
	QByteArray data();
	int index();
	bool isSent();
	bool isAcknowleged();
	void setSent();
	void setAcknowleged();

};

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

	QList<QSharedPointer<Blob> > mBlobs;
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
