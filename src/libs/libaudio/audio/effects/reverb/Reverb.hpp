#ifndef REVERB_HPP
#define REVERB_HPP

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "audio/IAudio.hpp"

#include "Settings.hpp"
#include "Utils.hpp"
#include "DelayLine.hpp"
#include "FilterState.hpp"
#include "Echo.hpp"
#include "EarlyDelay.hpp"
#include "LateDelay.hpp"
#include "Modulator.hpp"



#include <QtMath>
#include <algorithm>
#include <QtGlobal>
#include <stdint.h>
#include <cmath>



// This is a user config option for modifying the overall output of the reverb effect.
#define ReverbBoost (1.0)


namespace audio{
namespace effects{



template <typename T, int OUTPUT_CHANNELS = 1, int REVERB_BUFFERSIZE = 2048u, int MAX_AMBI_COEFFS = 4>
class Reverb: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
public:
	// This coefficient is used to define the maximum frequency range controlled
	// by the modulation depth.  The current value of 0.1 will allow it to swing
	// from 0.9x to 1.1x.  This value must be below 1.  At 1 it will cause the
	// sampler to stall on the downswing, and above 1 it will cause it to sample
	// backwards.
	static constexpr T MODULATION_DEPTH_COEFF = 0.1;
	
	// A filter is used to avoid the terrible distortion caused by changing
	// modulation time and/or depth.  To be consistent across different sample
	// rates, the coefficient must be raised to a constant divided by the sample
	// rate:  coeff^(constant / rate).
	
	static constexpr T MODULATION_FILTER_COEFF = 0.048;
	static constexpr T MODULATION_FILTER_CONST = 100000.0;
	
	// When diffusion is above 0, an all-pass filter is used to take the edge off
	// the echo effect.  It uses the following line length (in seconds).
	static constexpr T ECHO_ALLPASS_LENGTH = 0.0133;
	
	// Input into the late reverb is decorrelated between four channels.  Their
	// timings are dependent on a fraction and multiplier.  See the
	// UpdateDecorrelator() routine for the calculations involved.
	static constexpr T DECO_FRACTION = 0.15;
	static constexpr T DECO_MULTIPLIER = 2.0;
	
	// All delay line lengths are specified in seconds.
	
	// The lengths of the early delay lines.
	static constexpr T EARLY_LINE_LENGTH[4]{
		0.0015, 0.0045, 0.0135, 0.0405
	};
	
	// The lengths of the late all-pass delay lines.
	static constexpr T ALLPASS_LINE_LENGTH[4]{
		0.0151, 0.0167, 0.0183, 0.0200,
	};
	// The lengths of the late cyclical delay lines.
	static constexpr T LATE_LINE_LENGTH[4]{
		0.0211, 0.0311, 0.0461, 0.0680
	};
	
	// The late cyclical delay lines have a variable length dependent on the effect's density parameter (inverted for some reason) and this multiplier.
	static constexpr T LATE_LINE_MULTIPLIER = 4.0;
	
	
private:
	reverb::Settings<T> settings;
	
	// All delay lines are allocated as a single buffer to reduce memory fragmentation and management code.
	T  *mSampleBuffer;
	quint32    mTotalSamples;
	
	// Master effect filters
	FilterState mLpFilter;
	FilterState mHpFilter; // EAX only
	
	Modulator<T> mModulator;
	
	// Initial effect delay.
	DelayLine<T> mDelay;
	// The tap points for the initial delay.  First tap goes to early reflections, the last to late reverb.
	quint32    mDelayTap[2];
	
	EarlyDelay<T, OUTPUT_CHANNELS> mEarly;
	
	// Decorrelator delay line.
	DelayLine<T> mDecorrelator;
	// There are actually 4 decorrelator taps, but the first occurs at the initial sample.
	quint32    mDecoTap[3];
	
	LateDelay<T, OUTPUT_CHANNELS> mLate;
	
	Echo<T> mEcho;
	
	// The current read offset for all delay lines.
	quint32 mOffset;
	
public:
	Reverb();
	virtual ~Reverb();
	
	// IAudioProcessor interface
public:
	void init(QAudioFormat af) override;
	void process(qint64 num, T *in, T *out)override;
	
public:
	void update(QAudioFormat af);
	void loadPreset(reverb::Settings<T> &);
	
private:
	// Temporary storage used when processing, before deinterlacing.
	T reverbSamples[REVERB_BUFFERSIZE][4];
	T earlySamples[REVERB_BUFFERSIZE][4];
	
	T ambiCoeffs[MAX_AMBI_COEFFS][MAX_AMBI_COEFFS];
	
	void computeAmbientGains(T ingain, T gains[OUTPUT_CHANNELS]);
	void computeDirectionalGains(const T dir[3], T ingain, T gains[OUTPUT_CHANNELS]);
	
	void allocLines(quint32 frequency);
	
	inline T EAXModulation(T in);
	
	inline T earlyDelayLineOut(int index);
	inline void earlyReflection(T in, T *out);
	
	inline T lateAllPassInOut(quint32 index, T in);
	inline T lateDelayLineOut(quint32 index);
	
	inline T lateLowPassInOut(quint32 index, T in);
	inline void lateReverb(const T *in, T *out);
	
