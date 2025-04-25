#include "Encoder.hpp"

#include <QBitArray>
#include <QImage>
#include <QString>
#include <QVector>
#include <qdebug.h>

#include "BitBuffer.hpp"
#include "Code.hpp"
#include "ErrorCorrection.hpp"
#include "Helpers.hpp"
#include "Segment.hpp"
#include "uptime/New.hpp"

Encoder::Encoder()
	: ecl(&ECC_LOW)
{

}

QBitArray bytesToBits(const QByteArray &bytes){
	QBitArray out(bytes.size() * 8);
	for (int byteIndex = 0; byteIndex < bytes.size(); ++byteIndex) {
		char byte = bytes[byteIndex];
		for (int bitIndex = 0; bitIndex < 8; ++bitIndex) {
			out.setBit(byteIndex * 8 + bitIndex, (byte >> (7 - bitIndex)) & 1);
		}
	}
	return out;
}



bool Encoder::fail(const QString &reason){
	mErrorString = reason;
	return (!mErrorString.isNull()) || (!mErrorString.isEmpty());
}


bool Encoder::begin(){
	if(mHasBegun){
		return fail("begin() called multiple times");
	}
	mHasEnded = false;
	mHasBegun = true;
	mErrorString.clear();
	mSegments.clear();
	ecl = &ECC_LOW;
	return true;
}


bool Encoder::appendSegment(QSharedPointer<Segment> segment){
	mSegments.append(segment);
	return true;
}



bool Encoder::appendBytes(const QByteArray &bytes){
	auto seg = QSharedPointer<Segment>::create(Segment::Mode::BYTE, bytesToBits(bytes));
	return appendSegment(seg);
}


bool Encoder::appendNumeric(const QString &numbers) {
	QBitArray bits;
	
	int bitIndex = 0; // Current position in the QBitArray
	int accumData = 0;
	int accumCount = 0;
	
	// Validate and process the input
	for (int i = 0; i < numbers.length(); ++i) {
		QChar c = numbers.at(i);
		if (!c.isDigit()) {
			return fail("Input contains non-numeric characters");
		}
		accumData = accumData * 10 + (c.unicode() - '0');
		accumCount++;
		
		// Append full chunks of 3 digits as 10 bits
		if (accumCount == 3) {
			for (int bit = 9; bit >= 0; --bit) {
				bits.setBit(bitIndex++, (accumData >> bit) & 1);
			}
			accumData = 0;
			accumCount = 0;
		}
	}
	
	// Append remaining digits (1 or 2 digits)
	if (accumCount > 0) {
		int bitCount = accumCount * 3 + 1;
		for (int bit = bitCount - 1; bit >= 0; --bit) {
			bits.setBit(bitIndex++, (accumData >> bit) & 1);
		}
	}
	
	// Adjust QBitArray size to the actual bit count (just in case)
	bits.resize(bitIndex);
	
	// Create the segment and append it
	auto seg = QSharedPointer<Segment>::create(Segment::Mode::NUMERIC, bits);
	return appendSegment(seg);
}



bool Encoder::appendAlphaNumeric(const QString &alphaNumeric) {
	// Check if the entire input is valid alphanumeric
	if (!isAlphaNumeric(alphaNumeric)) {
		return fail("Input contains unencodable characters in alphanumeric mode");
	}
	QBitArray bits;
	int bitIndex = 0;
	int accumData = 0;
	int accumCount = 0;
	for (QChar c : alphaNumeric) {
		int value = ALPHANUMERIC[c.unicode() - ' '];
		accumData = accumData * 45 + value;
		accumCount++;
		if (accumCount == 2) {
			for (int bit = 10; bit >= 0; --bit) {
				bits.setBit(bitIndex++, (accumData >> bit) & 1);
			}
			accumData = 0;
			accumCount = 0;
		}
	}
	if (accumCount > 0) {
		for (int bit = 5; bit >= 0; --bit) {
			bits.setBit(bitIndex++, (accumData >> bit) & 1);
		}
	}
	bits.resize(bitIndex);
	auto seg = QSharedPointer<Segment>::create(Segment::Mode::ALPHANUMERIC, bits);
	return appendSegment(seg);
}


