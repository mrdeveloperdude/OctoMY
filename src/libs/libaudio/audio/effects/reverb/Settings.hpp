#ifndef SETTINGS_H
#define SETTINGS_H

#include "uptime/MethodGate.hpp"
#include "random/RNG.hpp"

#include "Utils.hpp"


#define BOOL int


// Effect parameter ranges and defaults.
#define REVERB_DENSITY_MIN                 (0.0)
#define REVERB_DENSITY_MAX                 (1.0)
#define REVERB_DENSITY_DEFAULT             (1.0)

#define REVERB_DIFFUSION_MIN               (0.0)
#define REVERB_DIFFUSION_MAX               (1.0)
#define REVERB_DIFFUSION_DEFAULT           (1.0)

#define REVERB_GAIN_MIN                    (0.0)
#define REVERB_GAIN_MAX                    (1.0)
#define REVERB_DEFAULT_GAIN                (0.32)

#define REVERB_MIN_GAINHF                  (0.0)
#define REVERB_MAX_GAINHF                  (1.0)
#define REVERB_DEFAULT_GAINHF              (0.89)

#define REVERB_MIN_GAINLF                  (0.0)
#define REVERB_MAX_GAINLF                  (1.0)
#define REVERB_DEFAULT_GAINLF              (1.0)

#define REVERB_MIN_DECAY_TIME              (0.1)
#define REVERB_MAX_DECAY_TIME              (20.0)
#define REVERB_DEFAULT_DECAY_TIME          (1.49)

#define REVERB_MIN_DECAY_HFRATIO           (0.1)
#define REVERB_MAX_DECAY_HFRATIO           (2.0)
#define REVERB_DEFAULT_DECAY_HFRATIO       (0.83)

#define REVERB_MIN_DECAY_LFRATIO           (0.1)
#define REVERB_MAX_DECAY_LFRATIO           (2.0)
#define REVERB_DEFAULT_DECAY_LFRATIO       (1.0)

#define REVERB_MIN_REFLECTIONS_GAIN        (0.0)
#define REVERB_MAX_REFLECTIONS_GAIN        (3.16)
#define REVERB_DEFAULT_REFLECTIONS_GAIN    (0.05)

#define REVERB_MIN_REFLECTIONS_DELAY       (0.0)
#define REVERB_MAX_REFLECTIONS_DELAY       (0.3)
#define REVERB_DEFAULT_REFLECTIONS_DELAY   (0.007)


#define REVERB_MIN_REFLECTIONS_PAN_XYZ (-1.0)
#define REVERB_MAX_REFLECTIONS_PAN_XYZ (1.0)
#define REVERB_DEFAULT_REFLECTIONS_PAN_XYZ (0.0)

#define REVERB_MIN_LATE_REVERB_GAIN        (0.0)
#define REVERB_MAX_LATE_REVERB_GAIN        (10.0)
#define REVERB_DEFAULT_LATE_REVERB_GAIN    (1.26)

#define REVERB_MIN_LATE_REVERB_DELAY       (0.0)
#define REVERB_MAX_LATE_REVERB_DELAY       (0.1)
#define REVERB_DEFAULT_LATE_REVERB_DELAY   (0.011)



#define REVERB_MIN_LATE_REVERB_PAN_XYZ (-1.0)
#define REVERB_MAX_LATE_REVERB_PAN_XYZ (1.0)
#define REVERB_DEFAULT_LATE_REVERB_PAN_XYZ (0.0)

#define REVERB_MIN_ECHO_TIME               (0.075)
#define REVERB_MAX_ECHO_TIME               (0.25)
#define REVERB_DEFAULT_ECHO_TIME           (0.25)

#define REVERB_MIN_ECHO_DEPTH              (0.0)
#define REVERB_MAX_ECHO_DEPTH              (1.0)
#define REVERB_DEFAULT_ECHO_DEPTH          (0.0)

#define REVERB_MIN_MODULATION_TIME         (0.04)
#define REVERB_MAX_MODULATION_TIME         (4.0)
#define REVERB_DEFAULT_MODULATION_TIME     (0.25)

