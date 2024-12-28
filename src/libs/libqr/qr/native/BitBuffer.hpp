#ifndef BITBUFFER_HPP
#define BITBUFFER_HPP

#include <QBitArray>
#include <QString>

class Segment;

class BitBuffer : public QBitArray {
private:
	QString mErrorString;

public:
	BitBuffer();
	
private:
	bool fail(const QString &reason);
	
public:	
	// Get the total number of bits
	int getBitLength() const;
	
	// Get the data as bytes
	QByteArray  bytes() const;
	
	// Append bits to the bit buffer
	void appendBits(uint32_t val, int len);
	
	void appendData(const Segment &seg);
	
	QString errorString();
};

#endif // BITBUFFER_HPP
