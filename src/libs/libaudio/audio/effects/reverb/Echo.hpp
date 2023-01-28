#ifndef ECHO_H
#define ECHO_H

#include "DelayLine.hpp"

template <typename T>
class Echo
{
public:
	
	Echo()
		: mDensityGain(0)
		, mCoeff(0)
		, mApFeedCoeff(0)
		, mApCoeff(0)
		, mOffset(0)
		, mApOffset(0)
		, mLpCoeff(0)
		, mLpSample(0)
		, mMixCoeff{0,0}
	{
	}
	
public:
	// Attenuation to compensate for the modal density and decay rate of
	// the echo line.
	T   mDensityGain;
	
	// Echo delay and all-pass lines.
	DelayLine<T> mDelay;
	DelayLine<T> mApDelay;
	
	T   mCoeff;
	T   mApFeedCoeff;
	T   mApCoeff;
	
	quint32    mOffset;
	quint32    mApOffset;
	
	// The echo line is 1-pole low-pass filtered.
	T   mLpCoeff;
	T   mLpSample;
	
	// Echo mixing coefficients.
	T   mMixCoeff[2];
} ;


#endif // ECHO_H