	inline void EAXEcho(T in, T *late);
	inline void EAXVerbPass(T in, T *early, T *late);
	
	void updateDelayLine(T earlyDelay, T lateDelay, quint32 frequency);
	void updateModulator(T modTime, T modDepth, quint32 frequency);
	void updateEarlyLines(T reverbGain, T earlyGain, T lateDelay);
	void updateDecorrelator(T density, quint32 frequency);
	void updateLateLines(T reverbGain, T lateGain, T xMix, T density, T decayTime, T diffusion, T hfRatio, T cw, quint32 frequency);
	void updateEchoLine(T reverbGain, T lateGain, T echoTime, T decayTime, T diffusion, T echoDepth, T hfRatio, T cw, quint32 frequency);
	
	void update3DPanning(const T *ReflectionsPan, const T *LateReverbPan, T Gain);
	
};



///////////////////////////////////////////////////////////////////////////




template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::Reverb()
	: mSampleBuffer(nullptr)
	, mTotalSamples(0)
	, mDelayTap{0, 0}
	, mDecoTap{0,0,0}
	, mOffset(0)
{
	OC_METHODGATE();
	
}


template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::~Reverb()
{
	OC_METHODGATE();
	delete[] mSampleBuffer;
	mSampleBuffer =nullptr;
}




template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::computeAmbientGains(T ingain, T gains[OUTPUT_CHANNELS])
{
	OC_METHODGATE();
	quint32 i;
	
	for(i = 0; i < OUTPUT_CHANNELS; i++) {
		// The W coefficients are based on a mathematical average of the
		// output, scaled by sqrt(2) to compensate for FuMa-style Ambisonics
		// scaling the W channel input by sqrt(0.5). The square root of the
		// base average provides for a more perceptual average volume, better
		// suited to non-directional gains.
		gains[i] = std::sqrt(ambiCoeffs[i][0]/1.4142) * ingain;
	}
}

template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::computeDirectionalGains(const T dir[3], T ingain, T gains[OUTPUT_CHANNELS])
{
	OC_METHODGATE();
	T coeffs[MAX_AMBI_COEFFS];
	quint32 i, j;
	// Convert from OpenAL coords to Ambisonics.
	T x = -dir[2];
	T y = -dir[0];
	T z =  dir[1];
	
	// Zeroth-order
	coeffs[0] = 0.7071; // W = sqrt(1.0 / 2.0)
	// First-order
	coeffs[1] = y; // Y = Y
	coeffs[2] = z; // Z = Z
	coeffs[3] = x; // X = X
	
	for(i = 0; i < OUTPUT_CHANNELS; i++) {
		T gain = 0.0;
		for(j = 0; j < MAX_AMBI_COEFFS; j++) {
			gain += ambiCoeffs[i][j]*coeffs[j];
		}
		gains[i] = gain * ingain;
	}
}


// Given an input sample, this function produces modulation for the late reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
T Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::EAXModulation(T in)
{
	OC_METHODGATE();
	T sinus, frac;
	quint32 offset;
	T out0, out1;
	
	// Calculate the sinus rythm (dependent on modulation time and the
	// sampling rate).  The center of the sinus is moved to reduce the delay
	// of the effect when the time or depth are low.
	sinus = 1.0 - std::cos(M_2_PI* mModulator.mIndex / mModulator.mRange);
	
	// The depth determines the range over which to read the input samples
	// from, so it must be filtered to reduce the distortion caused by even
	// small parameter changes.
	mModulator.mFilter = lerp(mModulator.mFilter, mModulator.mDepth, mModulator.mCoeff);
	
	// Calculate the read offset and fraction between it and the next sample.
	frac   = (1.0 + (mModulator.mFilter * sinus));
	offset = Truncate(frac);
	frac  -= offset;
	
	// Get the two samples crossed by the offset, and feed the delay line
	// with the next input sample.
	out0 = mModulator.mDelay.delayLineOut(mOffset - offset);
	out1 = mModulator.mDelay.delayLineOut(mOffset - offset - 1);
	mModulator.mDelay.delayLineIn(mOffset, in);
	
	// Step the modulation index forward, keeping it bound to its range.
	mModulator.mIndex = (mModulator.mIndex + 1) % mModulator.mRange;
	
	// The output is obtained by linearly interpolating the two samples that
	// were acquired above.
	return lerp(out0, out1, frac);
}

// Delay line output routine for early reflections.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
T Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::earlyDelayLineOut(int index)
{
	OC_METHODGATE();
	return mEarly.mDelay[index].attenuatedDelayLineOut(mOffset - mEarly.mOffset[index], mEarly.mCoeff[index]);
}

// Given an input sample, this function produces four-channel output for the  early reflections.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::earlyReflection(T in, T *out)
{
	OC_METHODGATE();
	T d[4], v, f[4];
	
	// Obtain the decayed results of each early delay line.
	for(int i=0; i<4; ++i) {
		d[i] = earlyDelayLineOut(i);
	}
	
	// The following uses a lossless scattering junction from waveguide
	// theory.  It actually amounts to a householder mixing matrix, which
	// will produce a maximally diffuse response, and means this can probably
	// be considered a simple feed-back delay network (FDN).
	//          N
	//         ---
	//         |
	// v = 2/N /   d_i
	//         ---
	//         i=1
	
	v = (d[0] + d[1] + d[2] + d[3]) * 0.5;
	// The junction is loaded with the input here.
	v += in;
	
	// Calculate the feed values for the delay lines.
	for(int i=0; i<4; ++i) {
		f[i] = v - d[i];
	}
	
	// Re-feed the delay lines.
	for(int i=0; i<4; ++i) {
		mEarly.mDelay[i].delayLineIn( mOffset, f[i]);
	}
	
	
	// Output the results of the junction for all four channels.
	for(int i=0; i<4; ++i) {
		out[i] = mEarly.mGain * f[i];
	}
}


// All-pass input/output routine for late reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
T Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::lateAllPassInOut(quint32 index, T in)
{
	OC_METHODGATE();
	return mLate.mApDelay[index].allpassInOut(mOffset - mLate.mApOffset[index], mOffset, in, mLate.mApFeedCoeff, mLate.mApCoeff[index]);
}

// Delay line output routine for late reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
T Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::lateDelayLineOut(quint32 index)
{
	OC_METHODGATE();
	return mLate.mDelay[index].attenuatedDelayLineOut(mOffset - mLate.mOffset[index], mLate.mCoeff[index]);
}

// Low-pass filter input/output routine for late reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
T Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::lateLowPassInOut(quint32 index, T in)
{
	OC_METHODGATE();
	in = lerp(in, mLate.mLpSample[index], mLate.mLpCoeff[index]);
	mLate.mLpSample[index] = in;
	return in;
}

// Given four decorrelated input samples, this function produces four-channel output for the late reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::lateReverb(const T *in, T *out)
{
	OC_METHODGATE();
	T d[4], f[4];
	
	// Obtain the decayed results of the cyclical delay lines, and add the
	// corresponding input channels.  Then pass the results through the
	// low-pass filters.
	
	// This is where the feed-back cycles from line 0 to 1 to 3 to 2 and back
	// to 0.
	d[0] = lateLowPassInOut(2, in[2] + lateDelayLineOut(2));
	d[1] = lateLowPassInOut(0, in[0] + lateDelayLineOut(0));
	d[2] = lateLowPassInOut(3, in[3] + lateDelayLineOut(3));
	d[3] = lateLowPassInOut(1, in[1] + lateDelayLineOut(1));
	
	// To help increase diffusion, run each line through an all-pass filter.
	// When there is no diffusion, the shortest all-pass filter will feed the
	// shortest delay line.
	for(quint32 i=0; i<4; ++i) {
		d[i] = lateAllPassInOut(i, d[i]);
	}
	
	
	// Late reverb is done with a modified feed-back delay network (FDN)
	// topology.  Four input lines are each fed through their own all-pass
	// filter and then into the mixing matrix.  The four outputs of the
	// mixing matrix are then cycled back to the inputs.  Each output feeds
	// a different input to form a circlular feed cycle.
	//
	// The mixing matrix used is a 4D skew-symmetric rotation matrix derived
	// using a single unitary rotational parameter:
	//
	//  [  d,  a,  b,  c ]          1 = a^2 + b^2 + c^2 + d^2
	//  [ -a,  d,  c, -b ]
	//  [ -b, -c,  d,  a ]
	//  [ -c,  b, -a,  d ]
	//
	// The rotation is constructed from the effect's diffusion parameter,
	// yielding:  1 = x^2 + 3 y^2; where a, b, and c are the coefficient y
	// with differing signs, and d is the coefficient x.  The matrix is thus:
	//
	//  [  x,  y, -y,  y ]          n = sqrt(matrix_order - 1)
	//  [ -y,  x,  y,  y ]          t = diffusion_parameter * atan(n)
	//  [  y, -y,  x,  y ]          x = cos(t)
	//  [ -y, -y, -y,  x ]          y = sin(t) / n
	//
	// To reduce the number of multiplies, the x coefficient is applied with
	// the cyclical delay line coefficients.  Thus only the y coefficient is
	// applied when mixing, and is modified to be:  y / x.
	
	f[0] = d[0] + (mLate.mMixCoeff * (         d[1] + -d[2] + d[3]));
	f[1] = d[1] + (mLate.mMixCoeff * (-d[0]         +  d[2] + d[3]));
	f[2] = d[2] + (mLate.mMixCoeff * ( d[0] + -d[1]         + d[3]));
	f[3] = d[3] + (mLate.mMixCoeff * (-d[0] + -d[1] + -d[2]       ));
	
	// Output the results of the matrix for all four channels, attenuated by
	// the late reverb gain (which is attenuated by the 'x' mix coefficient).
	for(int i=0; i<4; ++i) {
		out[i] = mLate.mGain * f[i];
	}
	
	// Re-feed the cyclical delay lines.
	for(int i=0; i<4; ++i) {
		mLate.mDelay[i].delayLineIn(mOffset, f[i]);
	}
	
}

// Given an input sample, this function mixes echo into the four-channel late
// reverb.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::EAXEcho(T in, T *late)
{
	OC_METHODGATE();
	T out, feed;
	
	// Get the latest attenuated echo sample for output.
	feed = mEcho.mDelay.attenuatedDelayLineOut( mOffset - mEcho.mOffset, mEcho.mCoeff);
	
	// Mix the output into the late reverb channels.
	out = mEcho.mMixCoeff[0] * feed;
	late[0] = (mEcho.mMixCoeff[1] * late[0]) + out;
	late[1] = (mEcho.mMixCoeff[1] * late[1]) + out;
	late[2] = (mEcho.mMixCoeff[1] * late[2]) + out;
	late[3] = (mEcho.mMixCoeff[1] * late[3]) + out;
	
	// Mix the energy-attenuated input with the output and pass it through
	// the echo low-pass filter.
	feed += mEcho.mDensityGain * in;
	feed = lerp(feed, mEcho.mLpSample, mEcho.mLpCoeff);
	mEcho.mLpSample = feed;
	
	// Then the echo all-pass filter.
	feed = mEcho.mApDelay.allpassInOut(mOffset - mEcho.mApOffset, mOffset, feed, mEcho.mApFeedCoeff, mEcho.mApCoeff);
	
	// Feed the delay with the mixed and filtered sample.
	mEcho.mDelay.delayLineIn(mOffset, feed);
}


// Perform the EAX reverb pass on a given input sample, resulting in four-channel output.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::EAXVerbPass(T in, T *early, T *late)
{
	OC_METHODGATE();
	T feed, taps[4];
	
	// Low-pass filter the incoming sample.
	in = mLpFilter.process(in);
	in = mHpFilter.process(in);
	
	// Perform any modulation on the input.
	in = EAXModulation(in);
	
	// Feed the initial delay line.
	mDelay.delayLineIn(mOffset, in);
	
	// Calculate the early reflection from the first delay tap.
	in = mDelay.delayLineOut(mOffset - mDelayTap[0]);
	earlyReflection(in, early);
	
	// Feed the decorrelator from the energy-attenuated output of the second
	// delay tap.
	in = mDelay.delayLineOut(mOffset - mDelayTap[1]);
	feed = in * mLate.mDensityGain;
	mDecorrelator.delayLineIn(mOffset, feed);
	
	// Calculate the late reverb from the decorrelator taps.
	taps[0] = feed;
	taps[1] = mDecorrelator.delayLineOut(mOffset - mDecoTap[0]);
	taps[2] = mDecorrelator.delayLineOut(mOffset - mDecoTap[1]);
	taps[3] = mDecorrelator.delayLineOut(mOffset - mDecoTap[2]);
	lateReverb(taps, late);
	
	// Calculate and mix in any echo.
	EAXEcho(in, late);
	
	// Step all delays forward one sample.
	mOffset++;
}


template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::process(qint64 num, T *in, T *out)
{
	OC_METHODGATE();
	T (*early)[4] = earlySamples;
	T (*late)[4] = reverbSamples;
	quint32 index, c;
	
	// Process reverb for these samples.
	for(index = 0; index < num; index++) {
		EAXVerbPass(in[index], early[index], late[index]);
	}
	
	for(c = 0; c < OUTPUT_CHANNELS ; c++) {
		T earlyGain, lateGain;
		
		earlyGain = mEarly.mPanGain[c];
		if(std::abs(earlyGain) > GAIN_SILENCE_THRESHOLD) {
			for(index = 0; index < num; index++) {
				out[index * OUTPUT_CHANNELS + c] = earlyGain*early[index][c&3];
			}
		}
		
		
		lateGain = mLate.mPanGain[c];
		if(std::abs(lateGain) > GAIN_SILENCE_THRESHOLD) {
			for(index = 0; index < num; index++) {
				out[index * OUTPUT_CHANNELS + c] = lateGain*late[index][c&3];
			}
		}
	}
}


// Calculates the delay line metrics and allocates the shared sample buffer for all lines given the sample rate (frequency).
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::allocLines(quint32 frequency)
{
	OC_METHODGATE();
	quint32 totalSamples, index;
	T length;
	
	// All delay line lengths are calculated to accomodate the full range of lengths given their respective paramters.
	totalSamples = 0;
	
	// The modulator's line length is calculated from the maximum modulation
	// time and depth coefficient, and halfed for the low-to-high frequency
	// swing.  An additional sample is added to keep it stable when there is no modulation.
	
	length = (REVERB_MAX_MODULATION_TIME*MODULATION_DEPTH_COEFF/2.0) + (1.0 / frequency);
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mModulator.mDelay);
	
