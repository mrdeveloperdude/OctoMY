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

	const QString mName;
	const QByteArray mData;
	const quint32 mChunkSize;
	const quint8 mPriority;

	const int mTotalIndecies;
	const int mMaxInTransit;
	int mUnsentIndex;
	int mUnacknowlegedIndex;
	quint64 mLastAcknowlegedTime;
	quint64 mLastResendTime;
	QBitArray mIsInTransit;
	QBitArray mIsAcknowleged;

public:
	explicit Blob(const QString name, const QByteArray data, const quint32 chunkSize, const quint8 priority);
	virtual ~Blob();

public:

	QString name() const ;
	QByteArray data(int index) const ;
	void update();
	bool isDone() const ;
	bool isMyTurn() const ;
	qreal progress() const ;
	int numInTransit() const;
	int numAcknowleged() const;
	int numTotal() const;

	int firstUnsent() const;
	int firstUnacknowleged() const;

	BlobChunk chunk(int index);

	BlobChunk nextWorkItem();

	bool isSent(int index) const;
	bool isAcknowleged(int index) const;
	void setSent(int index);
	void setAcknowleged(int index);


	QBitArray isInTransit();
	QBitArray isAcknowleged();

};


////////////////////////////////////////////////////////////////////////////

/*!
 * \brief The BlobChunk class represents one piece of a Blob with metadata about
 * when it was attempted sent last, and wether or not it has been confirmed received.
 */

class BlobChunk
{
private:
	Blob *mBlob;
	int mIndex; //NOTE: index is NOT in bytes, but in multiples of Blob::mChunkSize
	//NOTE: index<0 means invalid chunk (similar to null chunk)

public:
	explicit BlobChunk(Blob *blob, int offset);
	explicit BlobChunk();

public:

	BlobChunk & operator= ( const BlobChunk & );

public:

	QString id() const;
	QByteArray data() const;
	int index() const;
	bool isValid() const;
	bool isSent() const;
	bool isAcknowleged() const;
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
