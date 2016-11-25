#ifndef BLOBCHUNK_HPP
#define BLOBCHUNK_HPP

#include <QString>

class SendingBlob;
class ReceivingBlob;

/*!
 * \brief The BlobChunk class represents one piece of a Blob with metadata about
 * when it was attempted sent last, and wether or not it has been confirmed received.
 */

class SendingBlobChunk
{
private:
	SendingBlob *mBlob;
	quint32 mIndex; //NOTE: index is NOT in bytes, but in multiples of Blob::mChunkSize
	//NOTE: index<0 means invalid chunk (similar to null chunk)

public:
	explicit SendingBlobChunk(SendingBlob *blob, quint32 offset);
	explicit SendingBlobChunk();

public:

	SendingBlobChunk & operator= ( const SendingBlobChunk & );

public:

	QString id() const;
	QByteArray data() const;
	quint32 index() const;
	bool isValid() const;
	bool isSent() const;
	bool isAcknowleged() const;
	void setSent();
	void setAcknowleged();

};

class ReceivingBlobChunk
{
private:
	ReceivingBlob *mBlob;
	quint32 mIndex; //NOTE: index is NOT in bytes, but in multiples of Blob::mChunkSize
	//NOTE: index<0 means invalid chunk (similar to null chunk)

public:
	explicit ReceivingBlobChunk(ReceivingBlob *blob, quint32 offset);
	explicit ReceivingBlobChunk();

public:

	QString id() const;
	quint32 index() const;

	bool isValid() const;
	bool setReceived(char *data, quint32 size);

};


#endif // BLOBCHUNK_HPP