bool Encoder::setErrorCorrectionLevel(Ecc *level){
	if(nullptr == level){
		return fail("Can't unset ecc level");
	}
	ecl = level;
	return true;
}


bool Encoder::setVersionSpan(int minVersion, int maxVersion){
	if(minVersion<1){
		return fail("Can't set min version below 1");
	}
	if(maxVersion>40){
		return fail("Can't set max version above 40");
	}
	if(minVersion>maxVersion){
		return fail("Can't set min version above max version");
	}
	this->minVersion = minVersion;
	this->maxVersion = maxVersion;
	return true;
}


bool Encoder::setMask(int mask){
	if(mask < -1 || mask > 7){
		return fail("Can't set mask outside range -1 to 7");
	}
	this->mask=mask;
	return true;
}


// Find the minimal version number to use
bool Encoder::calculateVersion(int &version, int &dataUsedBits){
	for (version = minVersion; ; version++) {
		const auto dataCapacityBits = numDataCodewords(version, *ecl) * 8;  // Number of data bits available
		dataUsedBits = totalBits(mSegments, version);
		if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits){
			// This version number is found to be suitable
			break; 
		}
		if (version >= maxVersion){
			// All versions in the range could not fit the given data
			return fail("Data too long");
		}
	}
	if (dataUsedBits == -1){
		return fail("Databits error");
	}
	return true;
}


QSharedPointer<Code> Encoder::end(){
	if(!mHasBegun){
		fail("end() called before begin()");
		return QSharedPointer<Code>();
	}
	if(mHasEnded){
		fail("end() called twice");
		return QSharedPointer<Code>();
	}
	// Reset it such that begin() can be called again
	mHasBegun=false;
	mHasEnded=true;
	
	if(mSegments.size() <=0 ){
		fail("No segments to encode.");
		return QSharedPointer<Code>();
	}
	int version{0};
	int dataUsedBits{0};
	if(!calculateVersion(version, dataUsedBits)){
		return nullptr;
	}
	const Ecc *newEcl = calculateBoostedEcc(version, dataUsedBits, ecl);
	
	// Create the data bit string by concatenating all segments
	const auto dataCapacityBits = numDataCodewords(version, *newEcl) * 8;
	BitBuffer bb;
	for (auto const &seg:mSegments) {
		bb.appendBits(seg->mode.modeBits, 4);
		bb.appendBits(seg->numChars(), seg->mode.numCharCountBits(version));
		bb.appendData(*seg);
	}
	// Add terminator and pad up to a byte if applicable
	bb.appendBits(0, qMin(4, dataCapacityBits - bb.size()));
	bb.appendBits(0, (8 - bb.size() % 8) % 8);
	
	// Pad with alternate bytes until data capacity is reached
	for (quint8 padByte = 0xEC; bb.getBitLength() < dataCapacityBits; padByte ^= 0xEC ^ 0x11){
		bb.appendBits(padByte, 8);
	}
	if (bb.getBitLength() % 8 != 0){
		fail("Bit length error");
		return nullptr;
	}
	return QSharedPointer<Code>( OC_NEW Code(*newEcl, version, mask, bb.bytes()));
}



bool Encoder::appendAutodetect(const QString &data){
	if (data.isEmpty() || data.isNull());
	else if ( isNumeric(data)) {
		return appendNumeric(data);
	} else if ( isAlphaNumeric(data)) {
		return appendAlphaNumeric(data);
	} else {
		return appendBytes(data.toUtf8());
	}
	return false;
}

///////////////////////////////


QString Encoder::errorString(){
	return mErrorString;
}



void Encoder::append(const QString &text) {
	qDebug()<<"Encoder::append("<<text<<")";
}

QSharedPointer<IQrCode> Encoder::generate() {
	qDebug()<<"Encoder::generate()";
	return nullptr;
}
