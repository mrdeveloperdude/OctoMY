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
		qreal none=1.0f,
		qreal scrambleBits=0.0f,
		qreal unsetBits=0.0f,
		qreal setBits=0.0f,
		qreal toggleBits=0.0f,
		qreal chopLeft=0.0f,
		qreal chopRight=0.0f,
		qreal chopMid=0.0f,
		qreal scrambleByteRanges=0.0f,
		qreal setByteRanges=0.0f,
		qreal unsetByteRanges=0.0f,
		qreal swapByteRanges=0.0f,
		qreal reorderPackets=0.0f,
		qreal mixPacketFragments=0.0f,
		qreal splitPackets=0.0f
	);

public:

	void set(CorruptionType type, qreal val);
	qreal get(CorruptionType type);

	QByteArray corruptify(CorruptionType type, QByteArray in);

	QByteArray corruptify(QByteArray in);

};

#endif // CORRUPTIFIER_HPP
