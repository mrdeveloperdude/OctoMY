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

typedef enum FilterType {
	/** EFX-style low-pass filter, specifying a gain and reference frequency. */
	Filter_HighShelf,
	/** EFX-style high-pass filter, specifying a gain and reference frequency. */
	Filter_LowShelf,
} FilterType;

typedef struct {
	// The delay lines use sample lengths that are powers of 2 to allow the
	// use of bit-masking instead of a modulus for wrapping.
	uint32_t   Mask;
	double *Line;
} DelayLine;

typedef struct {
	double x[2]; /* History of two last input samples  */
	double y[2]; /* History of two last output samples */
	double a[3]; /* Transfer function coefficients "a" */
	double b[3]; /* Transfer function coefficients "b" */
} FilterState;


struct ReverbSettings {
	// Shared Reverb Properties
	double Density;
	double Diffusion;
	double Gain;
	double GainHF;
	double DecayTime;
	double DecayHFRatio;
	double ReflectionsGain;
	double ReflectionsDelay;
	double LateReverbGain;
	double LateReverbDelay;
	double AirAbsorptionGainHF;
	double RoomRolloffFactor;
	BOOL DecayHFLimit;

	// Additional EAX Reverb Properties
	double GainLF;
	double DecayLFRatio;
	double ReflectionsPan[3];
	double LateReverbPan[3];
	double EchoTime;
	double EchoDepth;
	double ModulationTime;
	double ModulationDepth;
	double HFReference;
	double LFReference;

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

	double frandParam(RNG *rng, double min, double def, double max)
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

typedef struct {
	// Modulator delay line.
	DelayLine Delay;

	// The vibrato time is tracked with an index over a modulus-wrapped
	// range (in samples).
	uint32_t    Index;
	uint32_t    Range;

	// The depth of frequency change (also in samples) and its filter.
	double   Depth;
	double   Coeff;
	double   Filter;
} Modulator;

typedef struct {
	// Output gain for early reflections.
	double   Gain;

	// Early reflections are done with 4 delay lines.
	double   Coeff[4];
	DelayLine Delay[4];
	uint32_t    Offset[4];

	// The gain for each output channel based on 3D panning (only for the
	// EAX path).
	double   PanGain[OUTPUT_CHANNELS];
} EarlyDelay;

typedef struct {
	// Output gain for late reverb.
	double   Gain;

	// Attenuation to compensate for the modal density and decay rate of
	// the late lines.
	double   DensityGain;

	// The feed-back and feed-forward all-pass coefficient.
	double   ApFeedCoeff;

	// Mixing matrix coefficient.
	double   MixCoeff;

	// Late reverb has 4 parallel all-pass filters.
	double   ApCoeff[4];
	DelayLine ApDelay[4];
	uint32_t    ApOffset[4];

	// In addition to 4 cyclical delay lines.
	double   Coeff[4];
	DelayLine Delay[4];
	uint32_t    Offset[4];

	// The cyclical delay lines are 1-pole low-pass filtered.
	double   LpCoeff[4];
	double   LpSample[4];

	// The gain for each output channel based on 3D panning (only for the
	// EAX path).
	double   PanGain[OUTPUT_CHANNELS];
} LateDelay;

typedef struct {
	// Attenuation to compensate for the modal density and decay rate of
	// the echo line.
	double   DensityGain;

	// Echo delay and all-pass lines.
	DelayLine Delay;
	DelayLine ApDelay;

	double   Coeff;
	double   ApFeedCoeff;
	double   ApCoeff;

	uint32_t    Offset;
	uint32_t    ApOffset;

	// The echo line is 1-pole low-pass filtered.
	double   LpCoeff;
	double   LpSample;

	// Echo mixing coefficients.
	double   MixCoeff[2];
} ReverbEcho;

class ReverbEffect
{
public:
	ReverbEffect();
	virtual ~ReverbEffect();

	void init(uint32_t frequency);

	void Process(uint32_t SamplesToDo, const double *SamplesIn, double *SamplesOut);
	void Update(int frequency);

	void LoadPreset(int environment, double environmentSize, double environmentDiffusion, int room, int roomHF, int roomLF,
					double decayTime, double decayHFRatio, double decayLFRatio,
					int reflections, double reflectionsDelay, double reflectionsPanX, double reflectionsPanY, double reflectionsPanZ,
					int reverb, double reverbDelay, double reverbPanX, double reverbPanY, double reverbPanZ,
					double echoTime, double echoDepth, double modulationTime, double modulationDepth, double airAbsorptionHF,
					double hfReference, double lfReference, double roomRolloffFactor, int flags);



	void LoadPreset(ReverbSettings);




private:
	ReverbSettings settings;

	// All delay lines are allocated as a single buffer to reduce memory
	// fragmentation and management code.
	double  *SampleBuffer;
	uint32_t    TotalSamples;

	// Master effect filters
	FilterState LpFilter;
	FilterState HpFilter; // EAX only

	Modulator Mod;

	// Initial effect delay.
	DelayLine Delay;
	// The tap points for the initial delay.  First tap goes to early
	// reflections, the last to late reverb.
	uint32_t    DelayTap[2];

	EarlyDelay Early;

	// Decorrelator delay line.
	DelayLine Decorrelator;
	// There are actually 4 decorrelator taps, but the first occurs at the
	// initial sample.
	uint32_t    DecoTap[3];

	LateDelay Late;

	ReverbEcho Echo;

	// The current read offset for all delay lines.
	uint32_t Offset;

	/* Temporary storage used when processing, before deinterlacing. */
	double ReverbSamples[REVERB_BUFFERSIZE][4];
	double EarlySamples[REVERB_BUFFERSIZE][4];

	double ambiCoeffs[MAX_AMBI_COEFFS][MAX_AMBI_COEFFS];

	void ComputeAmbientGains(double ingain, double gains[OUTPUT_CHANNELS]);
	void ComputeDirectionalGains(const double dir[3], double ingain, double gains[OUTPUT_CHANNELS]);

	void AllocLines(uint32_t frequency);

	inline double EAXModulation(double in);

	inline double EarlyDelayLineOut(uint32_t index);
	inline void EarlyReflection(double in, double *out);

	inline double LateAllPassInOut(uint32_t index, double in);
	inline double LateDelayLineOut(uint32_t index);

	inline double LateLowPassInOut(uint32_t index, double in);
	inline void LateReverb(const double *in, double *out);

	inline void EAXEcho(double in, double *late);
	inline void EAXVerbPass(double in, double *early, double *late);

	void UpdateDelayLine(double earlyDelay, double lateDelay, uint32_t frequency);
	void UpdateModulator(double modTime, double modDepth, uint32_t frequency);
	void UpdateEarlyLines(double reverbGain, double earlyGain, double lateDelay);
	void UpdateDecorrelator(double density, uint32_t frequency);
	void UpdateLateLines(double reverbGain, double lateGain, double xMix, double density, double decayTime, double diffusion, double hfRatio, double cw, uint32_t frequency);
	void UpdateEchoLine(double reverbGain, double lateGain, double echoTime, double decayTime, double diffusion, double echoDepth, double hfRatio, double cw, uint32_t frequency);

	void Update3DPanning(const double *ReflectionsPan, const double *LateReverbPan, double Gain);
};


#endif // REVERB_HPP
