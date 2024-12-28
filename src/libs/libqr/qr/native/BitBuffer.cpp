#include "BitBuffer.hpp"

#include "Segment.hpp"

// Constructor
BitBuffer::BitBuffer() : QBitArray() {
	// Initialize as empty QBitArray
}


bool BitBuffer::fail(const QString &reason){
	mErrorString = reason;
	return (!mErrorString.isNull()) || (!mErrorString.isEmpty());
}


QString BitBuffer::errorString(){
	return mErrorString;
}


// Get the current bit length
int BitBuffer::getBitLength() const {
	return size();
}

// Get the buffer as a vector of bytes
QByteArray BitBuffer::bytes() const {
	QByteArray byteArray((size() + 7) / 8, 0); // Initialize QByteArray with zeroed bytes
	for (int i = 0; i < size(); ++i) {
		if (testBit(i)) {
			byteArray[i / 8] |= (1 << (7 - (i % 8))); // Set the corresponding bit
		}
	}
	return byteArray;
}


// Append bits to the buffer
void BitBuffer::appendBits(uint32_t val, int len) {
	if (len < 0 || len > 32 || (len < 32 && (val >> len) != 0)) {
		fail("Value out of range");
		return;
	}
	
	int currentLength = size();
	resize(currentLength + len);  // Resize the bit array to accommodate new bits
	
	// Append bits one by one, from MSB to LSB
	for (int i = len - 1; i >= 0; --i) {
		bool bit = (val >> i) & 1;
		setBit(currentLength++, bit);
	}
}


void BitBuffer::appendData(const Segment &seg) {
	const auto old{size()};
	const auto add{seg.mBits.size()};
	const auto sz{old + add};
	resize(sz);
	for (int i = 0; i < add; ++i) {
		setBit(i + old, seg.mBits.at(i));
	}
}
