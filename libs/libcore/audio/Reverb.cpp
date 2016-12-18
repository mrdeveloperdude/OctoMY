#include "Reverb.hpp"

#include "../libutil/utility/Standard.hpp"

#include <cmath>


const qreal ReverbEffect::EARLY_LINE_LENGTH[] = {
	0.0015f, 0.0045f, 0.0135f, 0.0405f
};

// The lengths of the late all-pass delay lines.
const qreal ReverbEffect::ALLPASS_LINE_LENGTH[] = {
	0.0151f, 0.0167f, 0.0183f, 0.0200f,
};

// The lengths of the late cyclical delay lines.
const qreal ReverbEffect::LATE_LINE_LENGTH[] = {
	0.0211f, 0.0311f, 0.0461f, 0.0680f
};


void ReverbEffect::computeAmbientGains(qreal ingain, qreal gains[OUTPUT_CHANNELS])
{
	OC_METHODGATE();
	quint32 i;

	for(i = 0; i < OUTPUT_CHANNELS; i++) {
		// The W coefficients are based on a mathematical average of the
		// output, scaled by sqrt(2) to compensate for FuMa-style Ambisonics
		// scaling the W channel input by sqrt(0.5). The square root of the
		// base average provides for a more perceptual average volume, better
		// suited to non-directional gains.
		gains[i] = sqrtf(ambiCoeffs[i][0]/1.4142f) * ingain;
	}
}

void ReverbEffect::computeDirectionalGains(const qreal dir[3], qreal ingain, qreal gains[OUTPUT_CHANNELS])
{
	OC_METHODGATE();
	qreal coeffs[MAX_AMBI_COEFFS];
	quint32 i, j;
	/* Convert from OpenAL coords to Ambisonics. */
	qreal x = -dir[2];
	qreal y = -dir[0];
	qreal z =  dir[1];

	/* Zeroth-order */
	coeffs[0] = 0.7071f; /* W = sqrt(1.0 / 2.0) */
	/* First-order */
	coeffs[1] = y; /* Y = Y */
	coeffs[2] = z; /* Z = Z */
	coeffs[3] = x; /* X = X */

	for(i = 0; i < OUTPUT_CHANNELS; i++) {
		qreal gain = 0.0f;
		for(j = 0; j < MAX_AMBI_COEFFS; j++) {
			gain += ambiCoeffs[i][j]*coeffs[j];
		}
		gains[i] = gain * ingain;
	}
}


