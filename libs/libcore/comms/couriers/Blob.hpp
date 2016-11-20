#ifndef BLOB_HPP
#define BLOB_HPP


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


#endif // BLOB_HPP
