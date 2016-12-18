#ifndef REVERB_HPP
#define REVERB_HPP

#include "random/RNG.hpp"
#include <QtGlobal>
#include <stdint.h>

#define BOOL int

#define OUTPUT_CHANNELS 2
#define REVERB_BUFFERSIZE (2048u)
#define F_2PI (6.28318530717958647692f)
#define FLT_EPSILON 1.19209290E-07F

#define  MAX_AMBI_COEFFS 4

#define SPEEDOFSOUNDMETRESPERSEC 343.3f

#define GAIN_SILENCE_THRESHOLD  0.00001f

/* This is a user config option for modifying the overall output of the reverb effect. */
#define ReverbBoost 1.0f

/* Effect parameter ranges and defaults. */
#define EAXREVERB_MIN_DENSITY                 (0.0)
#define EAXREVERB_MAX_DENSITY                 (1.0)
#define EAXREVERB_DEFAULT_DENSITY             (1.0)

#define EAXREVERB_MIN_DIFFUSION               (0.0)
#define EAXREVERB_MAX_DIFFUSION               (1.0)
#define EAXREVERB_DEFAULT_DIFFUSION           (1.0)

#define EAXREVERB_MIN_GAIN                    (0.0)
#define EAXREVERB_MAX_GAIN                    (1.0)
#define EAXREVERB_DEFAULT_GAIN                (0.32)

#define EAXREVERB_MIN_GAINHF                  (0.0)
#define EAXREVERB_MAX_GAINHF                  (1.0)
#define EAXREVERB_DEFAULT_GAINHF              (0.89)

#define EAXREVERB_MIN_GAINLF                  (0.0)
#define EAXREVERB_MAX_GAINLF                  (1.0)
#define EAXREVERB_DEFAULT_GAINLF              (1.0)

#define EAXREVERB_MIN_DECAY_TIME              (0.1)
#define EAXREVERB_MAX_DECAY_TIME              (20.0)
#define EAXREVERB_DEFAULT_DECAY_TIME          (1.49)

#define EAXREVERB_MIN_DECAY_HFRATIO           (0.1)
#define EAXREVERB_MAX_DECAY_HFRATIO           (2.0)
#define EAXREVERB_DEFAULT_DECAY_HFRATIO       (0.83)

#define EAXREVERB_MIN_DECAY_LFRATIO           (0.1)
#define EAXREVERB_MAX_DECAY_LFRATIO           (2.0)
#define EAXREVERB_DEFAULT_DECAY_LFRATIO       (1.0)

#define EAXREVERB_MIN_REFLECTIONS_GAIN        (0.0)
#define EAXREVERB_MAX_REFLECTIONS_GAIN        (3.16)
#define EAXREVERB_DEFAULT_REFLECTIONS_GAIN    (0.05)

#define EAXREVERB_MIN_REFLECTIONS_DELAY       (0.0)
#define EAXREVERB_MAX_REFLECTIONS_DELAY       (0.3)
#define EAXREVERB_DEFAULT_REFLECTIONS_DELAY   (0.007)


#define EAXREVERB_MIN_REFLECTIONS_PAN_XYZ (-1.0)
#define EAXREVERB_MAX_REFLECTIONS_PAN_XYZ (1.0)
#define EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ (0.0)

#define EAXREVERB_MIN_LATE_REVERB_GAIN        (0.0)
#define EAXREVERB_MAX_LATE_REVERB_GAIN        (10.0)
#define EAXREVERB_DEFAULT_LATE_REVERB_GAIN    (1.26)

#define EAXREVERB_MIN_LATE_REVERB_DELAY       (0.0)
#define EAXREVERB_MAX_LATE_REVERB_DELAY       (0.1)
#define EAXREVERB_DEFAULT_LATE_REVERB_DELAY   (0.011)



#define EAXREVERB_MIN_LATE_REVERB_PAN_XYZ (-1.0)
#define EAXREVERB_MAX_LATE_REVERB_PAN_XYZ (1.0)
#define EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ (0.0)

#define EAXREVERB_MIN_ECHO_TIME               (0.075)
#define EAXREVERB_MAX_ECHO_TIME               (0.25)
#define EAXREVERB_DEFAULT_ECHO_TIME           (0.25)