// Given an input sample, this function produces modulation for the late reverb.
inline qreal ReverbEffect::EAXModulation(qreal in)
{
	qreal sinus, frac;
	quint32 offset;
	qreal out0, out1;

	// Calculate the sinus rythm (dependent on modulation time and the
	// sampling rate).  The center of the sinus is moved to reduce the delay
	// of the effect when the time or depth are low.
	sinus = 1.0f - cosf(F_2PI * mModulator.mIndex / mModulator.mRange);

	// The depth determines the range over which to read the input samples
	// from, so it must be filtered to reduce the distortion caused by even
	// small parameter changes.
	mModulator.mFilter = lerp(mModulator.mFilter, mModulator.mDepth, mModulator.mCoeff);

	// Calculate the read offset and fraction between it and the next sample.
	frac   = (1.0f + (mModulator.mFilter * sinus));
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
inline qreal ReverbEffect::earlyDelayLineOut(quint32 index)
{
	return mEarly.mDelay[index].attenuatedDelayLineOut(mOffset - mEarly.mOffset[index], mEarly.mCoeff[index]);
}

// Given an input sample, this function produces four-channel output for the  early reflections.
inline void ReverbEffect::earlyReflection(qreal in, qreal *out)
{
	qreal d[4], v, f[4];

	// Obtain the decayed results of each early delay line.
	for(int i=0; i<4; ++i) {
		d[i] = earlyDelayLineOut(i);
	}

	/* The following uses a lossless scattering junction from waveguide
	 * theory.  It actually amounts to a householder mixing matrix, which
	 * will produce a maximally diffuse response, and means this can probably
	 * be considered a simple feed-back delay network (FDN).
	 *          N
	 *         ---
	 *         \
	 * v = 2/N /   d_i
	 *         ---
	 *         i=1
	 */
	v = (d[0] + d[1] + d[2] + d[3]) * 0.5f;
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
inline qreal ReverbEffect::lateAllPassInOut(quint32 index, qreal in)
{
	return mLate.mApDelay[index].allpassInOut(mOffset - mLate.mApOffset[index], mOffset, in, mLate.mApFeedCoeff, mLate.mApCoeff[index]);
}

// Delay line output routine for late reverb.
inline qreal ReverbEffect::lateDelayLineOut(quint32 index)
{
	return mLate.mDelay[index].attenuatedDelayLineOut(mOffset - mLate.mOffset[index], mLate.mCoeff[index]);
}

// Low-pass filter input/output routine for late reverb.
inline qreal ReverbEffect::lateLowPassInOut(quint32 index, qreal in)
{
	in = lerp(in, mLate.mLpSample[index], mLate.mLpCoeff[index]);
	mLate.mLpSample[index] = in;
	return in;
}

// Given four decorrelated input samples, this function produces four-channel output for the late reverb.
inline void ReverbEffect::lateReverb(const qreal *in, qreal *out)
{
	qreal d[4], f[4];

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
	for(int i=0; i<4; ++i) {
		d[i] = lateAllPassInOut(i, d[i]);
	}


	/* Late reverb is done with a modified feed-back delay network (FDN)
	 * topology.  Four input lines are each fed through their own all-pass
	 * filter and then into the mixing matrix.  The four outputs of the
	 * mixing matrix are then cycled back to the inputs.  Each output feeds
	 * a different input to form a circlular feed cycle.
	 *
	 * The mixing matrix used is a 4D skew-symmetric rotation matrix derived
	 * using a single unitary rotational parameter:
	 *
	 *  [  d,  a,  b,  c ]          1 = a^2 + b^2 + c^2 + d^2
	 *  [ -a,  d,  c, -b ]
	 *  [ -b, -c,  d,  a ]
	 *  [ -c,  b, -a,  d ]
	 *
	 * The rotation is constructed from the effect's diffusion parameter,
	 * yielding:  1 = x^2 + 3 y^2; where a, b, and c are the coefficient y
	 * with differing signs, and d is the coefficient x.  The matrix is thus:
	 *
	 *  [  x,  y, -y,  y ]          n = sqrt(matrix_order - 1)
	 *  [ -y,  x,  y,  y ]          t = diffusion_parameter * atan(n)
	 *  [  y, -y,  x,  y ]          x = cos(t)
	 *  [ -y, -y, -y,  x ]          y = sin(t) / n
	 *
	 * To reduce the number of multiplies, the x coefficient is applied with
	 * the cyclical delay line coefficients.  Thus only the y coefficient is
	 * applied when mixing, and is modified to be:  y / x.
	 */
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
inline void ReverbEffect::EAXEcho(qreal in, qreal *late)
{
	qreal out, feed;

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
inline void ReverbEffect::EAXVerbPass(qreal in, qreal *early, qreal *late)
{
	OC_METHODGATE();
	qreal feed, taps[4];

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

void ReverbEffect::Process(quint32 SamplesToDo, const qreal *SamplesIn, qreal *SamplesOut)
{
	OC_METHODGATE();
	qreal (*early)[4] = earlySamples;
	qreal (*late)[4] = reverbSamples;
	quint32 index, c;

	// Process reverb for these samples.
	for(index = 0; index < SamplesToDo; index++) {
		EAXVerbPass(SamplesIn[index], early[index], late[index]);
	}

	for(c = 0; c < OUTPUT_CHANNELS ; c++) {
		qreal earlyGain, lateGain;

		earlyGain = mEarly.mPanGain[c];
		if(fabsf(earlyGain) > GAIN_SILENCE_THRESHOLD) {
			for(index = 0; index < SamplesToDo; index++) {
				SamplesOut[index * OUTPUT_CHANNELS + c] = earlyGain*early[index][c&3];
			}
		}


		lateGain = mLate.mPanGain[c];
		if(fabsf(lateGain) > GAIN_SILENCE_THRESHOLD) {
			for(index = 0; index < SamplesToDo; index++) {
				SamplesOut[index * OUTPUT_CHANNELS + c] = lateGain*late[index][c&3];
			}
		}
	}
}

// Given the allocated sample buffer, this function updates each delay line offset.
static inline void RealizeLineOffset(qreal *sampleBuffer, DelayLine *Delay)
{
	Delay->mLine = &sampleBuffer[(ptrdiff_t)Delay->mLine];
}

// Calculate the length of a delay line and store its mask and offset.
static quint32 CalcLineLength(qreal length, ptrdiff_t offset, quint32 frequency, DelayLine *Delay)
{
	quint32 samples;

	// All line lengths are powers of 2, calculated from their lengths, with
	// an additional sample in case of rounding errors.
	samples = NextPowerOf2(Truncate(length * frequency) + 1);
	// All lines share a single sample buffer.
	Delay->mMask = samples - 1;
	Delay->mLine = (qreal*)offset;
	// Return the sample count for accumulation.
	return samples;
}

// Calculates the delay line metrics and allocates the shared sample buffer for all lines given the sample rate (frequency).
void ReverbEffect::allocLines(quint32 frequency)
{
	quint32 totalSamples, index;
	qreal length;

	// All delay line lengths are calculated to accomodate the full range of lengths given their respective paramters.
	totalSamples = 0;

	/* The modulator's line length is calculated from the maximum modulation
	 * time and depth coefficient, and halfed for the low-to-high frequency
	 * swing.  An additional sample is added to keep it stable when there is no modulation.
	 */
	length = (EAXREVERB_MAX_MODULATION_TIME*MODULATION_DEPTH_COEFF/2.0f) + (1.0f / frequency);
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mModulator.mDelay);

	// The initial delay is the sum of the reflections and late reverb delays.
	length = EAXREVERB_MAX_REFLECTIONS_DELAY + EAXREVERB_MAX_LATE_REVERB_DELAY;
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mDelay);

	// The early reflection lines.
	for(index = 0; index < 4; index++) {
		totalSamples += CalcLineLength(EARLY_LINE_LENGTH[index], totalSamples, frequency, &mEarly.mDelay[index]);
	}

	// The decorrelator line is calculated from the lowest reverb density (a
	// parameter value of 1).
	length = (DECO_FRACTION * DECO_MULTIPLIER * DECO_MULTIPLIER) * LATE_LINE_LENGTH[0] * (1.0f + LATE_LINE_MULTIPLIER);
	totalSamples += CalcLineLength(length, totalSamples, frequency, &mDecorrelator);

	// The late all-pass lines.
	for(index = 0; index < 4; index++) {
		totalSamples += CalcLineLength(ReverbEffect::ALLPASS_LINE_LENGTH[index], totalSamples, frequency, &mLate.mApDelay[index]);
	}

	// The late delay lines are calculated from the lowest reverb density.
	for(index = 0; index < 4; index++) {
		length = LATE_LINE_LENGTH[index] * (1.0f + LATE_LINE_MULTIPLIER);
		totalSamples += CalcLineLength(length, totalSamples, frequency, &mLate.mDelay[index]);
	}

	// The echo all-pass and delay lines.
	totalSamples += CalcLineLength(ECHO_ALLPASS_LENGTH, totalSamples, frequency, &mEcho.mApDelay);
	totalSamples += CalcLineLength(EAXREVERB_MAX_ECHO_TIME, totalSamples, frequency, &mEcho.mDelay);

	qreal* newBuf = new qreal[totalSamples];
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
		mSampleBuffer[index] = 0.0f;
	}
}

// Calculate a decay coefficient given the length of each cycle and the time until the decay reaches -60 dB.
static inline qreal CalcDecayCoeff(qreal length, qreal decayTime)
{
	return powf(0.001f/*-60 dB*/, length/decayTime);
}

// Calculate a decay length from a coefficient and the time until the decay reaches -60 dB.
static inline qreal CalcDecayLength(qreal coeff, qreal decayTime)
{
	return log10f(coeff) * decayTime / log10f(0.001f)/*-60 dB*/;
}

// Calculate an attenuation to be applied to the input of any echo models to
// compensate for modal density and decay time.
static inline qreal CalcDensityGain(qreal a)
{
	/* The energy of a signal can be obtained by finding the area under the
	 * squared signal.  This takes the form of Sum(x_n^2), where x is the
	 * amplitude for the sample n.
	 *
	 * Decaying feedback matches exponential decay of the form Sum(a^n),
	 * where a is the attenuation coefficient, and n is the sample.  The area
	 * under this decay curve can be calculated as:  1 / (1 - a).
	 *
	 * Modifying the above equation to find the squared area under the curve
	 * (for energy) yields:  1 / (1 - a^2).  Input attenuation can then be
	 * calculated by inverting the square root of this approximation,
	 * yielding:  1 / sqrt(1 / (1 - a^2)), simplified to: sqrt(1 - a^2).
	 */
	return sqrtf(1.0f - (a * a));
}

// Calculate the mixing matrix coefficients given a diffusion factor.
static inline void CalcMatrixCoeffs(qreal diffusion, qreal *x, qreal *y)
{
	qreal n, t;

	// The matrix is of order 4, so n is sqrt (4 - 1).
	n = sqrtf(3.0f);
	t = diffusion * atanf(n);

	// Calculate the first mixing matrix coefficient.
	*x = cosf(t);
	// Calculate the second mixing matrix coefficient.
	*y = sinf(t) / n;
}

// Calculate the limited HF ratio for use with the late reverb low-pass filters.
static qreal CalcLimitedHfRatio(qreal hfRatio, qreal airAbsorptionGainHF, qreal decayTime)
{
	qreal limitRatio;

	/* Find the attenuation due to air absorption in dB (converting delay
	 * time to meters using the speed of sound).  Then reversing the decay
	 * equation, solve for HF ratio.  The delay length is cancelled out of
	 * the equation, so it can be calculated once for all lines.
	 */
	limitRatio = 1.0f / (CalcDecayLength(airAbsorptionGainHF, decayTime) * SPEEDOFSOUNDMETRESPERSEC);
	/* Using the limit calculated above, apply the upper bound to the HF
	 * ratio. Also need to limit the result to a minimum of 0.1, just like the
	 * HF ratio parameter. */
	return clampf(limitRatio, 0.1f, hfRatio);
}

// Calculate the coefficient for a HF (and eventually LF) decay damping filter.
static inline qreal CalcDampingCoeff(qreal hfRatio, qreal length, qreal decayTime, qreal decayCoeff, qreal cw)
{
	qreal coeff, g;

	// Eventually this should boost the high frequencies when the ratio
	// exceeds 1.
	coeff = 0.0f;
	if (hfRatio < 1.0f) {
		// Calculate the low-pass coefficient by dividing the HF decay
		// coefficient by the full decay coefficient.
		g = CalcDecayCoeff(length, decayTime * hfRatio) / decayCoeff;

		// Damping is done with a 1-pole filter, so g needs to be squared.
		g *= g;
		if(g < 0.9999f) { /* 1-epsilon */
			/* Be careful with gains < 0.001, as that causes the coefficient
			 * head towards 1, which will flatten the signal. */
			g = maxf(g, 0.001f);
			coeff = (1 - g*cw - sqrtf(2*g*(1-cw) - g*g*(1 - cw*cw))) /
					(1 - g);
		}

		// Very low decay times will produce minimal output, so apply an
		// upper bound to the coefficient.
		coeff = minf(coeff, 0.98f);
	}
	return coeff;
}

// Update the EAX modulation index, range, and depth.
// Keep in mind that this kind of vibrato is additive and not multiplicative as one may expect.
// The downswing will sound stronger than the upswing.
void ReverbEffect::updateModulator(qreal modTime, qreal modDepth, quint32 frequency)
{
	quint32 range;

	/* Modulation is calculated in two parts.
	 *
	 * The modulation time effects the sinus applied to the change in
	 * frequency.  An index out of the current time range (both in samples)
	 * is incremented each sample.  The range is bound to a reasonable
	 * minimum (1 sample) and when the timing changes, the index is rescaled
	 * to the new range (to keep the sinus consistent).
	 */
	range = maxu(Truncate(modTime*frequency), 1);
	mModulator.mIndex = (quint32)(mModulator.mIndex * (uint64_t)range / mModulator.mRange);
	mModulator.mRange = range;

	/* The modulation depth effects the amount of frequency change over the
	 * range of the sinus.  It needs to be scaled by the modulation time so
	 * that a given depth produces a consistent change in frequency over all
	 * ranges of time.  Since the depth is applied to a sinus value, it needs
	 * to be halfed once for the sinus range and again for the sinus swing
	 * in time (half of it is spent decreasing the frequency, half is spent
	 * increasing it).
	 */
	mModulator.mDepth = modDepth * MODULATION_DEPTH_COEFF * modTime / 2.0f /
						2.0f * frequency;
}

// Update the offsets for the initial effect delay line.
void ReverbEffect::updateDelayLine(qreal earlyDelay, qreal lateDelay, quint32 frequency)
{
	// Calculate the initial delay taps.
	mDelayTap[0] = Truncate(earlyDelay * frequency);
	mDelayTap[1] = Truncate((earlyDelay + lateDelay) * frequency);
}

// Update the early reflections gain and line coefficients.
void ReverbEffect::updateEarlyLines(qreal reverbGain, qreal earlyGain, qreal lateDelay)
{
	quint32 index;

	// Calculate the early reflections gain (from the master effect gain, and
	// reflections gain parameters) with a constant attenuation of 0.5.
	mEarly.mGain = 0.5f * reverbGain * earlyGain;

	// Calculate the gain (coefficient) for each early delay line using the
	// late delay time.  This expands the early reflections to the start of
	// the late reverb.
	for(index = 0; index < 4; index++)
		mEarly.mCoeff[index] = CalcDecayCoeff(EARLY_LINE_LENGTH[index],
											  lateDelay);
}

// Update the offsets for the decorrelator line.
void ReverbEffect::updateDecorrelator(qreal density, quint32 frequency)
{
	quint32 index;
	qreal length;

	/* The late reverb inputs are decorrelated to smooth the reverb tail and
	 * reduce harsh echos.  The first tap occurs immediately, while the
	 * remaining taps are delayed by multiples of a fraction of the smallest
	 * cyclical delay time.
	 *
	 * offset[index] = (FRACTION (MULTIPLIER^index)) smallest_delay
	 */
	for(index = 0; index < 3; index++) {
		length = (DECO_FRACTION * powf(DECO_MULTIPLIER, (qreal)index)) *
				 LATE_LINE_LENGTH[0] * (1.0f + (density * LATE_LINE_MULTIPLIER));
		mDecoTap[index] = Truncate(length * frequency);
	}
}

// Update the late reverb gains, line lengths, and line coefficients.
void ReverbEffect::updateLateLines(qreal reverbGain, qreal lateGain, qreal xMix, qreal density, qreal decayTime, qreal diffusion, qreal hfRatio, qreal cw, quint32 frequency)
{
	qreal length;
	quint32 index;

	/* Calculate the late reverb gain (from the master effect gain, and late
	 * reverb gain parameters).  Since the output is tapped prior to the
	 * application of the next delay line coefficients, this gain needs to be
	 * attenuated by the 'x' mixing matrix coefficient as well.
	 */
	mLate.mGain = reverbGain * lateGain * xMix;

	/* To compensate for changes in modal density and decay time of the late
	 * reverb signal, the input is attenuated based on the maximal energy of
	 * the outgoing signal.  This approximation is used to keep the apparent
	 * energy of the signal equal for all ranges of density and decay time.
	 *
	 * The average length of the cyclcical delay lines is used to calculate
	 * the attenuation coefficient.
	 */
	length = (LATE_LINE_LENGTH[0] + LATE_LINE_LENGTH[1] +
			  LATE_LINE_LENGTH[2] + LATE_LINE_LENGTH[3]) / 4.0f;
	length *= 1.0f + (density * LATE_LINE_MULTIPLIER);
	mLate.mDensityGain = CalcDensityGain(CalcDecayCoeff(length, decayTime));

	// Calculate the all-pass feed-back and feed-forward coefficient.
	mLate.mApFeedCoeff = 0.5f * powf(diffusion, 2.0f);

	for(index = 0; index < 4; index++) {
		// Calculate the gain (coefficient) for each all-pass line.
		mLate.mApCoeff[index] = CalcDecayCoeff(ALLPASS_LINE_LENGTH[index], decayTime);

		// Calculate the length (in seconds) of each cyclical delay line.
		length = LATE_LINE_LENGTH[index] * (1.0f + (density * LATE_LINE_MULTIPLIER));

		// Calculate the delay offset for each cyclical delay line.
		mLate.mOffset[index] = Truncate(length * frequency);

		// Calculate the gain (coefficient) for each cyclical line.
		mLate.mCoeff[index] = CalcDecayCoeff(length, decayTime);

		// Calculate the damping coefficient for each low-pass filter.
		mLate.mLpCoeff[index] =
			CalcDampingCoeff(hfRatio, length, decayTime,
							 mLate.mCoeff[index], cw);

		// Attenuate the cyclical line coefficients by the mixing coefficient
		// (x).
		mLate.mCoeff[index] *= xMix;
	}
}

// Update the echo gain, line offset, line coefficients, and mixing coefficients.
void ReverbEffect::updateEchoLine(qreal reverbGain, qreal lateGain, qreal echoTime, qreal decayTime, qreal diffusion, qreal echoDepth, qreal hfRatio, qreal cw, quint32 frequency)
{
	// Update the offset and coefficient for the echo delay line.
	mEcho.mOffset = Truncate(echoTime * frequency);

	// Calculate the decay coefficient for the echo line.
	mEcho.mCoeff = CalcDecayCoeff(echoTime, decayTime);

	// Calculate the energy-based attenuation coefficient for the echo delay
	// line.
	mEcho.mDensityGain = CalcDensityGain(mEcho.mCoeff);

	// Calculate the echo all-pass feed coefficient.
	mEcho.mApFeedCoeff = 0.5f * powf(diffusion, 2.0f);

	// Calculate the echo all-pass attenuation coefficient.
	mEcho.mApCoeff = CalcDecayCoeff(ECHO_ALLPASS_LENGTH, decayTime);

	// Calculate the damping coefficient for each low-pass filter.
	mEcho.mLpCoeff = CalcDampingCoeff(hfRatio, echoTime, decayTime, mEcho.mCoeff, cw);

	/* Calculate the echo mixing coefficients.  The first is applied to the
	 * echo itself.  The second is used to attenuate the late reverb when
	 * echo depth is high and diffusion is low, so the echo is slightly
	 * stronger than the decorrelated echos in the reverb tail.
	 */
	mEcho.mMixCoeff[0] = reverbGain * lateGain * echoDepth;
	mEcho.mMixCoeff[1] = 1.0f - (echoDepth * 0.5f * (1.0f - diffusion));
}

// Update the early and late 3D panning gains.
void ReverbEffect::update3DPanning(const qreal *ReflectionsPan, const qreal *LateReverbPan, qreal Gain)
{
	qreal earlyPan[3] = { ReflectionsPan[0], ReflectionsPan[1], -ReflectionsPan[2] };
	qreal latePan[3] = { LateReverbPan[0], LateReverbPan[1], -LateReverbPan[2] };
	qreal AmbientGains[OUTPUT_CHANNELS];
	qreal DirGains[OUTPUT_CHANNELS];
	qreal length, invlen;
	quint32 i;

	computeAmbientGains(1.4142f, AmbientGains);

	length = earlyPan[0]*earlyPan[0] + earlyPan[1]*earlyPan[1] + earlyPan[2]*earlyPan[2];
	if(!(length > FLT_EPSILON)) {
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mEarly.mPanGain[i] = AmbientGains[i] * Gain;
		}
	} else {
		invlen = 1.0f / sqrtf(length);
		earlyPan[0] *= invlen;
		earlyPan[1] *= invlen;
		earlyPan[2] *= invlen;

		length = minf(length, 1.0f);
		computeDirectionalGains(earlyPan, 1.4142f, DirGains);
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
		invlen = 1.0f / sqrtf(length);
		latePan[0] *= invlen;
		latePan[1] *= invlen;
		latePan[2] *= invlen;

		length = minf(length, 1.0f);
		computeDirectionalGains(latePan, 1.4142f, DirGains);
		for(i = 0; i < OUTPUT_CHANNELS; i++) {
			mLate.mPanGain[i] = lerp(AmbientGains[i], DirGains[i], length) * Gain;
		}
	}
}


void ReverbEffect::Update(int frequency)
{
	qreal lfscale, hfscale, hfRatio;
	qreal cw, x, y;

	// Calculate the master low-pass filter (from the master effect HF gain).
	hfscale = settings.HFReference / frequency;
	mLpFilter.setParams(Filter_HighShelf, settings.GainHF, hfscale, 0.0f);

	lfscale = settings.LFReference / frequency;
	mHpFilter.setParams(Filter_LowShelf, settings.GainLF, lfscale, 0.0f);

	// Update the modulator line.
	updateModulator(settings.ModulationTime, settings.ModulationDepth, frequency);

	// Update the initial effect delay.
	updateDelayLine(settings.ReflectionsDelay, settings.LateReverbDelay, frequency);

	// Update the early lines.
	updateEarlyLines(settings.Gain, settings.ReflectionsGain, settings.LateReverbDelay);

	// Update the decorrelator.
	updateDecorrelator(settings.Density, frequency);

	// Get the mixing matrix coefficients (x and y).
	CalcMatrixCoeffs(settings.Diffusion, &x, &y);
	// Then divide x into y to simplify the matrix calculation.
	mLate.mMixCoeff = y / x;

	// If the HF limit parameter is flagged, calculate an appropriate limit
	// based on the air absorption parameter.
	hfRatio = settings.DecayHFRatio;

	if(settings.DecayHFLimit && settings.AirAbsorptionGainHF < 1.0f) {
		hfRatio = CalcLimitedHfRatio(hfRatio, settings.AirAbsorptionGainHF, settings.DecayTime);
	}

	cw = cosf(F_2PI * hfscale);
	// Update the late lines.
	updateLateLines(settings.Gain, settings.LateReverbGain, x, settings.Density, settings.DecayTime, settings.Diffusion, hfRatio, cw, frequency);

	// Update the echo line.
	updateEchoLine(settings.Gain, settings.LateReverbGain, settings.EchoTime, settings.DecayTime, settings.Diffusion, settings.EchoDepth, hfRatio, cw, frequency);

	// Update early and late 3D panning.
	update3DPanning(settings.ReflectionsPan, settings.LateReverbPan, ReverbBoost);
}


static qreal eaxDbToAmp(qreal eaxDb)
{
	qreal dB = eaxDb / 2000.0f;
	return pow(10.0f, dB);
}


void ReverbEffect::LoadPreset(int environment, qreal environmentSize, qreal environmentDiffusion, int room, int roomHF, int roomLF, qreal decayTime, qreal decayHFRatio, qreal decayLFRatio, int reflections, qreal reflectionsDelay, qreal reflectionsPanX, qreal reflectionsPanY, qreal reflectionsPanZ, int reverb, qreal reverbDelay, qreal reverbPanX, qreal reverbPanY, qreal reverbPanZ, qreal echoTime, qreal echoDepth, qreal modulationTime, qreal modulationDepth, qreal airAbsorptionHF, qreal hfReference, qreal lfReference, qreal roomRolloffFactor, int flags)
{
	ReverbSettings set;
	set.Density = 1.0f; // todo, currently default
	set.Diffusion = environmentDiffusion;
	set.Gain =  eaxDbToAmp(room); //0.32f;
	set.GainHF = eaxDbToAmp(roomHF); //0.89f;
	set.GainLF = eaxDbToAmp(roomLF); // 1.0f;
	set.DecayTime = decayTime;
	set.DecayHFRatio = decayHFRatio;
	set.DecayLFRatio = decayLFRatio;
	set.ReflectionsGain = eaxDbToAmp(reflections); // 0.05f;
	set.ReflectionsDelay = reflectionsDelay;
	set.ReflectionsPan[0] = reflectionsPanX;
	set.ReflectionsPan[1] = reflectionsPanY;
	set.ReflectionsPan[2] = reflectionsPanZ;
	set.LateReverbGain = eaxDbToAmp(reverb); //1.26f;
	set.LateReverbDelay = reverbDelay;
	set.LateReverbPan[0] = reverbPanX;
	set.LateReverbPan[1] = reverbPanY;
	set.LateReverbPan[2] = reverbPanZ;
	set.EchoTime = echoTime;
	set.EchoDepth = echoDepth;
	set.ModulationTime = modulationTime;
	set.ModulationDepth = modulationDepth;
	set.AirAbsorptionGainHF = eaxDbToAmp(airAbsorptionHF); //0.995f;
	set.HFReference = hfReference;
	set.LFReference = lfReference;
	set.RoomRolloffFactor = roomRolloffFactor;
	set.DecayHFLimit = 1;
	LoadPreset(set);
}



void ReverbEffect::LoadPreset(ReverbSettings set)
{
	settings=set;
	settings.correct();
}

ReverbEffect::ReverbEffect()
	: mSampleBuffer(nullptr)
	, mTotalSamples(0)
	, mDelayTap{0, 0}
	, mDecoTap{0,0,0}
	, mOffset(0)
{

}

void ReverbEffect::init(quint32 frequency)
{
	ambiCoeffs[0][0] = 0.7071f;
	ambiCoeffs[0][1] = 0.5f;
	ambiCoeffs[0][2] = 0.0f;
	ambiCoeffs[0][3] = 0.0f;

	ambiCoeffs[1][0] = 0.7071f;
	ambiCoeffs[1][1] = -0.5f;
	ambiCoeffs[2][2] = 0.0f;
	ambiCoeffs[3][3] = 0.0f;

	quint32 index=0;

	mTotalSamples = 0;
	mSampleBuffer = NULL;

	mLpFilter.clear();
	mHpFilter.clear();

	mModulator.mDelay.mMask = 0;
	mModulator.mDelay.mLine = NULL;
	mModulator.mIndex = 0;
	mModulator.mRange = 1;
	mModulator.mDepth = 0.0f;
	mModulator.mCoeff = 0.0f;
	mModulator.mFilter = 0.0f;

	mDelay.mMask = 0;
	mDelay.mLine = NULL;
	mDelayTap[0] = 0;
	mDelayTap[1] = 0;

	mEarly.mGain = 0.0f;
	for(index = 0; index < 4; index++) {
		mEarly.mCoeff[index] = 0.0f;
		mEarly.mDelay[index].mMask = 0;
		mEarly.mDelay[index].mLine = NULL;
		mEarly.mOffset[index] = 0;
	}

	mDecorrelator.mMask = 0;
	mDecorrelator.mLine = NULL;
	mDecoTap[0] = 0;
	mDecoTap[1] = 0;
	mDecoTap[2] = 0;

	mLate.mGain = 0.0f;
	mLate.mDensityGain = 0.0f;
	mLate.mApFeedCoeff = 0.0f;
	mLate.mMixCoeff = 0.0f;
	for(index = 0; index < 4; index++) {
		mLate.mApCoeff[index] = 0.0f;
		mLate.mApDelay[index].mMask = 0;
		mLate.mApDelay[index].mLine = NULL;
		mLate.mApOffset[index] = 0;

		mLate.mCoeff[index] = 0.0f;
		mLate.mDelay[index].mMask = 0;
		mLate.mDelay[index].mLine = NULL;
		mLate.mOffset[index] = 0;

		mLate.mLpCoeff[index] = 0.0f;
		mLate.mLpSample[index] = 0.0f;
	}

	for(index = 0; index < OUTPUT_CHANNELS; index++) {
		mEarly.mPanGain[index] = 0.0f;
		mLate.mPanGain[index] = 0.0f;
	}

	mEcho.mDensityGain = 0.0f;
	mEcho.mDelay.mMask = 0;
	mEcho.mDelay.mLine = NULL;
	mEcho.mApDelay.mMask = 0;
	mEcho.mApDelay.mLine = NULL;
	mEcho.mCoeff = 0.0f;
	mEcho.mApFeedCoeff = 0.0f;
	mEcho.mApCoeff = 0.0f;
	mEcho.mOffset = 0;
	mEcho.mApOffset = 0;
	mEcho.mLpCoeff = 0.0f;
	mEcho.mLpSample = 0.0f;
	mEcho.mMixCoeff[0] = 0.0f;
	mEcho.mMixCoeff[1] = 0.0f;

	mOffset = 0;

	settings.Density   = EAXREVERB_DEFAULT_DENSITY;
	settings.Diffusion = EAXREVERB_DEFAULT_DIFFUSION;
	settings.Gain   = EAXREVERB_DEFAULT_GAIN;
	settings.GainHF = EAXREVERB_DEFAULT_GAINHF;
	settings.GainLF = EAXREVERB_DEFAULT_GAINLF;
	settings.DecayTime    = EAXREVERB_DEFAULT_DECAY_TIME;
	settings.DecayHFRatio = EAXREVERB_DEFAULT_DECAY_HFRATIO;
	settings.DecayLFRatio = EAXREVERB_DEFAULT_DECAY_LFRATIO;
	settings.ReflectionsGain   = EAXREVERB_DEFAULT_REFLECTIONS_GAIN;
	settings.ReflectionsDelay  = EAXREVERB_DEFAULT_REFLECTIONS_DELAY;
	settings.ReflectionsPan[0] = EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.ReflectionsPan[1] = EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.ReflectionsPan[2] = EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
	settings.LateReverbGain   = EAXREVERB_DEFAULT_LATE_REVERB_GAIN;
	settings.LateReverbDelay  = EAXREVERB_DEFAULT_LATE_REVERB_DELAY;
	settings.LateReverbPan[0] = EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.LateReverbPan[1] = EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.LateReverbPan[2] = EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
	settings.EchoTime  = EAXREVERB_DEFAULT_ECHO_TIME;
	settings.EchoDepth = EAXREVERB_DEFAULT_ECHO_DEPTH;
	settings.ModulationTime  = EAXREVERB_DEFAULT_MODULATION_TIME;
	settings.ModulationDepth = EAXREVERB_DEFAULT_MODULATION_DEPTH;
	settings.AirAbsorptionGainHF = EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
	settings.HFReference = EAXREVERB_DEFAULT_HFREFERENCE;
	settings.LFReference = EAXREVERB_DEFAULT_LFREFERENCE;
	settings.RoomRolloffFactor = EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
	settings.DecayHFLimit = EAXREVERB_DEFAULT_DECAY_HFLIMIT;

	// Allocate the delay lines.
	allocLines(frequency);

	// Calculate the modulation filter coefficient.  Notice that the exponent
	// is calculated given the current sample rate.  This ensures that the
	// resulting filter response over time is consistent across all sample
	// rates.
	mModulator.mCoeff = powf(MODULATION_FILTER_COEFF,
							 MODULATION_FILTER_CONST / frequency);

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

ReverbEffect::~ReverbEffect()
{
	delete[] mSampleBuffer;
	mSampleBuffer = NULL;
}
