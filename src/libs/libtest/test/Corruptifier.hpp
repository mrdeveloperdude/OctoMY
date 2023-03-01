#ifndef CORRUPTIFIER_HPP
#define CORRUPTIFIER_HPP

#include <QByteArray>
#include <QVector>


enum CorruptionType:quint16 {
	// Do nothing (return input
	None
	, ScrambleBit
	, UnsetBit
	, SetBit
	, ToggleBit

	, ChopLeft
	, ChopRight
	, ChopMid

	, ScrambleByteRange
	, SetByteRange
	, UnsetByteRange
	, SwapByteRange

	// Swap the order of packets
	, ReorderPacket

	// Send some packets more than once
	, DuplicatePacket

	// Skip some packet
	, DropPacket

	// Take out random bits of packets and send them later
	, MixPacketFragment

	// Send some now, some later
	, SplitPacket
};

class Corruptifier
{
private:

	QVector<qreal> mSettings;

public:
	Corruptifier(
		qreal none=1.0,
		qreal scrambleBits=0.0,
		qreal unsetBits=0.0,
		qreal setBits=0.0,
		qreal toggleBits=0.0,
		qreal chopLeft=0.0,
		qreal chopRight=0.0,
		qreal chopMid=0.0,
		qreal scrambleByteRanges=0.0,
		qreal setByteRanges=0.0,
		qreal unsetByteRanges=0.0,
		qreal swapByteRanges=0.0,
		qreal reorderPackets=0.0,
		qreal mixPacketFragments=0.0,
		qreal splitPackets=0.0
	);

public:

	void set(CorruptionType type, qreal val);
	qreal get(CorruptionType type);

	QByteArray corruptify(CorruptionType type, QByteArray in);

	QByteArray corruptify(QByteArray in);



};

QByteArray bitsToBytes(QBitArray &in);

QBitArray bytesToBits(QByteArray &in);

#endif // CORRUPTIFIER_HPP