#define REVERB_MIN_MODULATION_DEPTH        (0.0)
#define REVERB_MAX_MODULATION_DEPTH        (1.0)
#define REVERB_DEFAULT_MODULATION_DEPTH    (0.0)

#define REVERB_MIN_AIR_ABSORPTION_GAINHF   (0.892)
#define REVERB_MAX_AIR_ABSORPTION_GAINHF   (1.0)
#define REVERB_DEFAULT_AIR_ABSORPTION_GAINHF (0.994)

#define REVERB_MIN_HFREFERENCE             (1000.0)
#define REVERB_MAX_HFREFERENCE             (20000.0)
#define REVERB_DEFAULT_HFREFERENCE         (5000.0)

#define REVERB_MIN_LFREFERENCE             (20.0)
#define REVERB_MAX_LFREFERENCE             (1000.0)
#define REVERB_DEFAULT_LFREFERENCE         (250.0)

#define REVERB_MIN_ROOM_ROLLOFF_FACTOR     (0.0)
#define REVERB_MAX_ROOM_ROLLOFF_FACTOR     (10.0)
#define REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR (0.0)

#define REVERB_MIN_DECAY_HFLIMIT           false
#define REVERB_MAX_DECAY_HFLIMIT           true
#define REVERB_DEFAULT_DECAY_HFLIMIT       true


namespace audio{

namespace effects{
namespace reverb{

template <typename T>
struct Settings {
	
public:
	// Shared Reverb Properties
	T Density;
	T Diffusion;
	T Gain;
	T GainHF;
	T DecayTime;
	T DecayHFRatio;
	T ReflectionsGain;
	T ReflectionsDelay;
	T LateReverbGain;
	T LateReverbDelay;
	T AirAbsorptionGainHF;
	T RoomRolloffFactor;
	BOOL DecayHFLimit;
	
	// Additional EAX Reverb Properties
	T GainLF;
	T DecayLFRatio;
	T ReflectionsPan[3];
	T LateReverbPan[3];
	T EchoTime;
	T EchoDepth;
	T ModulationTime;
	T ModulationDepth;
	T HFReference;
	T LFReference;
	
public:
	
	Settings()
		: Density(REVERB_DENSITY_DEFAULT)
		, Diffusion(REVERB_DIFFUSION_DEFAULT)
		, Gain(REVERB_DEFAULT_GAIN)
		, GainHF(REVERB_DEFAULT_GAINHF)
		, DecayTime(REVERB_DEFAULT_DECAY_TIME)
		, DecayHFRatio(REVERB_DEFAULT_DECAY_HFRATIO)
		, ReflectionsGain(REVERB_DEFAULT_REFLECTIONS_GAIN)
		, ReflectionsDelay(REVERB_DEFAULT_REFLECTIONS_DELAY)
		, LateReverbGain(REVERB_DEFAULT_LATE_REVERB_GAIN)
		, LateReverbDelay(REVERB_DEFAULT_LATE_REVERB_DELAY)
		, AirAbsorptionGainHF(REVERB_DEFAULT_AIR_ABSORPTION_GAINHF)
		, RoomRolloffFactor(REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR)
		, DecayHFLimit(REVERB_DEFAULT_DECAY_HFLIMIT)
		, GainLF(REVERB_DEFAULT_GAINLF)
		, DecayLFRatio(REVERB_DEFAULT_DECAY_LFRATIO)
		, ReflectionsPan{0,0,0}
		, LateReverbPan{0,0,0}
		, EchoTime(REVERB_DEFAULT_ECHO_TIME)
		, EchoDepth(REVERB_DEFAULT_ECHO_DEPTH)
		, ModulationTime(REVERB_DEFAULT_MODULATION_TIME)
		, ModulationDepth(REVERB_DEFAULT_MODULATION_DEPTH)
		, HFReference(REVERB_DEFAULT_HFREFERENCE)
		, LFReference(REVERB_DEFAULT_LFREFERENCE)
	{
		
	}
	