	// The initial delay is the sum of the reflections and late reverb delays.
	length = REVERB_MAX_REFLECTIONS_DELAY + REVERB_MAX_LATE_REVERB_DELAY;
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mDelay);
	
	// The early reflection lines.
	for(index = 0; index < 4; index++) {
		totalSamples += CalcLineLength(EARLY_LINE_LENGTH[index], totalSamples, frequency, &mEarly.mDelay[index]);
	}
	
	// The decorrelator line is calculated from the lowest reverb density (a
	// parameter value of 1).
	length = (DECO_FRACTION * DECO_MULTIPLIER * DECO_MULTIPLIER) * LATE_LINE_LENGTH[0] * (1.0 + LATE_LINE_MULTIPLIER);
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mDecorrelator);
	
	// The late all-pass lines.
	for(index = 0; index < 4; index++) {
		totalSamples += CalcLineLength(Reverb::ALLPASS_LINE_LENGTH[index], totalSamples, frequency, &mLate.mApDelay[index]);
	}
	
	// The late delay lines are calculated from the lowest reverb density.
	for(index = 0; index < 4; index++) {
		length = LATE_LINE_LENGTH[index] * (1.0 + LATE_LINE_MULTIPLIER);
		totalSamples += CalcLineLength(length, totalSamples, frequency, &mLate.mDelay[index]);
	}
	
	// The echo all-pass and delay lines.
	totalSamples += CalcLineLength(ECHO_ALLPASS_LENGTH, totalSamples, frequency, &mEcho.mApDelay);
	totalSamples += CalcLineLength(REVERB_MAX_ECHO_TIME, totalSamples, frequency, &mEcho.mDelay);
	
	T* newBuf = OC_NEW T[totalSamples];
	mSampleBuffer = newBuf;
	mTotalSamples = totalSamples;
	
	// Update all delays to reflect the new sample buffer.
	RealizeLineOffset(mSampleBuffer, &mDelay);
	RealizeLineOffset(mSampleBuffer, &mDecorrelator);
	for(index = 0; index < 4; index++) {
		RealizeLineOffset(mSampleBuffer, &mEarly.mDelay[index]);
		RealizeLineOffset(mSampleBuffer, &mLate.mApDelay[index]);
		RealizeLineOffset(mSampleBuffer, &mLate.mDelay[index]);
	}
	RealizeLineOffset(mSampleBuffer, &mModulator.mDelay);
	RealizeLineOffset(mSampleBuffer, &mEcho.mApDelay);
	RealizeLineOffset(mSampleBuffer, &mEcho.mDelay);
	
	// Clear the sample buffer.
	for(index = 0; index < mTotalSamples; index++) {
		mSampleBuffer[index] = 0.0;
	}
}


