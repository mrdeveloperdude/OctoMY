#ifndef BLOB_HPP
#define BLOB_HPP


#include <QSharedPointer>
#include <QBitArray>


/*!
 * \brief The Blob class represents a named stream of bytes (Binary Large OBject)
 * to be transferred via BlobCourier. It can be in either sending or receiveing
 * mode as determined by the bool mSending member variable.
 *
 */

class SendingBlobChunk;
class ReceivingBlobChunk;

class Blob
{
protected:

	QString mName;
	const quint16 mID;
	QByteArray mData;
	const quint32 mChunkSize;
	const int mTotalIndecies;




public:
	explicit Blob(const QString name, const quint16 id, const QByteArray data, const quint32 chunkSize);
	virtual ~Blob();

public:

	QString name() const ;
	quint16 id() const ;
	QByteArray data(int index) const ;
	char *dataRef(int index);
	QByteArray data() const ;
	quint64  dataSize() const ;
	quint32 chunkSize() const;

	void setName(QString name);


	// Chunk data
public:

	int numTotal() const;



};



///////////////////////////////////////////////////////////////////////////////


class SendingBlob:public Blob
{
private:
	const quint8 mPriority;

	const int mMaxInTransit;
	// Name send state
	bool mNameSent;
	bool mNameAcknowleged;

	// Chunk send state
	int mUnsentIndex;
	int mUnacknowlegedIndex;
	quint64 mLastAcknowlegedTime;
	quint64 mLastResendTime;
	QBitArray mIsSent;

	QBitArray mIsAcknowleged;


public:

	explicit SendingBlob(const QString name, const quint16 id, const quint32 chunkSize, const QByteArray data, const quint8 priority);
	virtual ~SendingBlob();

	// Meta
public:
	qreal sendProgress() const ;

	bool isDone() const ;

	// Chunk data
public:

	int numSent() const;
	int numAcknowleged() const;


	int firstUnsent() const;
	int firstUnacknowleged() const;

	SendingBlobChunk findNextSendingChunk();

	void setSent(int index);
	void setAcknowleged(int index);
	bool isSent(int index) const;
	bool isAcknowleged(int index) const;

	QBitArray isSent();
	QBitArray isAcknowleged();

	SendingBlobChunk chunk(int index);


	// Name
public:

	void setNameSent(bool v);
	bool isNameSent();


	void setNameAcknowleged(bool v);
	bool isNameAcknowleged();

};


///////////////////////////////////////////////////////////////////////////////



class ReceivingBlob:public Blob
{
private:

	QBitArray mIsReceived;
	quint64 mLastReceivedTime;

	// Name send state
	bool mNameReceived;

public:

	explicit ReceivingBlob(const QString name, const quint16 id, const quint32 chunkSize, const quint32 byteSize); //Receiving
	virtual ~ReceivingBlob();

	// Chunk data
public:

	int numReceived() const;


	int firstUnreceived() const;




	// Meta
public:
	qreal receiveProgress() const ;

	bool isDone() const ;

	// Chunk data
public:

	ReceivingBlobChunk chunk(int index);


	bool isReceived(int index)const;
	void setReceived(int index);

	QBitArray isReceived()const;



	// Name
public:

	void setNameReceived(bool v);
	bool isNameReceived();

};

#endif // BLOB_HPP
