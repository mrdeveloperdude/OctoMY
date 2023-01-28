#ifndef DELAYLINE_HPP
#define DELAYLINE_HPP

#include "uptime/MethodGate.hpp"

template <typename T>
class DelayLine
{
public:
	// The delay lines use sample lengths that are powers of 2 to allow the
	// use of bit-masking instead of a modulus for wrapping.
	quint32 mMask;
	T *mLine;
public:
	
	DelayLine()
		: mMask(0)
		, mLine(nullptr)
	{
		OC_METHODGATE();
	}
	
public:
	
	// Basic delay line input/output routines.
	inline T delayLineOut(quint32 offset)
	{
		OC_METHODGATE();
		return mLine[offset & mMask];
	}
	
	inline void delayLineIn(quint32 offset, T in)
	{
		OC_METHODGATE();
		mLine[ offset & mMask] = in;
	}
	
	// Attenuated delay line output routine.
	inline T attenuatedDelayLineOut(quint32 offset, T coeff)
	{
		OC_METHODGATE();
		return coeff * mLine[ offset & mMask ];
	}
	
	// Basic attenuated all-pass input/output routine.
	inline T allpassInOut(quint32 outOffset, quint32 inOffset, T in, T feedCoeff, T coeff)
	{
		OC_METHODGATE();
		T out, feed;
		
		out = delayLineOut(outOffset);
		feed = feedCoeff * in;
		delayLineIn(inOffset, (feedCoeff * (out - feed)) + in);
		
		// The time-based attenuation is only applied to the delay output to
		// keep it from affecting the feed-back path (which is already controlled
		// by the all-pass feed coefficient).
		return (coeff * out) - feed;
	}
	
	
};

#endif // DELAYLINE_HPP