// Update the EAX modulation index, range, and depth.
// Keep in mind that this kind of vibrato is additive and not multiplicative as one may expect.
// The downswing will sound stronger than the upswing.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateModulator(T modTime, T modDepth, quint32 frequency)
{
	OC_METHODGATE();
	quint32 range;
	
	// Modulation is calculated in two parts.
	//
	// The modulation time effects the sinus applied to the change in
	// frequency.  An index out of the current time range (both in samples)
	// is incremented each sample.  The range is bound to a reasonable
	// minimum (1 sample) and when the timing changes, the index is rescaled
	// to the new range (to keep the sinus consistent).
	
	range = max<quint32>(Truncate(modTime*frequency), 1);
	mModulator.mIndex = static_cast<quint32>(mModulator.mIndex * static_cast<uint64_t>(range) / mModulator.mRange);
	mModulator.mRange = range;
	
	// The modulation depth effects the amount of frequency change over the
	// range of the sinus.  It needs to be scaled by the modulation time so
	// that a given depth produces a consistent change in frequency over all
	// ranges of time.  Since the depth is applied to a sinus value, it needs
	// to be halfed once for the sinus range and again for the sinus swing
	// in time (half of it is spent decreasing the frequency, half is spent
	// increasing it).
	
	mModulator.mDepth = modDepth * MODULATION_DEPTH_COEFF * modTime / 2.0 / 2.0 * frequency;
}

