#ifndef EARLYDELAY_HPP
#define EARLYDELAY_HPP

#include "uptime/MethodGate.hpp"
#include "DelayLine.hpp"

template <typename T, int OUTPUT_CHANNELS>
class EarlyDelay
{
public:
	// Output gain for early reflections.
	T mGain;
	
	// Early reflections are done with 4 delay lines.
	T mCoeff[4];
	DelayLine<T> mDelay[4];
	quint32 mOffset[4];
	
	// The gain for each output channel based on 3D panning
	T mPanGain[OUTPUT_CHANNELS]{0};
public:
	
	EarlyDelay()
		: mGain(0)
		, mCoeff{0,0,0,0}
		, mOffset{0,0,0,0}
		, mPanGain{0}
	{
		OC_METHODGATE();
	}
};


#endif // EARLYDELAY_HPP