	void set(
			int environment
			, T environmentSize
			, T environmentDiffusion
			, int room
			, int roomHF
			, int roomLF
			, T decayTime
			, T decayHFRatio
			, T decayLFRatio
			, int reflections
			, T reflectionsDelay
			, T reflectionsPanX
			, T reflectionsPanY
			, T reflectionsPanZ
			, int reverb
			, T reverbDelay
			, T reverbPanX
			, T reverbPanY
			, T reverbPanZ
			, T echoTime
			, T echoDepth
			, T modulationTime
			, T modulationDepth
			, T airAbsorptionHF
			, T hfReference
			, T lfReference
			, T roomRolloffFactor
			, int flags)
	
	{
		OC_METHODGATE();
		Q_UNUSED(environment);
		Q_UNUSED(environmentSize);
		Q_UNUSED(flags);
		Density = 1.0f; // todo, currently default
		Diffusion = environmentDiffusion;
		Gain =  eaxDbToAmp(room); //0.32f;
		GainHF = eaxDbToAmp(roomHF); //0.89f;
		GainLF = eaxDbToAmp(roomLF); // 1.0f;
		DecayTime = decayTime;
		DecayHFRatio = decayHFRatio;
		DecayLFRatio = decayLFRatio;
		ReflectionsGain = eaxDbToAmp(reflections); // 0.05f;
		ReflectionsDelay = reflectionsDelay;
		ReflectionsPan[0] = reflectionsPanX;
		ReflectionsPan[1] = reflectionsPanY;
		ReflectionsPan[2] = reflectionsPanZ;
		LateReverbGain = eaxDbToAmp(reverb); //1.26f;
		LateReverbDelay = reverbDelay;
		LateReverbPan[0] = reverbPanX;
		LateReverbPan[1] = reverbPanY;
		LateReverbPan[2] = reverbPanZ;
		EchoTime = echoTime;
		EchoDepth = echoDepth;
		ModulationTime = modulationTime;
		ModulationDepth = modulationDepth;
		AirAbsorptionGainHF = eaxDbToAmp(airAbsorptionHF); //0.995f;
		HFReference = hfReference;
		LFReference = lfReference;
		RoomRolloffFactor = roomRolloffFactor;
		DecayHFLimit = 1;
	}
	
	void correct()
	{
		Density=qBound(REVERB_DENSITY_MIN,Density,REVERB_DENSITY_MAX);
		Diffusion=qBound(REVERB_DIFFUSION_MIN,Diffusion,REVERB_DIFFUSION_MAX);
		Gain=qBound(REVERB_GAIN_MIN,Gain,REVERB_GAIN_MAX);
		GainHF=qBound(REVERB_MIN_GAINHF,GainHF,REVERB_MAX_GAINHF);
		GainLF=qBound(REVERB_MIN_GAINLF,GainLF,REVERB_MAX_GAINLF);
		DecayTime=qBound(REVERB_MIN_DECAY_TIME,DecayTime,REVERB_MAX_DECAY_TIME);
		DecayHFRatio=qBound(REVERB_MIN_DECAY_HFRATIO,DecayHFRatio,REVERB_MAX_DECAY_HFRATIO);
		DecayLFRatio=qBound(REVERB_MIN_DECAY_LFRATIO,DecayLFRatio,REVERB_MAX_DECAY_LFRATIO);
		ReflectionsGain=qBound(REVERB_MIN_REFLECTIONS_GAIN,ReflectionsGain,REVERB_MAX_REFLECTIONS_GAIN);
		ReflectionsDelay=qBound(REVERB_MIN_REFLECTIONS_DELAY,ReflectionsDelay,REVERB_MAX_REFLECTIONS_DELAY);
		
		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=qBound(REVERB_MIN_REFLECTIONS_PAN_XYZ,ReflectionsPan[i],REVERB_MAX_REFLECTIONS_PAN_XYZ);
			LateReverbPan[i]=qBound(REVERB_MIN_LATE_REVERB_PAN_XYZ,LateReverbPan[i],REVERB_MAX_LATE_REVERB_PAN_XYZ);
		}
		