#define EAXREVERB_MIN_ECHO_DEPTH              (0.0)
#define EAXREVERB_MAX_ECHO_DEPTH              (1.0)
#define EAXREVERB_DEFAULT_ECHO_DEPTH          (0.0)

#define EAXREVERB_MIN_MODULATION_TIME         (0.04)
#define EAXREVERB_MAX_MODULATION_TIME         (4.0)
#define EAXREVERB_DEFAULT_MODULATION_TIME     (0.25)

#define EAXREVERB_MIN_MODULATION_DEPTH        (0.0)
#define EAXREVERB_MAX_MODULATION_DEPTH        (1.0)
#define EAXREVERB_DEFAULT_MODULATION_DEPTH    (0.0)

#define EAXREVERB_MIN_AIR_ABSORPTION_GAINHF   (0.892)
#define EAXREVERB_MAX_AIR_ABSORPTION_GAINHF   (1.0)
#define EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF (0.994)

#define EAXREVERB_MIN_HFREFERENCE             (1000.0)
#define EAXREVERB_MAX_HFREFERENCE             (20000.0)
#define EAXREVERB_DEFAULT_HFREFERENCE         (5000.0)

#define EAXREVERB_MIN_LFREFERENCE             (20.0)
#define EAXREVERB_MAX_LFREFERENCE             (1000.0)
#define EAXREVERB_DEFAULT_LFREFERENCE         (250.0)

#define EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR     (0.0)
#define EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR     (10.0)
#define EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR (0.0)

#define EAXREVERB_MIN_DECAY_HFLIMIT           false
#define EAXREVERB_MAX_DECAY_HFLIMIT           true
#define EAXREVERB_DEFAULT_DECAY_HFLIMIT       true



inline qreal lerp(qreal val1, qreal val2, qreal mu)
{
	return val1 + (val2-val1)*mu;
}

