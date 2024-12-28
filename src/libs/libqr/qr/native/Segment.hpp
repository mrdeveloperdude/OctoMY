#ifndef SEGMENT_HPP
#define SEGMENT_HPP

#include <QBitArray>
#include <QString>

#include "uptime/SharedPointerWrapper.hpp"

class QByteArray;

/**
* @brief The Segment class represents a segment inside a QR code. Each segment
* has it's own encoding parameters, usually for the purpose of saving bits.
*/
class Segment
{
public:
	class Mode final {
	public:
		static const Mode NUMERIC;
		static const Mode ALPHANUMERIC;
		static const Mode BYTE;
		static const Mode KANJI;
		// (Package-private) An unsigned 4-bit integer value (range 0 to 15) representing the mode indicator bits for this mode object.
		const int modeBits;
	private:
		int numBitsCharCount[3];
	private:
		Mode(int mode, int cc0, int cc1, int cc2)
			: modeBits(mode)
		{
			numBitsCharCount[0] = cc0;
			numBitsCharCount[1] = cc1;
			numBitsCharCount[2] = cc2;
		}

	public:
			// (Package-private) Returns the bit width of the segment character count field for this mode object at the given version number.
		int numCharCountBits(int ver) const;
		
		friend QDebug operator<<(QDebug debug, const Segment::Mode &mode);
	};
	
private:
	const Mode mode;
	const QBitArray mBits;
public:
	Segment(Mode mode, const QBitArray &bits);
	
public:
	
	int segmentSize(int version) const;
	
	int numChars() const;
	
	friend class Encoder;
	friend class BitBuffer;
	friend QDebug operator<<(QDebug debug, const Segment::Mode &mode);
};



QDebug operator<<(QDebug debug, const Segment::Mode &mode);

int totalBits(const QVector<QSharedPointer<Segment> > &segs, int version);


const qint8 ALPHANUMERIC[59] = {
	36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1, -1,	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};



inline static bool isAlphaNumeric(const QString &text)
{
	for (QChar c : text) {
		const auto u = c.unicode();
		if (u < ' ' || u > 'Z' || ALPHANUMERIC[u - ' '] == -1) {
			return false;
		}
	}
	return true;
}

inline static bool isNumeric(const QString &text)
{
	for (QChar c : text) {
		if (c.unicode() < '0' || c.unicode() > '9') {
			return false;
		}
	}
	return true;
}



#endif // SEGMENT_HPP
