#ifndef BLOBCHUNK_HPP
#define BLOBCHUNK_HPP

#include <QString>

class Blob;

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



#endif // BLOBCHUNK_HPP