/* Find the next power-of-2 for non-power-of-2 numbers. */
inline quint32 NextPowerOf2(quint32 value)
{
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

inline quint32 Truncate(qreal f)
{
	return (quint32)f;
}

inline qreal minf(qreal a, qreal b)
{
	return ((a > b) ? b : a);
}

inline qreal maxf(qreal a, qreal b)
{
	return ((a > b) ? a : b);
}

inline qreal clampf(qreal val, qreal min, qreal max)
{
	return minf(max, maxf(min, val));
}

inline quint32 maxu(quint32 a, quint32 b)
{
	return ((a > b) ? a : b);
}




enum FilterType {
	/** EFX-style low-pass filter, specifying a gain and reference frequency. */
	Filter_HighShelf,
	/** EFX-style high-pass filter, specifying a gain and reference frequency. */
	Filter_LowShelf,
};


class DelayLine
{
public:
	// The delay lines use sample lengths that are powers of 2 to allow the
	// use of bit-masking instead of a modulus for wrapping.
	quint32 mMask;
	qreal *mLine;
public:

	DelayLine()
		: mMask(0)
		, mLine(nullptr)
	{

	}

public:

	// Basic delay line input/output routines.
	inline qreal delayLineOut(quint32 offset)
	{
		return mLine[offset & mMask];
	}

	inline void delayLineIn(quint32 offset, qreal in)
	{
		OC_METHODGATE();
		mLine[ offset & mMask] = in;
	}

	// Attenuated delay line output routine.
	inline qreal attenuatedDelayLineOut(quint32 offset, qreal coeff)
	{
		OC_METHODGATE();
		return coeff * mLine[ offset & mMask ];
	}

	// Basic attenuated all-pass input/output routine.
	inline qreal allpassInOut(quint32 outOffset, quint32 inOffset, qreal in, qreal feedCoeff, qreal coeff)
	{
		OC_METHODGATE();
		qreal out, feed;

		out = delayLineOut(outOffset);
		feed = feedCoeff * in;
		delayLineIn(inOffset, (feedCoeff * (out - feed)) + in);

		// The time-based attenuation is only applied to the delay output to
		// keep it from affecting the feed-back path (which is already controlled
		// by the all-pass feed coefficient).
		return (coeff * out) - feed;
	}


};


class FilterState
{
public:
	qreal x[2]; /* History of two last input samples  */
	qreal y[2]; /* History of two last output samples */
	qreal a[3]; /* Transfer function coefficients "a" */
	qreal b[3]; /* Transfer function coefficients "b" */

public:

	FilterState()
		: x{0,0}
		, y{0,0}
		, a{0,0,0}
		, b{0,0,0}
	{

	}

public:

	void clear()
	{
		x[0] = 0.0f;
		x[1] = 0.0f;
		y[0] = 0.0f;
		y[1] = 0.0f;
	}
public:

	inline qreal process(qreal sample)
	{
		OC_METHODGATE();
		qreal outsmp;

		outsmp = b[0] * sample +
				 b[1] * x[0] +
				 b[2] * x[1] -
				 a[1] * y[0] -
				 a[2] * y[1];
		x[1] = x[0];
		x[0] = sample;
		y[1] = y[0];
		y[0] = outsmp;

		return outsmp;
	}


	void setParams(FilterType type, qreal gain, qreal freq_mult, qreal bandwidth)
	{
		OC_METHODGATE();
		qreal alpha;
		qreal w0;

		// Limit gain to -100dB
		gain = maxf(gain, 0.00001f);

		w0 = F_2PI * freq_mult;

		/* Calculate filter coefficients depending on filter type */
		switch(type) {
		case Filter_HighShelf:
			alpha = sinf(w0)/2.0f*sqrtf((gain + 1.0f/gain)*(1.0f/0.75f - 1.0f) + 2.0f);
			b[0] =       gain*((gain+1.0f) + (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha);
			b[1] = -2.0f*gain*((gain-1.0f) + (gain+1.0f)*cosf(w0)                         );
			b[2] =       gain*((gain+1.0f) + (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha);
			a[0] =             (gain+1.0f) - (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha;
			a[1] =  2.0f*     ((gain-1.0f) - (gain+1.0f)*cosf(w0)                         );
			a[2] =             (gain+1.0f) - (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha;
			break;
		case Filter_LowShelf:
			alpha = sinf(w0)/2.0f*sqrtf((gain + 1.0f/gain)*(1.0f/0.75f - 1.0f) + 2.0f);
			b[0] =       gain*((gain+1.0f) - (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha);
			b[1] =  2.0f*gain*((gain-1.0f) - (gain+1.0f)*cosf(w0)                         );
			b[2] =       gain*((gain+1.0f) - (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha);
			a[0] =             (gain+1.0f) + (gain-1.0f)*cosf(w0) + 2.0f*sqrtf(gain)*alpha;
			a[1] = -2.0f*     ((gain-1.0f) + (gain+1.0f)*cosf(w0)                         );
			a[2] =             (gain+1.0f) + (gain-1.0f)*cosf(w0) - 2.0f*sqrtf(gain)*alpha;
			break;
		}

		b[2] /= a[0];
		b[1] /= a[0];
		b[0] /= a[0];
		a[2] /= a[0];
		a[1] /= a[0];
		a[0] /= a[0];
	}

};



struct ReverbSettings {

public:
	// Shared Reverb Properties
	qreal Density;
	qreal Diffusion;
	qreal Gain;
	qreal GainHF;
	qreal DecayTime;
	qreal DecayHFRatio;
	qreal ReflectionsGain;
	qreal ReflectionsDelay;
	qreal LateReverbGain;
	qreal LateReverbDelay;
	qreal AirAbsorptionGainHF;
	qreal RoomRolloffFactor;
	BOOL DecayHFLimit;

	// Additional EAX Reverb Properties
	qreal GainLF;
	qreal DecayLFRatio;
	qreal ReflectionsPan[3];
	qreal LateReverbPan[3];
	qreal EchoTime;
	qreal EchoDepth;
	qreal ModulationTime;
	qreal ModulationDepth;
	qreal HFReference;
	qreal LFReference;

public:

	ReverbSettings()
		: Density(EAXREVERB_DEFAULT_DENSITY)
		, Diffusion(EAXREVERB_DEFAULT_DIFFUSION)
		, Gain(EAXREVERB_DEFAULT_GAIN)
		, GainHF(EAXREVERB_DEFAULT_GAINHF)
		, DecayTime(EAXREVERB_DEFAULT_DECAY_TIME)
		, DecayHFRatio(EAXREVERB_DEFAULT_DECAY_HFRATIO)
		, ReflectionsGain(EAXREVERB_DEFAULT_REFLECTIONS_GAIN)
		, ReflectionsDelay(EAXREVERB_DEFAULT_REFLECTIONS_DELAY)
		, LateReverbGain(EAXREVERB_DEFAULT_LATE_REVERB_GAIN)
		, LateReverbDelay(EAXREVERB_DEFAULT_LATE_REVERB_DELAY)
		, AirAbsorptionGainHF(EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF)
		, RoomRolloffFactor(EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR)
		, DecayHFLimit(EAXREVERB_DEFAULT_DECAY_HFLIMIT)
		, GainLF(EAXREVERB_DEFAULT_GAINLF)
		, DecayLFRatio(EAXREVERB_DEFAULT_DECAY_LFRATIO)
		, ReflectionsPan{0,0,0}
		, LateReverbPan{0,0,0}
		, EchoTime(EAXREVERB_DEFAULT_ECHO_TIME)
		, EchoDepth(EAXREVERB_DEFAULT_ECHO_DEPTH)
		, ModulationTime(EAXREVERB_DEFAULT_MODULATION_TIME)
		, ModulationDepth(EAXREVERB_DEFAULT_MODULATION_DEPTH)
		, HFReference(EAXREVERB_DEFAULT_HFREFERENCE)
		, LFReference(EAXREVERB_DEFAULT_LFREFERENCE)
	{

	}

	void correct()
	{
		Density=qBound(EAXREVERB_MIN_DENSITY,Density,EAXREVERB_MAX_DENSITY);
		Diffusion=qBound(EAXREVERB_MIN_DIFFUSION,Diffusion,EAXREVERB_MAX_DIFFUSION);
		Gain=qBound(EAXREVERB_MIN_GAIN,Gain,EAXREVERB_MAX_GAIN);
		GainHF=qBound(EAXREVERB_MIN_GAINHF,GainHF,EAXREVERB_MAX_GAINHF);
		GainLF=qBound(EAXREVERB_MIN_GAINLF,GainLF,EAXREVERB_MAX_GAINLF);
		DecayTime=qBound(EAXREVERB_MIN_DECAY_TIME,DecayTime,EAXREVERB_MAX_DECAY_TIME);
		DecayHFRatio=qBound(EAXREVERB_MIN_DECAY_HFRATIO,DecayHFRatio,EAXREVERB_MAX_DECAY_HFRATIO);
		DecayLFRatio=qBound(EAXREVERB_MIN_DECAY_LFRATIO,DecayLFRatio,EAXREVERB_MAX_DECAY_LFRATIO);
		ReflectionsGain=qBound(EAXREVERB_MIN_REFLECTIONS_GAIN,ReflectionsGain,EAXREVERB_MAX_REFLECTIONS_GAIN);
		ReflectionsDelay=qBound(EAXREVERB_MIN_REFLECTIONS_DELAY,ReflectionsDelay,EAXREVERB_MAX_REFLECTIONS_DELAY);

		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=qBound(EAXREVERB_MIN_REFLECTIONS_PAN_XYZ,ReflectionsPan[i],EAXREVERB_MAX_REFLECTIONS_PAN_XYZ);
			LateReverbPan[i]=qBound(EAXREVERB_MIN_LATE_REVERB_PAN_XYZ,LateReverbPan[i],EAXREVERB_MAX_LATE_REVERB_PAN_XYZ);
		}

		LateReverbGain=qBound(EAXREVERB_MIN_LATE_REVERB_GAIN,LateReverbGain,EAXREVERB_MAX_LATE_REVERB_GAIN);
		LateReverbDelay=qBound(EAXREVERB_MIN_LATE_REVERB_DELAY,LateReverbDelay,EAXREVERB_MAX_LATE_REVERB_DELAY);
		EchoTime=qBound(EAXREVERB_MIN_ECHO_TIME,EchoTime,EAXREVERB_MAX_ECHO_TIME);
		EchoDepth=qBound(EAXREVERB_MIN_ECHO_DEPTH,EchoDepth,EAXREVERB_MAX_ECHO_DEPTH);
		ModulationTime=qBound(EAXREVERB_MIN_MODULATION_TIME,ModulationTime,EAXREVERB_MAX_MODULATION_TIME);
		ModulationDepth=qBound(EAXREVERB_MIN_MODULATION_DEPTH,ModulationDepth,EAXREVERB_MAX_MODULATION_DEPTH);
		AirAbsorptionGainHF=qBound(EAXREVERB_MIN_AIR_ABSORPTION_GAINHF,AirAbsorptionGainHF,EAXREVERB_MAX_AIR_ABSORPTION_GAINHF);
		HFReference=qBound(EAXREVERB_MIN_HFREFERENCE,HFReference,EAXREVERB_MAX_HFREFERENCE);
		LFReference=qBound(EAXREVERB_MIN_LFREFERENCE,LFReference,EAXREVERB_MAX_LFREFERENCE);
		RoomRolloffFactor=qBound(EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR,RoomRolloffFactor,EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR);
		//DecayHFLimit=qBound(EAXREVERB_MIN_DECAY_HFLIMIT,DecayHFLimit,EAXREVERB_MAX_DECAY_HFLIMIT);
	}

	void reset()
	{

		Density=EAXREVERB_DEFAULT_DENSITY;
		Diffusion=EAXREVERB_DEFAULT_DIFFUSION;
		Gain=EAXREVERB_DEFAULT_GAIN;
		GainHF=EAXREVERB_DEFAULT_GAINHF;
		GainLF=EAXREVERB_DEFAULT_GAINLF;
		DecayTime=EAXREVERB_DEFAULT_DECAY_TIME;
		DecayHFRatio=EAXREVERB_DEFAULT_DECAY_HFRATIO;
		DecayLFRatio=EAXREVERB_DEFAULT_DECAY_LFRATIO;
		ReflectionsGain=EAXREVERB_DEFAULT_REFLECTIONS_GAIN;
		ReflectionsDelay=EAXREVERB_DEFAULT_REFLECTIONS_DELAY;
		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
			LateReverbPan[i]=EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
		}
		LateReverbGain=EAXREVERB_DEFAULT_LATE_REVERB_GAIN;
		LateReverbDelay=EAXREVERB_DEFAULT_LATE_REVERB_DELAY;
		EchoTime=EAXREVERB_DEFAULT_ECHO_TIME;
		EchoDepth=EAXREVERB_DEFAULT_ECHO_DEPTH;
		ModulationTime=EAXREVERB_DEFAULT_MODULATION_TIME;
		ModulationDepth=EAXREVERB_DEFAULT_MODULATION_DEPTH;
		AirAbsorptionGainHF=EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
		HFReference=EAXREVERB_DEFAULT_HFREFERENCE;
		LFReference=EAXREVERB_DEFAULT_LFREFERENCE;
		RoomRolloffFactor=EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
		DecayHFLimit=EAXREVERB_DEFAULT_DECAY_HFLIMIT;
	}

	qreal frandParam(RNG *rng, qreal min, qreal def, qreal max)
	{
		return def;
	}

	void randomize(RNG *rng)
	{
		Density=frandParam(rng, EAXREVERB_MIN_DENSITY,EAXREVERB_DEFAULT_DENSITY,EAXREVERB_MAX_DENSITY);
		Diffusion=frandParam(rng, EAXREVERB_MIN_DIFFUSION,EAXREVERB_DEFAULT_DIFFUSION,EAXREVERB_MAX_DIFFUSION);
		Gain=frandParam(rng, EAXREVERB_MIN_GAIN,EAXREVERB_DEFAULT_GAIN,EAXREVERB_MAX_GAIN);
		GainHF=frandParam(rng, EAXREVERB_MIN_GAINHF,EAXREVERB_DEFAULT_GAINHF,EAXREVERB_MAX_GAINHF);
		GainLF=frandParam(rng, EAXREVERB_MIN_GAINLF,EAXREVERB_DEFAULT_GAINLF,EAXREVERB_MAX_GAINLF);
		DecayTime=frandParam(rng, EAXREVERB_MIN_DECAY_TIME,EAXREVERB_DEFAULT_DECAY_TIME,EAXREVERB_MAX_DECAY_TIME);
		DecayHFRatio=frandParam(rng, EAXREVERB_MIN_DECAY_HFRATIO,EAXREVERB_DEFAULT_DECAY_HFRATIO,EAXREVERB_MAX_DECAY_HFRATIO);
		DecayLFRatio=frandParam(rng, EAXREVERB_MIN_DECAY_LFRATIO,EAXREVERB_DEFAULT_DECAY_LFRATIO,EAXREVERB_MAX_DECAY_LFRATIO);
		ReflectionsGain=frandParam(rng, EAXREVERB_MIN_REFLECTIONS_GAIN,EAXREVERB_DEFAULT_REFLECTIONS_GAIN,EAXREVERB_MAX_REFLECTIONS_GAIN);
		ReflectionsDelay=frandParam(rng, EAXREVERB_MIN_REFLECTIONS_DELAY,EAXREVERB_DEFAULT_REFLECTIONS_DELAY,EAXREVERB_MAX_REFLECTIONS_DELAY);
		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=frandParam(rng, EAXREVERB_MIN_REFLECTIONS_PAN_XYZ,EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ,EAXREVERB_MAX_REFLECTIONS_PAN_XYZ);
			LateReverbPan[i]=frandParam(rng, EAXREVERB_MIN_LATE_REVERB_PAN_XYZ,EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ,EAXREVERB_MAX_LATE_REVERB_PAN_XYZ);
		}
		LateReverbGain=frandParam(rng, EAXREVERB_MIN_LATE_REVERB_GAIN,EAXREVERB_DEFAULT_LATE_REVERB_GAIN,EAXREVERB_MAX_LATE_REVERB_GAIN);
		LateReverbDelay=frandParam(rng, EAXREVERB_MIN_LATE_REVERB_DELAY,EAXREVERB_DEFAULT_LATE_REVERB_DELAY,EAXREVERB_MAX_LATE_REVERB_DELAY);
		EchoTime=frandParam(rng, EAXREVERB_MIN_ECHO_TIME,EAXREVERB_DEFAULT_ECHO_TIME,EAXREVERB_MAX_ECHO_TIME);
		EchoDepth=frandParam(rng, EAXREVERB_MIN_ECHO_DEPTH,EAXREVERB_DEFAULT_ECHO_DEPTH,EAXREVERB_MAX_ECHO_DEPTH);
		ModulationTime=frandParam(rng, EAXREVERB_MIN_MODULATION_TIME,EAXREVERB_DEFAULT_MODULATION_TIME,EAXREVERB_MAX_MODULATION_TIME);
		ModulationDepth=frandParam(rng, EAXREVERB_MIN_MODULATION_DEPTH,EAXREVERB_DEFAULT_MODULATION_DEPTH,EAXREVERB_MAX_MODULATION_DEPTH);
		AirAbsorptionGainHF=frandParam(rng, EAXREVERB_MIN_AIR_ABSORPTION_GAINHF,EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF,EAXREVERB_MAX_AIR_ABSORPTION_GAINHF);
		HFReference=frandParam(rng, EAXREVERB_MIN_HFREFERENCE,EAXREVERB_DEFAULT_HFREFERENCE,EAXREVERB_MAX_HFREFERENCE);
		LFReference=frandParam(rng, EAXREVERB_MIN_LFREFERENCE,EAXREVERB_DEFAULT_LFREFERENCE,EAXREVERB_MAX_LFREFERENCE);
		RoomRolloffFactor=frandParam(rng, EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR,EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR,EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR);
		DecayHFLimit=rng->generateReal1()>0.5;
	}

	void defaults()
	{

	}

} ;

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
	DelayLine mDelay;

	// The vibrato time is tracked with an index over a modulus-wrapped
	// range (in samples).
	quint32    mIndex;
	quint32    mRange;

	// The depth of frequency change (also in samples) and its filter.
	qreal   mDepth;
	qreal   mCoeff;
	qreal   mFilter;
};


class EarlyDelay
{
public:

	EarlyDelay()
		: mGain(0)
		, mCoeff{0,0,0,0}
		, mOffset{0,0,0,0}
		, mPanGain{0,0}
	{

	}

public:
	// Output gain for early reflections.
	qreal   mGain;

	// Early reflections are done with 4 delay lines.
	qreal   mCoeff[4];
	DelayLine mDelay[4];
	quint32    mOffset[4];

	// The gain for each output channel based on 3D panning (only for the
	// EAX path).
	qreal   mPanGain[OUTPUT_CHANNELS];
};


class LateDelay
{
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

		, mPanGain{0,0}
	{

	}
public:
	// Output gain for late reverb.
	qreal   mGain;

	// Attenuation to compensate for the modal density and decay rate of
	// the late lines.
	qreal   mDensityGain;

	// The feed-back and feed-forward all-pass coefficient.
	qreal   mApFeedCoeff;

	// Mixing matrix coefficient.
	qreal   mMixCoeff;

	// Late reverb has 4 parallel all-pass filters.
	qreal   mApCoeff[4];
	DelayLine mApDelay[4];
	quint32    mApOffset[4];

	// In addition to 4 cyclical delay lines.
	qreal   mCoeff[4];
	DelayLine mDelay[4];
	quint32    mOffset[4];

	// The cyclical delay lines are 1-pole low-pass filtered.
	qreal   mLpCoeff[4];
	qreal   mLpSample[4];

	// The gain for each output channel based on 3D panning (only for the
	// EAX path).
	qreal   mPanGain[OUTPUT_CHANNELS];
};

class ReverbEcho
{
public:

	ReverbEcho()
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
	qreal   mDensityGain;

	// Echo delay and all-pass lines.
	DelayLine mDelay;
	DelayLine mApDelay;

	qreal   mCoeff;
	qreal   mApFeedCoeff;
	qreal   mApCoeff;

	quint32    mOffset;
	quint32    mApOffset;

	// The echo line is 1-pole low-pass filtered.
	qreal   mLpCoeff;
	qreal   mLpSample;

	// Echo mixing coefficients.
	qreal   mMixCoeff[2];
} ;

class ReverbEffect
{
public:



	/* This coefficient is used to define the maximum frequency range controlled
	 * by the modulation depth.  The current value of 0.1 will allow it to swing
	 * from 0.9x to 1.1x.  This value must be below 1.  At 1 it will cause the
	 * sampler to stall on the downswing, and above 1 it will cause it to sample
	 * backwards.
	 */
	static constexpr qreal MODULATION_DEPTH_COEFF = 0.1f;

	/* A filter is used to avoid the terrible distortion caused by changing
	 * modulation time and/or depth.  To be consistent across different sample
	 * rates, the coefficient must be raised to a constant divided by the sample
	 * rate:  coeff^(constant / rate).
	 */
	static constexpr qreal MODULATION_FILTER_COEFF = 0.048f;
	static constexpr qreal MODULATION_FILTER_CONST = 100000.0f;

	// When diffusion is above 0, an all-pass filter is used to take the edge off
	// the echo effect.  It uses the following line length (in seconds).
	static constexpr qreal ECHO_ALLPASS_LENGTH = 0.0133f;

	// Input into the late reverb is decorrelated between four channels.  Their
	// timings are dependent on a fraction and multiplier.  See the
	// UpdateDecorrelator() routine for the calculations involved.
	static constexpr qreal DECO_FRACTION = 0.15f;
	static constexpr qreal DECO_MULTIPLIER = 2.0f;

	// All delay line lengths are specified in seconds.

	// The lengths of the early delay lines.
	static const qreal EARLY_LINE_LENGTH[4];

	// The lengths of the late all-pass delay lines.
	static const qreal ALLPASS_LINE_LENGTH[4] ;
	// The lengths of the late cyclical delay lines.
	static const qreal LATE_LINE_LENGTH[4];

	// The late cyclical delay lines have a variable length dependent on the
	// effect's density parameter (inverted for some reason) and this multiplier.
	static constexpr qreal LATE_LINE_MULTIPLIER = 4.0f;


private:
	ReverbSettings settings;

	// All delay lines are allocated as a single buffer to reduce memory
	// fragmentation and management code.
	qreal  *mSampleBuffer;
	quint32    mTotalSamples;

	// Master effect filters
	FilterState mLpFilter;
	FilterState mHpFilter; // EAX only

	Modulator mModulator;

	// Initial effect delay.
	DelayLine mDelay;
	// The tap points for the initial delay.  First tap goes to early
	// reflections, the last to late reverb.
	quint32    mDelayTap[2];

	EarlyDelay mEarly;

	// Decorrelator delay line.
	DelayLine mDecorrelator;
	// There are actually 4 decorrelator taps, but the first occurs at the
	// initial sample.
	quint32    mDecoTap[3];

	LateDelay mLate;

	ReverbEcho mEcho;

	// The current read offset for all delay lines.
	quint32 mOffset;

public:
	ReverbEffect();
	virtual ~ReverbEffect();

public:
	void init(quint32 frequency);

	void Process(quint32 SamplesToDo, const qreal *SamplesIn, qreal *SamplesOut);
	void Update(int frequency);
	void LoadPreset(int environment, qreal environmentSize, qreal environmentDiffusion, int room, int roomHF, int roomLF, qreal decayTime, qreal decayHFRatio, qreal decayLFRatio, int reflections, qreal reflectionsDelay, qreal reflectionsPanX, qreal reflectionsPanY, qreal reflectionsPanZ, int reverb, qreal reverbDelay, qreal reverbPanX, qreal reverbPanY, qreal reverbPanZ, qreal echoTime, qreal echoDepth, qreal modulationTime, qreal modulationDepth, qreal airAbsorptionHF, qreal hfReference, qreal lfReference, qreal roomRolloffFactor, int flags);
	void LoadPreset(ReverbSettings);

private:

	/* Temporary storage used when processing, before deinterlacing. */
	qreal reverbSamples[REVERB_BUFFERSIZE][4];
	qreal earlySamples[REVERB_BUFFERSIZE][4];

	qreal ambiCoeffs[MAX_AMBI_COEFFS][MAX_AMBI_COEFFS];

	void computeAmbientGains(qreal ingain, qreal gains[OUTPUT_CHANNELS]);
	void computeDirectionalGains(const qreal dir[3], qreal ingain, qreal gains[OUTPUT_CHANNELS]);

	void allocLines(quint32 frequency);

	inline qreal EAXModulation(qreal in);

	inline qreal earlyDelayLineOut(quint32 index);
	inline void earlyReflection(qreal in, qreal *out);

	inline qreal lateAllPassInOut(quint32 index, qreal in);
	inline qreal lateDelayLineOut(quint32 index);

	inline qreal lateLowPassInOut(quint32 index, qreal in);
	inline void lateReverb(const qreal *in, qreal *out);

	inline void EAXEcho(qreal in, qreal *late);
	inline void EAXVerbPass(qreal in, qreal *early, qreal *late);

	void updateDelayLine(qreal earlyDelay, qreal lateDelay, quint32 frequency);
	void updateModulator(qreal modTime, qreal modDepth, quint32 frequency);
	void updateEarlyLines(qreal reverbGain, qreal earlyGain, qreal lateDelay);
	void updateDecorrelator(qreal density, quint32 frequency);
	void updateLateLines(qreal reverbGain, qreal lateGain, qreal xMix, qreal density, qreal decayTime, qreal diffusion, qreal hfRatio, qreal cw, quint32 frequency);
	void updateEchoLine(qreal reverbGain, qreal lateGain, qreal echoTime, qreal decayTime, qreal diffusion, qreal echoDepth, qreal hfRatio, qreal cw, quint32 frequency);

	void update3DPanning(const qreal *ReflectionsPan, const qreal *LateReverbPan, qreal Gain);

};

#endif // REVERB_HPP
