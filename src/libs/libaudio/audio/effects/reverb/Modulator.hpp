#ifndef MODULATOR_HPP
#define MODULATOR_HPP


#include "DelayLine.hpp"

template <typename T>
class Modulator
{
public:

	Modulator()
		: mIndex(0)
		, mRange(0)
		, mDepth(0)
		, mCoeff(0)
		, mFilter(0)
	{

	}

public:
	// Modulator delay line.
	DelayLine<T> mDelay;

	// The vibrato time is tracked with an index over a modulus-wrapped
	// range (in samples).
	quint32    mIndex;
	quint32    mRange;

	// The depth of frequency change (also in samples) and its filter.
	T   mDepth;
	T   mCoeff;
	T   mFilter;
};


#endif // MODULATOR_HPP