// Update the offsets for the initial effect delay line.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateDelayLine(T earlyDelay, T lateDelay, quint32 frequency)
{
	OC_METHODGATE();
	// Calculate the initial delay taps.
	mDelayTap[0] = Truncate(earlyDelay * frequency);
	mDelayTap[1] = Truncate((earlyDelay + lateDelay) * frequency);
}

// Update the early reflections gain and line coefficients.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateEarlyLines(T reverbGain, T earlyGain, T lateDelay)
{
	OC_METHODGATE();
	quint32 index;
	
	// Calculate the early reflections gain (from the master effect gain, and
	// reflections gain parameters) with a constant attenuation of 0.5.
	mEarly.mGain = 0.5 * reverbGain * earlyGain;
	
	// Calculate the gain (coefficient) for each early delay line using the
	// late delay time.  This expands the early reflections to the start of
	// the late reverb.
	for(index = 0; index < 4; index++) {
		mEarly.mCoeff[index] = CalcDecayCoeff(EARLY_LINE_LENGTH[index], lateDelay);
	}
}

// Update the offsets for the decorrelator line.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateDecorrelator(T density, quint32 frequency)
{
	OC_METHODGATE();
	quint32 index;
	T length;
	
	// The late reverb inputs are decorrelated to smooth the reverb tail and
	// reduce harsh echos.  The first tap occurs immediately, while the
	// remaining taps are delayed by multiples of a fraction of the smallest
	// cyclical delay time.
	//
	// offset[index] = (FRACTION (MULTIPLIER^index)) smallest_delay
	
	for(index = 0; index < 3; index++) {
		length = (DECO_FRACTION * std::pow(DECO_MULTIPLIER, static_cast<T>(index))) * LATE_LINE_LENGTH[0] * (1.0 + (density * LATE_LINE_MULTIPLIER));
		mDecoTap[index] = Truncate(length * frequency);
	}
}