		LateReverbGain=qBound(REVERB_MIN_LATE_REVERB_GAIN,LateReverbGain,REVERB_MAX_LATE_REVERB_GAIN);
		LateReverbDelay=qBound(REVERB_MIN_LATE_REVERB_DELAY,LateReverbDelay,REVERB_MAX_LATE_REVERB_DELAY);
		EchoTime=qBound(REVERB_MIN_ECHO_TIME,EchoTime,REVERB_MAX_ECHO_TIME);
		EchoDepth=qBound(REVERB_MIN_ECHO_DEPTH,EchoDepth,REVERB_MAX_ECHO_DEPTH);
		ModulationTime=qBound(REVERB_MIN_MODULATION_TIME,ModulationTime,REVERB_MAX_MODULATION_TIME);
		ModulationDepth=qBound(REVERB_MIN_MODULATION_DEPTH,ModulationDepth,REVERB_MAX_MODULATION_DEPTH);
		AirAbsorptionGainHF=qBound(REVERB_MIN_AIR_ABSORPTION_GAINHF,AirAbsorptionGainHF,REVERB_MAX_AIR_ABSORPTION_GAINHF);
		HFReference=qBound(REVERB_MIN_HFREFERENCE,HFReference,REVERB_MAX_HFREFERENCE);
		LFReference=qBound(REVERB_MIN_LFREFERENCE,LFReference,REVERB_MAX_LFREFERENCE);
		RoomRolloffFactor=qBound(REVERB_MIN_ROOM_ROLLOFF_FACTOR,RoomRolloffFactor,REVERB_MAX_ROOM_ROLLOFF_FACTOR);
		//DecayHFLimit=qBound(REVERB_MIN_DECAY_HFLIMIT,DecayHFLimit,REVERB_MAX_DECAY_HFLIMIT);
	}
	
	void reset()
	{
		
		Density=REVERB_DENSITY_DEFAULT;
		Diffusion=REVERB_DIFFUSION_DEFAULT;
		Gain=REVERB_DEFAULT_GAIN;
		GainHF=REVERB_DEFAULT_GAINHF;
		GainLF=REVERB_DEFAULT_GAINLF;
		DecayTime=REVERB_DEFAULT_DECAY_TIME;
		DecayHFRatio=REVERB_DEFAULT_DECAY_HFRATIO;
		DecayLFRatio=REVERB_DEFAULT_DECAY_LFRATIO;
		ReflectionsGain=REVERB_DEFAULT_REFLECTIONS_GAIN;
		ReflectionsDelay=REVERB_DEFAULT_REFLECTIONS_DELAY;
		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=REVERB_DEFAULT_REFLECTIONS_PAN_XYZ;
			LateReverbPan[i]=REVERB_DEFAULT_LATE_REVERB_PAN_XYZ;
		}
		LateReverbGain=REVERB_DEFAULT_LATE_REVERB_GAIN;
		LateReverbDelay=REVERB_DEFAULT_LATE_REVERB_DELAY;
		EchoTime=REVERB_DEFAULT_ECHO_TIME;
		EchoDepth=REVERB_DEFAULT_ECHO_DEPTH;
		ModulationTime=REVERB_DEFAULT_MODULATION_TIME;
		ModulationDepth=REVERB_DEFAULT_MODULATION_DEPTH;
		AirAbsorptionGainHF=REVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
		HFReference=REVERB_DEFAULT_HFREFERENCE;
		LFReference=REVERB_DEFAULT_LFREFERENCE;
		RoomRolloffFactor=REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
		DecayHFLimit=REVERB_DEFAULT_DECAY_HFLIMIT;
	}
	
	T frandParam(RNG *rng, T min, T def, T max)
	{
		Q_UNUSED(rng);
		Q_UNUSED(min);
		Q_UNUSED(max);
		// TODO: Implement this
		return def;
	}
	
