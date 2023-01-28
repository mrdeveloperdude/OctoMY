#ifndef UTILS_H
#define UTILS_H

#include "uptime/MethodGate.hpp"

#include "DelayLine.hpp"

#include <QtGlobal>

#define FLT_EPSILON 1.19209290E-07

#define SPEED_OF_SOUND_IN_METRES_PER_SEC (343.3)

#define GAIN_SILENCE_THRESHOLD  (0.00001)


template <typename T>
inline T lerp(T val1, T val2, T mu)
{
	OC_FUNCTIONGATE();
	return val1 + (val2-val1)*mu;
}

// Find the next power-of-2 for non-power-of-2 numbers.
inline quint32 NextPowerOf2(quint32 value)
{
	OC_FUNCTIONGATE();
	if(value > 0) {
		value--;
		value |= value>>1;
		value |= value>>2;
		value |= value>>4;
		value |= value>>8;
		value |= value>>16;
	}
	return value+1;
}

template <typename T>
inline quint32 Truncate(T f)
{
	OC_FUNCTIONGATE();
	return (quint32)f;
}

template <typename T>
inline T min(T a, T b)
{
	OC_FUNCTIONGATE();
	return ((a > b) ? b : a);
}

template <typename T>
inline T max(T a, T b)
{
	OC_FUNCTIONGATE();
	return ((a > b) ? a : b);
}

template <typename T>
inline T clamp(T val, T mi, T ma)
{
	OC_FUNCTIONGATE();
	return min(ma, max(mi, val));
}


// Given the allocated sample buffer, this function updates each delay line offset.
template<typename T>
static inline void RealizeLineOffset(T *sampleBuffer, DelayLine<T> *Delay)
{
	OC_FUNCTIONGATE();
	Delay->mLine = &sampleBuffer[reinterpret_cast<ptrdiff_t>(Delay->mLine)];
}

// Calculate the length of a delay line and store its mask and offset.
template<typename T>
static quint32 CalcLineLength(T length, ptrdiff_t offset, quint32 frequency, DelayLine<T> *Delay)
{
	OC_FUNCTIONGATE();
	quint32 samples;
	
	// All line lengths are powers of 2, calculated from their lengths, with
	// an additional sample in case of rounding errors.
	samples = NextPowerOf2(Truncate(length * frequency) + 1);
	// All lines share a single sample buffer.
	Delay->mMask = samples - 1;
	Delay->mLine = reinterpret_cast<T* >(offset);
	// Return the sample count for accumulation.
	return samples;
}

// Calculate a decay coefficient given the length of each cycle and the time until the decay reaches -60 dB.
template<typename T>
static inline T CalcDecayCoeff(T length, T decayTime)
{
	OC_FUNCTIONGATE();
	//-60 dB
	return std::pow(0.001, length/decayTime);
}

// Calculate a decay length from a coefficient and the time until the decay reaches -60 dB.
template<typename T>
static inline T CalcDecayLength(T coeff, T decayTime)
{
	OC_FUNCTIONGATE();
	//-60 dB
	return std::log10(coeff) * decayTime / std::log10(0.001);
}

// Calculate an attenuation to be applied to the input of any echo models to
// compensate for modal density and decay time.
template<typename T>
static inline T CalcDensityGain(T a)
{
	OC_FUNCTIONGATE();
	// The energy of a signal can be obtained by finding the area under the
	// squared signal.  This takes the form of Sum(x_n^2), where x is the
	// amplitude for the sample n.
	//
	// Decaying feedback matches exponential decay of the form Sum(a^n),
	// where a is the attenuation coefficient, and n is the sample.  The area
	// under this decay curve can be calculated as:  1 / (1 - a).
	//
	// Modifying the above equation to find the squared area under the curve
	// (for energy) yields:  1 / (1 - a^2).  Input attenuation can then be
	// calculated by inverting the square root of this approximation,
	// yielding:  1 / sqrt(1 / (1 - a^2)), simplified to: sqrt(1 - a^2).
	
	return std::sqrt(1.0 - (a * a));
}

// Calculate the mixing matrix coefficients given a diffusion factor.
template<typename T>
static inline void CalcMatrixCoeffs(T diffusion, T *x, T *y)
{
	OC_FUNCTIONGATE();
	T n, t;
	
	// The matrix is of order 4, so n is sqrt (4 - 1).
	n = std::sqrt(3.0);
	t = diffusion * std::atan(n);
	
	// Calculate the first mixing matrix coefficient.
	*x = std::cos(t);
	// Calculate the second mixing matrix coefficient.
	*y = std::sin(t) / n;
}

// Calculate the limited HF ratio for use with the late reverb low-pass filters.
template<typename T>
static T CalcLimitedHfRatio(T hfRatio, T airAbsorptionGainHF, T decayTime)
{
	OC_FUNCTIONGATE();
	T limitRatio;
	
	// Find the attenuation due to air absorption in dB (converting delay
	// time to meters using the speed of sound).  Then reversing the decay
	// equation, solve for HF ratio.  The delay length is cancelled out of
	// the equation, so it can be calculated once for all lines.
	limitRatio = 1.0 / (CalcDecayLength(airAbsorptionGainHF, decayTime) * SPEED_OF_SOUND_IN_METRES_PER_SEC);
	// Using the limit calculated above, apply the upper bound to the HF
	// ratio. Also need to limit the result to a minimum of 0.1, just like the
	// HF ratio parameter.
	return qBound(0.1, limitRatio, hfRatio);
}

// Calculate the coefficient for a HF (and eventually LF) decay damping filter.
template<typename T>
static inline T CalcDampingCoeff(T hfRatio, T length, T decayTime, T decayCoeff, T cw)
{
	OC_FUNCTIONGATE();
	T coeff, g;
	
	// Eventually this should boost the high frequencies when the ratio
	// exceeds 1.
	coeff = 0.0;
	if (hfRatio < 1.0) {
		// Calculate the low-pass coefficient by dividing the HF decay
		// coefficient by the full decay coefficient.
		g = CalcDecayCoeff(length, decayTime * hfRatio) / decayCoeff;
		
		// Damping is done with a 1-pole filter, so g needs to be squared.
		g *= g;
		if(g < 0.9999) { // 1-epsilon
			// Be careful with gains < 0.001, as that causes the coefficient
			// head towards 1, which will flatten the signal.
			g = max<T>(g, 0.001);
			coeff = (1 - g*cw - std::sqrt(2*g*(1-cw) - g*g*(1 - cw*cw))) /
					(1 - g);
		}
		
		// Very low decay times will produce minimal output, so apply an
		// upper bound to the coefficient.
		coeff = std::min(coeff, 0.98);
	}
	return coeff;
}


template<typename T>
static T eaxDbToAmp(T eaxDb)
{
	OC_FUNCTIONGATE();
	T dB = eaxDb / 2000.0;
	return std::pow(10.0, dB);
}




#endif // UTILS_H