// Update the late reverb gains, line lengths, and line coefficients.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateLateLines(T reverbGain, T lateGain, T xMix, T density, T decayTime, T diffusion, T hfRatio, T cw, quint32 frequency)
{
	OC_METHODGATE();
	T length;
	quint32 index;
	
	// Calculate the late reverb gain (from the master effect gain, and late
	// reverb gain parameters).  Since the output is tapped prior to the
	// application of the next delay line coefficients, this gain needs to be
	// attenuated by the 'x' mixing matrix coefficient as well.
	
	mLate.mGain = reverbGain * lateGain * xMix;
	
	// To compensate for changes in modal density and decay time of the late
	// reverb signal, the input is attenuated based on the maximal energy of
	// the outgoing signal.  This approximation is used to keep the apparent
	// energy of the signal equal for all ranges of density and decay time.
	//
	// The average length of the cyclcical delay lines is used to calculate
	// the attenuation coefficient.
	
	length = (LATE_LINE_LENGTH[0] + LATE_LINE_LENGTH[1] + LATE_LINE_LENGTH[2] + LATE_LINE_LENGTH[3]) / 4.0;
	length *= 1.0 + (density * LATE_LINE_MULTIPLIER);
	mLate.mDensityGain = CalcDensityGain(CalcDecayCoeff(length, decayTime));
	
	// Calculate the all-pass feed-back and feed-forward coefficient.
	mLate.mApFeedCoeff = 0.5 * std::pow(diffusion, 2.0);
	
	for(index = 0; index < 4; index++) {
		// Calculate the gain (coefficient) for each all-pass line.
		mLate.mApCoeff[index] = CalcDecayCoeff(ALLPASS_LINE_LENGTH[index], decayTime);
		
		// Calculate the length (in seconds) of each cyclical delay line.
		length = LATE_LINE_LENGTH[index] * (1.0 + (density * LATE_LINE_MULTIPLIER));
		
		// Calculate the delay offset for each cyclical delay line.
		mLate.mOffset[index] = Truncate(length * frequency);
		
		// Calculate the gain (coefficient) for each cyclical line.
		mLate.mCoeff[index] = CalcDecayCoeff(length, decayTime);
		
		// Calculate the damping coefficient for each low-pass filter.
		mLate.mLpCoeff[index] = CalcDampingCoeff(hfRatio, length, decayTime, mLate.mCoeff[index], cw);
		
		// Attenuate the cyclical line coefficients by the mixing coefficient
		// (x).
		mLate.mCoeff[index] *= xMix;
	}
}

// Update the echo gain, line offset, line coefficients, and mixing coefficients.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::updateEchoLine(T reverbGain, T lateGain, T echoTime, T decayTime, T diffusion, T echoDepth, T hfRatio, T cw, quint32 frequency)
{
	OC_METHODGATE();
	// Update the offset and coefficient for the echo delay line.
	mEcho.mOffset = Truncate(echoTime * frequency);
	
	// Calculate the decay coefficient for the echo line.
	mEcho.mCoeff = CalcDecayCoeff(echoTime, decayTime);
	
	// Calculate the energy-based attenuation coefficient for the echo delay
	// line.
	mEcho.mDensityGain = CalcDensityGain(mEcho.mCoeff);
	
	// Calculate the echo all-pass feed coefficient.
	mEcho.mApFeedCoeff = 0.5 * std::pow(diffusion, 2.0);
	
	// Calculate the echo all-pass attenuation coefficient.
	mEcho.mApCoeff = CalcDecayCoeff(ECHO_ALLPASS_LENGTH, decayTime);
	
	// Calculate the damping coefficient for each low-pass filter.
	mEcho.mLpCoeff = CalcDampingCoeff(hfRatio, echoTime, decayTime, mEcho.mCoeff, cw);
	
	// Calculate the echo mixing coefficients.  The first is applied to the
	// echo itself.  The second is used to attenuate the late reverb when
	// echo depth is high and diffusion is low, so the echo is slightly
	// stronger than the decorrelated echos in the reverb tail.
	
	mEcho.mMixCoeff[0] = reverbGain * lateGain * echoDepth;
	mEcho.mMixCoeff[1] = 1.0 - (echoDepth * 0.5 * (1.0 - diffusion));
}

