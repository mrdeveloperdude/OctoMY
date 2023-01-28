#ifndef LATEDELAY_HPP
#define LATEDELAY_HPP

#include "uptime/MethodGate.hpp"
#include "DelayLine.hpp"

template <typename T, int OUTPUT_CHANNELS>
class LateDelay
{
public:
	// Output gain for late reverb.
	T   mGain;

	// Attenuation to compensate for the modal density and decay rate of
	// the late lines.
	T   mDensityGain;

	// The feed-back and feed-forward all-pass coefficient.
	T   mApFeedCoeff;

	// Mixing matrix coefficient.
	T   mMixCoeff;

	// Late reverb has 4 parallel all-pass filters.
	T   mApCoeff[4];
	DelayLine<T> mApDelay[4];
	quint32    mApOffset[4];

	// In addition to 4 cyclical delay lines.
	T   mCoeff[4];
	DelayLine<T> mDelay[4];
	quint32    mOffset[4];

	// The cyclical delay lines are 1-pole low-pass filtered.
	T   mLpCoeff[4];
	T   mLpSample[4];

	// The gain for each output channel based on 3D panning (only for the
	// EAX path).
	T   mPanGain[OUTPUT_CHANNELS]{0};
public:
	LateDelay()
		: mGain(0)
		, mDensityGain(0)
		, mApFeedCoeff(0)
		, mMixCoeff(0)
		, mApCoeff{0,0,0,0}
		, mApOffset{0,0,0,0}

		, mCoeff{0,0,0,0}
		, mOffset{0,0,0,0}

		, mLpCoeff{0,0,0,0}
		, mLpSample{0,0,0,0}

		, mPanGain{0}
	{
		OC_METHODGATE();
	}

};

#endif // LATEDELAY_HPP