	void randomize(RNG *rng)
	{
		Density=frandParam(rng, REVERB_DENSITY_MIN,REVERB_DENSITY_DEFAULT,REVERB_DENSITY_MAX);
		Diffusion=frandParam(rng, REVERB_DIFFUSION_MIN,REVERB_DIFFUSION_DEFAULT,REVERB_DIFFUSION_MAX);
		Gain=frandParam(rng, REVERB_GAIN_MIN,REVERB_DEFAULT_GAIN,REVERB_GAIN_MAX);
		GainHF=frandParam(rng, REVERB_MIN_GAINHF,REVERB_DEFAULT_GAINHF,REVERB_MAX_GAINHF);
		GainLF=frandParam(rng, REVERB_MIN_GAINLF,REVERB_DEFAULT_GAINLF,REVERB_MAX_GAINLF);
		DecayTime=frandParam(rng, REVERB_MIN_DECAY_TIME,REVERB_DEFAULT_DECAY_TIME,REVERB_MAX_DECAY_TIME);
		DecayHFRatio=frandParam(rng, REVERB_MIN_DECAY_HFRATIO,REVERB_DEFAULT_DECAY_HFRATIO,REVERB_MAX_DECAY_HFRATIO);
		DecayLFRatio=frandParam(rng, REVERB_MIN_DECAY_LFRATIO,REVERB_DEFAULT_DECAY_LFRATIO,REVERB_MAX_DECAY_LFRATIO);
		ReflectionsGain=frandParam(rng, REVERB_MIN_REFLECTIONS_GAIN,REVERB_DEFAULT_REFLECTIONS_GAIN,REVERB_MAX_REFLECTIONS_GAIN);
		ReflectionsDelay=frandParam(rng, REVERB_MIN_REFLECTIONS_DELAY,REVERB_DEFAULT_REFLECTIONS_DELAY,REVERB_MAX_REFLECTIONS_DELAY);
		for(int i=0; i<3; ++i) {
			ReflectionsPan[i]=frandParam(rng, REVERB_MIN_REFLECTIONS_PAN_XYZ,REVERB_DEFAULT_REFLECTIONS_PAN_XYZ,REVERB_MAX_REFLECTIONS_PAN_XYZ);
			LateReverbPan[i]=frandParam(rng, REVERB_MIN_LATE_REVERB_PAN_XYZ,REVERB_DEFAULT_LATE_REVERB_PAN_XYZ,REVERB_MAX_LATE_REVERB_PAN_XYZ);
		}
		LateReverbGain=frandParam(rng, REVERB_MIN_LATE_REVERB_GAIN,REVERB_DEFAULT_LATE_REVERB_GAIN,REVERB_MAX_LATE_REVERB_GAIN);
		LateReverbDelay=frandParam(rng, REVERB_MIN_LATE_REVERB_DELAY,REVERB_DEFAULT_LATE_REVERB_DELAY,REVERB_MAX_LATE_REVERB_DELAY);
		EchoTime=frandParam(rng, REVERB_MIN_ECHO_TIME,REVERB_DEFAULT_ECHO_TIME,REVERB_MAX_ECHO_TIME);
		EchoDepth=frandParam(rng, REVERB_MIN_ECHO_DEPTH,REVERB_DEFAULT_ECHO_DEPTH,REVERB_MAX_ECHO_DEPTH);
		ModulationTime=frandParam(rng, REVERB_MIN_MODULATION_TIME,REVERB_DEFAULT_MODULATION_TIME,REVERB_MAX_MODULATION_TIME);
		ModulationDepth=frandParam(rng, REVERB_MIN_MODULATION_DEPTH,REVERB_DEFAULT_MODULATION_DEPTH,REVERB_MAX_MODULATION_DEPTH);
		AirAbsorptionGainHF=frandParam(rng, REVERB_MIN_AIR_ABSORPTION_GAINHF,REVERB_DEFAULT_AIR_ABSORPTION_GAINHF,REVERB_MAX_AIR_ABSORPTION_GAINHF);
		HFReference=frandParam(rng, REVERB_MIN_HFREFERENCE,REVERB_DEFAULT_HFREFERENCE,REVERB_MAX_HFREFERENCE);
		LFReference=frandParam(rng, REVERB_MIN_LFREFERENCE,REVERB_DEFAULT_LFREFERENCE,REVERB_MAX_LFREFERENCE);
		RoomRolloffFactor=frandParam(rng, REVERB_MIN_ROOM_ROLLOFF_FACTOR,REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR,REVERB_MAX_ROOM_ROLLOFF_FACTOR);
		DecayHFLimit=rng->generateReal1()>0.5;
	}
	
	void defaults()
	{
		
	}
	
} ;


}

}
}


#endif // SETTINGS_H