// Update the early and late 3D panning gains.
template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::update3DPanning(const T *ReflectionsPan, const T *LateReverbPan, T Gain)
{
	OC_METHODGATE();
	T earlyPan[3] = { ReflectionsPan[0], ReflectionsPan[1], -ReflectionsPan[2] };
	T latePan[3] = { LateReverbPan[0], LateReverbPan[1], -LateReverbPan[2] };
	T AmbientGains[OUTPUT_CHANNELS];
	T DirGains[OUTPUT_CHANNELS];
	T length, invlen;
	quint32 i;
	
	computeAmbientGains(1.4142, AmbientGains);
	
	length = earlyPan[0]*earlyPan[0] + earlyPan[1]*earlyPan[1] + earlyPan[2]*earlyPan[2];
	if(!(length > FLT_EPSILON)) {
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mEarly.mPanGain[i] = AmbientGains[i] * Gain;
		}
	} else {
		invlen = 1.0 / std::sqrt(length);
		earlyPan[0] *= invlen;
		earlyPan[1] *= invlen;
		earlyPan[2] *= invlen;
		
		length = std::min(length, 1.0);
		computeDirectionalGains(earlyPan, 1.4142, DirGains);
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mEarly.mPanGain[i] = lerp(AmbientGains[i], DirGains[i], length) * Gain;
		}
	}
	
	length = latePan[0]*latePan[0] + latePan[1]*latePan[1] + latePan[2]*latePan[2];
	if(!(length > FLT_EPSILON)) {
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mLate.mPanGain[i] = AmbientGains[i] * Gain;
		}
	} else {
		invlen = 1.0 / std::sqrt(length);
		latePan[0] *= invlen;
		latePan[1] *= invlen;
		latePan[2] *= invlen;
		
		length = std::min(length, 1.0);
		computeDirectionalGains(latePan, 1.4142, DirGains);
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mLate.mPanGain[i] = lerp(AmbientGains[i], DirGains[i], length) * Gain;
		}
	}
}


template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::update(QAudioFormat f)
{
	OC_METHODGATE();
	int frequency=f.sampleRate();
	T lfscale, hfscale, hfRatio;
	T cw, x, y;
	
	quint32 freq=static_cast<quint32>(frequency);
	
	// Calculate the master low-pass filter (from the master effect HF gain).
	hfscale = settings.HFReference / frequency;
	mLpFilter.setParams(Filter_HighShelf, settings.GainHF, hfscale, 0.0);
	
	lfscale = settings.LFReference / frequency;
	mHpFilter.setParams(Filter_LowShelf, settings.GainLF, lfscale, 0.0);
	
	// Update the modulator line.
	updateModulator(settings.ModulationTime, settings.ModulationDepth, freq);
	
	// Update the initial effect delay.
	updateDelayLine(settings.ReflectionsDelay, settings.LateReverbDelay, freq);
	
	// Update the early lines.
	updateEarlyLines(settings.Gain, settings.ReflectionsGain, settings.LateReverbDelay);
	
	// Update the decorrelator.
	updateDecorrelator(settings.Density, freq);
	
	// Get the mixing matrix coefficients (x and y).
	CalcMatrixCoeffs(settings.Diffusion, &x, &y);
	// Then divide x into y to simplify the matrix calculation.
	mLate.mMixCoeff = y / x;
	
	// If the HF limit parameter is flagged, calculate an appropriate limit
	// based on the air absorption parameter.
	hfRatio = settings.DecayHFRatio;
	
	if(settings.DecayHFLimit && settings.AirAbsorptionGainHF < 1.0) {
		hfRatio = CalcLimitedHfRatio(hfRatio, settings.AirAbsorptionGainHF, settings.DecayTime);
	}
	
	cw = std::cos(M_2_PI * hfscale);
	// Update the late lines.
	updateLateLines(settings.Gain, settings.LateReverbGain, x, settings.Density, settings.DecayTime, settings.Diffusion, hfRatio, cw, freq);
	
	// Update the echo line.
	updateEchoLine(settings.Gain, settings.LateReverbGain, settings.EchoTime, settings.DecayTime, settings.Diffusion, settings.EchoDepth, hfRatio, cw, freq);
	
	// Update early and late 3D panning.
	update3DPanning(settings.ReflectionsPan, settings.LateReverbPan, ReverbBoost);
}



template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::loadPreset(reverb::Settings<T> &set)
{
	OC_METHODGATE();
	settings=set;
	settings.correct();
}




