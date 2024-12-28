#include "Segment.hpp"

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QDebug>


const Segment::Mode Segment::Mode::NUMERIC     (0x1, 10, 12, 14);
const Segment::Mode Segment::Mode::ALPHANUMERIC(0x2,  9, 11, 13);
const Segment::Mode Segment::Mode::BYTE        (0x4,  8, 16, 16);
const Segment::Mode Segment::Mode::KANJI       (0x8,  8, 10, 12);


int Segment::Mode::numCharCountBits(int ver) const
{
	if ( 1 <= ver && ver <=  9) {
		return numBitsCharCount[0];
	} else if (10 <= ver && ver <= 26) {
		return numBitsCharCount[1];
	} else if (27 <= ver && ver <= 40) {
		return numBitsCharCount[2];
	} else {
		qWarning() << "Version number out of range";
	}
	return -1;
}

QDebug operator<<(QDebug debug, const Segment::Mode &mode) {
	QDebugStateSaver saver(debug); // Save debug state for formatting consistency
	debug.nospace() << "Segment::Mode("
					<< "modeBits: " << mode.modeBits
					<< ", numBitsCharCount: [";
	
	// Output numBitsCharCount array
	for (int i = 0; i < 3; ++i) {
		debug.nospace() << mode.numBitsCharCount[i];
		if (i < 2) {
			debug.nospace() << ", ";
		}
	}
	
	debug.nospace() << "])";
	return debug;
}


////////////////////////////////////////////////////////////////////////////////


Segment::Segment(Mode mode, const QBitArray &bits)
	: mode(mode)
	, mBits(bits)
{
	qDebug()<<"Creating segment " << mode << bits;
}

int Segment::segmentSize(int version) const{
	int ccbits = mode.numCharCountBits(version);
	// Fail if segment length value doesn't fit in the length field's bit-width
	// TODO: Figure out a way to enable this again
	//if (numChars >= (1 << ccbits)) {
	//	return -1;
	//}
	return 4 + ccbits + mBits.size();	
}


int Segment::numChars() const{
	return (mBits.size() + 7) / 8;
}



int totalBits(const QVector<QSharedPointer<Segment> > &segs, int version)
{
	/*
	if (version < 1 || version > 40) {
		throwError("Version number out of range");
	}
*/
	int result = 0;
	for (auto const &seg:segs) {
		result += seg->segmentSize(version);
	}
	return result;
}
