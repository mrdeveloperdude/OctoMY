#include "Corruptifier.hpp"

#include "utility/random/Random.hpp"

#include <QBitArray>


Corruptifier::Corruptifier(
	qreal none,
	qreal scrambleBits,
	qreal unsetBits,
	qreal setBits,
	qreal toggleBits,
	qreal chopLeft,
	qreal chopRight,
	qreal chopMid,
	qreal scrambleByteRanges,
	qreal setByteRanges,
	qreal unsetByteRanges,
	qreal swapByteRanges,
	qreal reorderPackets,
	qreal mixPacketFragments,
	qreal splitPackets
)

{
	set(None, none);
	set(ScrambleBit, scrambleBits);
	set(UnsetBit, unsetBits);
	set(SetBit, setBits);
	set(ToggleBit, toggleBits);
	set(ChopLeft, chopLeft);
	set(ChopRight, chopRight);
	set(ChopMid, chopMid);
	set(ScrambleByteRange, scrambleByteRanges);
	set(SetByteRange, setByteRanges);
	set(UnsetByteRange, unsetByteRanges);
	set(SwapByteRange, swapByteRanges);
	set(ReorderPacket, reorderPackets);
	set(MixPacketFragment, mixPacketFragments);
	set(SplitPacket, splitPackets);

}


void Corruptifier::set(CorruptionType type, qreal val)
{
	mSettings[static_cast<quint16>(type)]=qMax(val, static_cast<qreal>(0.0));
}


qreal Corruptifier::get(CorruptionType type)
{
	return mSettings[type];
}


QByteArray bitsToBytes(QBitArray &in)
{
	QByteArray out;
	const int sz=in.data_ptr()->size;
	auto ind=in.data_ptr()->data();
	out.reserve(sz);
	auto outd=out.data_ptr()->data();
	for(int i=0; i<sz; ++i) {
		outd[i]=ind[i];
	}
	return out;
}


QBitArray bytesToBits(QByteArray &in)
{
	QBitArray out;
	const int sz=in.data_ptr()->size;
	auto ind=in.data_ptr()->data();
	out.resize(sz);
	auto outd=out.data_ptr()->data();
	for(int i=0; i<sz; ++i) {
		outd[i]=ind[i];
	}
	return out;
}




QByteArray Corruptifier::corruptify(CorruptionType type, QByteArray bytes)
{
	const auto numBytes=bytes.size();
	const auto numBits=numBytes*8;
	const auto bitIndex=utility::random::qrand()%numBits;
	const auto byteIndex=bitIndex/8;
	const auto byteRemainder=bitIndex%8;
	const quint8 mask=static_cast<quint8>(1<<byteRemainder);
	const bool oldBit=0!=(bytes[byteIndex]&mask);
	bool newBit=oldBit;
	switch(type) {
	case(None):
		break;
	case(ScrambleBit):
		newBit=randomBit();
		break;
	case(UnsetBit):
		newBit=false;
		break;
	case(SetBit):
		newBit=true;
		break;
	case(ToggleBit):
		newBit=!oldBit;
		break;
	case(ChopLeft): {
		const auto left=(utility::random::qrand()%numBytes)-1;
		return bytes.left(left);
	}
	case(ChopRight): {
		const auto right=(utility::random::qrand()%numBytes)-1;
		return bytes.right(right);
	}
	case(ChopMid): {
		const auto num=(utility::random::qrand()%numBytes)-1;
		const auto start=(utility::random::qrand()%numBytes)-1-num/2;
		return bytes.mid(start, num);
	}
	case(ScrambleByteRange): {
		const auto num=(utility::random::qrand()%numBytes)-1;
		const auto start=(utility::random::qrand()%numBytes)-1-num/2;
		for(auto i=0; i<numBytes; ++i) {
			if(i>=start && i< (start+num)) {
				bytes[i]=static_cast<qint8>(utility::random::qrand()%0xFF);
			}
		}
		return bytes;
	}
	case(SetByteRange): {
		const auto num=(utility::random::qrand()%numBytes)-1;
		const auto start=(utility::random::qrand()%numBytes)-1-num/2;
		for(auto i=0; i<numBytes; ++i) {
			if(i>=start && i< (start+num)) {
				bytes[i]=static_cast<qint8>(0xFF);
			}
		}
		return bytes;
	}
	case(UnsetByteRange): {
		const auto num=(utility::random::qrand()%numBytes)-1;
		const auto start=(utility::random::qrand()%numBytes)-1-num/2;
		for(auto i=0; i<numBytes; ++i) {
			if(i>=start && i< (start+num)) {
				bytes[i]=0x00;
			}
		}
		return bytes;
	}
	//TODO: Implement
	case(SwapByteRange):
		break;
	//TODO: Implement
	case(ReorderPacket):
		break;
	//TODO: Implement
	case(DuplicatePacket):
		break;
	//TODO: Implement
	case(DropPacket):
		break;
	//TODO: Implement
	case(MixPacketFragment):
		break;
	//TODO: Implement
	case(SplitPacket):
		break;
	}

	if(oldBit!=newBit) {
		if(oldBit) {
			bytes[byteIndex]=static_cast<qint8>(bytes[byteIndex]&(~mask));
		}
		if(newBit) {
			bytes[byteIndex]=bytes[byteIndex]|mask;
		}
	}
	// Return result
	return bytes;
}



QByteArray Corruptifier::corruptify(QByteArray in)
{
	qreal all=0.0;
	for(qreal v:mSettings) {
		all+=v;
	}
	const qreal r=(all*utility::random::dfrand());
	qreal acc=0.0;
	CorruptionType type=None;
	for(qreal v:mSettings) {
		acc+=v;
		if(acc>r) {
			break;
		}
		type=static_cast<CorruptionType>((static_cast<quint16>(type)+1));
	}
	return corruptify(type,in);
}