template <typename T, int OUTPUT_CHANNELS, int REVERB_BUFFERSIZE, int MAX_AMBI_COEFFS>
void Reverb<T, OUTPUT_CHANNELS, REVERB_BUFFERSIZE, MAX_AMBI_COEFFS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	const int frequency = f.sampleRate();
	ambiCoeffs[0][0] = 0.7071;
	ambiCoeffs[0][1] = 0.5;
	ambiCoeffs[0][2] = 0.0;
	ambiCoeffs[0][3] = 0.0;
	
	ambiCoeffs[1][0] = 0.7071;
	ambiCoeffs[1][1] = -0.5;
	ambiCoeffs[2][2] = 0.0;
	ambiCoeffs[3][3] = 0.0;
	
	quint32 index=0;
	
	mTotalSamples = 0;
	mSampleBuffer = nullptr;
	
	mLpFilter.clear();
	mHpFilter.clear();
	
	mModulator.mDelay.mMask = 0;
	mModulator.mDelay.mLine = nullptr;
	mModulator.mIndex = 0;
	mModulator.mRange = 1;
	mModulator.mDepth = 0.0;
	mModulator.mCoeff = 0.0;
	mModulator.mFilter = 0.0;
	
	mDelay.mMask = 0;
	mDelay.mLine = nullptr;
	mDelayTap[0] = 0;
	mDelayTap[1] = 0;
	
	mEarly.mGain = 0.0;
	for(index = 0; index < 4; index++) {
		mEarly.mCoeff[index] = 0.0;
		mEarly.mDelay[index].mMask = 0;
		mEarly.mDelay[index].mLine = nullptr;
		mEarly.mOffset[index] = 0;
	}
	
	mDecorrelator.mMask = 0;
	mDecorrelator.mLine = nullptr;
	mDecoTap[0] = 0;
	mDecoTap[1] = 0;
	mDecoTap[2] = 0;
	
	mLate.mGain = 0.0;
	mLate.mDensityGain = 0.0;
	mLate.mApFeedCoeff = 0.0;
	mLate.mMixCoeff = 0.0;
	for(index = 0; index < 4; index++) {
		mLate.mApCoeff[index] = 0.0;
		mLate.mApDelay[index].mMask = 0;
		mLate.mApDelay[index].mLine = nullptr;
		mLate.mApOffset[index] = 0;
		
		mLate.mCoeff[index] = 0.0;
		mLate.mDelay[index].mMask = 0;
		mLate.mDelay[index].mLine = nullptr;
		mLate.mOffset[index] = 0;
		
		mLate.mLpCoeff[index] = 0.0;
		mLate.mLpSample[index] = 0.0;
	}
	
	for(index = 0; index < OUTPUT_CHANNELS; index++) {
		mEarly.mPanGain[index] = 0.0;
		mLate.mPanGain[index] = 0.0;
	}
	
	mEcho.mDensityGain = 0.0;
	mEcho.mDelay.mMask = 0;
	mEcho.mDelay.mLine = nullptr;
	mEcho.mApDelay.mMask = 0;
	mEcho.mApDelay.mLine = nullptr;
	mEcho.mCoeff = 0.0;
	mEcho.mApFeedCoeff = 0.0;
	mEcho.mApCoeff = 0.0;
	mEcho.mOffset = 0;
	mEcho.mApOffset = 0;
	mEcho.mLpCoeff = 0.0;
	mEcho.mLpSample = 0.0;
	mEcho.mMixCoeff[0] = 0.0;
	mEcho.mMixCoeff[1] = 0.0;
	
	mOffset = 0;
	
	settings.Density   = REVERB_DENSITY_DEFAULT;
	settings.Diffusion = REVERB_DIFFUSION_DEFAULT;
	settings.Gain   = REVERB_DEFAULT_GAIN;
	settings.GainHF = REVERB_DEFAULT_GAINHF;
	settings.GainLF = REVERB_DEFAULT_GAINLF;
	settings.DecayTime    = REVERB_DEFAULT_DECAY_TIME;
	settings.DecayHFRatio = REVERB_DEFAULT_DECAY_HFRATIO;
	settings.DecayLFRatio = REVERB_DEFAULT_DECAY_LFRATIO;
	settings.ReflectionsGain   = REVERB_DEFAULT_REFLECTIONS_GAIN;
	settings.ReflectionsDelay  = REVERB_DEFAULT_REFLECTIONS_DELAY;
	settings.ReflectionsPan[0] = REVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.ReflectionsPan[1] = REVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.ReflectionsPan[2] = REVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.LateReverbGain   = REVERB_DEFAULT_LATE_REVERB_GAIN;
	settings.LateReverbDelay  = REVERB_DEFAULT_LATE_REVERB_DELAY;
	settings.LateReverbPan[0] = REVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.LateReverbPan[1] = REVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.LateReverbPan[2] = REVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.EchoTime  = REVERB_DEFAULT_ECHO_TIME;
	settings.EchoDepth = REVERB_DEFAULT_ECHO_DEPTH;
	settings.ModulationTime  = REVERB_DEFAULT_MODULATION_TIME;
	settings.ModulationDepth = REVERB_DEFAULT_MODULATION_DEPTH;
	settings.AirAbsorptionGainHF = REVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
	settings.HFReference = REVERB_DEFAULT_HFREFERENCE;
	settings.LFReference = REVERB_DEFAULT_LFREFERENCE;
	settings.RoomRolloffFactor = REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
	settings.DecayHFLimit = REVERB_DEFAULT_DECAY_HFLIMIT;
	
	// Allocate the delay lines.
	allocLines(static_cast<quint32>(frequency));
	
	// Calculate the modulation filter coefficient.  Notice that the exponent
	// is calculated given the current sample rate.  This ensures that the
	// resulting filter response over time is consistent across all sample
	// rates.
	mModulator.mCoeff = std::pow(MODULATION_FILTER_COEFF, MODULATION_FILTER_CONST / frequency);
	
	// The early reflection and late all-pass filter line lengths are static,
	// so their offsets only need to be calculated once.
	for(index = 0; index < 4; index++) {
		mEarly.mOffset[index] = Truncate(EARLY_LINE_LENGTH[index] * frequency);
		mLate.mApOffset[index] = Truncate(ALLPASS_LINE_LENGTH[index] * frequency);
	}
	
	// The echo all-pass filter line length is static, so its offset only
	// needs to be calculated once.
	mEcho.mApOffset = Truncate(ECHO_ALLPASS_LENGTH * frequency);
	
}

}

}


#endif
// REVERB_HPP
