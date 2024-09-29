#ifndef CREEPYVOICE_HPP
#define CREEPYVOICE_HPP

#include "uptime/MethodGate.hpp"
#include "audio/IAudio.hpp"
#include "audio/sources/Voice.hpp"
#include "audio/sources/Ding.hpp"
#include "audio/effects/Tremolo.hpp"
#include "audio/effects/Compressor.hpp"
#include "audio/effects/reverb/Reverb.hpp"
#include "security/PortableID.hpp"


#include <QAudioFormat>
#include <QtGlobal>
#include <cmath>


template <typename T, int OUTPUT_CHANNELS = 2>
class CreepyVoice: public IAudioSource<T, OUTPUT_CHANNELS> {
private:
	Voice<T, OUTPUT_CHANNELS> voice;
	Ding<T, OUTPUT_CHANNELS> ding;
	Tremolo<T, OUTPUT_CHANNELS> tremolo;
	Compressor<T, OUTPUT_CHANNELS>  comp;
	audio::effects::Reverb<T, OUTPUT_CHANNELS> reverb;
	
	RNG *rng;
	T distAmount{1.1};
	
	bool mInited{false};
	
private:
	T frandGauss();
	T frandGaussAbs();
	
public:
	explicit CreepyVoice();
	virtual ~CreepyVoice() override;
	
public:
	bool speak(const PortableID &id, const QString &sentence) const;
	bool voiceIsAvailable();
	
	// IAudioProcessor interface
public:
	void init(QAudioFormat af) override;
	void generate(qint64 num, T *out)override;
	
};


////////////////////////


template <typename T, int OUTPUT_CHANNELS>
CreepyVoice<T, OUTPUT_CHANNELS>::CreepyVoice()
	: rng(RNG::sourceFactory("mt"))
{
	OC_METHODGATE();
}

template <typename T, int OUTPUT_CHANNELS>
CreepyVoice<T, OUTPUT_CHANNELS>::~CreepyVoice(){
	
}



template <typename T, int OUTPUT_CHANNELS>
bool CreepyVoice<T, OUTPUT_CHANNELS>::speak(const PortableID &id, const QString &text) const 
{
	OC_METHODGATE();
	return voice.speak(id, text);
}


template <typename T, int OUTPUT_CHANNELS>
bool CreepyVoice<T, OUTPUT_CHANNELS>::voiceIsAvailable()
{
	OC_FUNCTIONGATE();
	return voice.voiceIsAvailable();
}




template <typename T, int OUTPUT_CHANNELS>
T CreepyVoice<T, OUTPUT_CHANNELS>::frandGauss()
{
	OC_METHODGATE();
	return rng->generateGauss();
}

template <typename T, int OUTPUT_CHANNELS>
T CreepyVoice<T, OUTPUT_CHANNELS>::frandGaussAbs()
{
	OC_METHODGATE();
	return qAbs(rng->generateGauss());
}


template <typename T, int OUTPUT_CHANNELS>
void CreepyVoice<T, OUTPUT_CHANNELS>::init(QAudioFormat af)
{
	OC_METHODGATE();
	voice.init(af);
	ding.init(af);
	tremolo.init(af);
	tremolo.set(frandGaussAbs()*25.0, 0.00001+frandGaussAbs()*0.00018);
	
	comp.set(0.5+frandGauss()*0.2, 0.5+frandGauss()*0.2, (3.0+frandGauss()*0.2)/1000.0, (1.0+frandGauss()*0.2)/1000.0, (0.1+frandGauss()*0.2)/1000.0, (300.0+frandGauss()*50.0)/1000.0);
	comp.init(af);
	reverb.init(af);
	
	audio::effects::reverb::Settings<T> s;
	s.set(
				static_cast<int>(25.0+frandGauss()*100)
				, 8.3+frandGauss()*20.0
				, 0.890+frandGauss()*0.3
				, static_cast<int>(-1000.0+frandGauss()*100.0)
				, static_cast<int>(-200.0-frandGauss()*100.0)
				, static_cast<int>(-2000.0+frandGauss()*600.0)
				, 1.22+frandGauss()*0.2
				, 0.83+frandGauss()*0.1
				, 0.31+frandGauss()*0.1
				, static_cast<int>(-100.0-frandGauss()*10.0)
				, 0.022+frandGauss()*0.002
				, 0.0+frandGaussAbs()
				, 0.0+frandGaussAbs()
				, 0.0+frandGaussAbs()
				, static_cast<int>(600.0+frandGauss()*100.0)
				, 0.011+frandGauss()*0.001
				, 0.0+frandGaussAbs()
				, 0.0+frandGaussAbs()
				, 0.0+frandGaussAbs()
				, 0.138+frandGauss()*0.01
				, 0.080+frandGauss()*0.01
				, 0.250+frandGauss()*0.1
				, 0.0+frandGaussAbs()
				, -5.0-frandGauss()*0.1
				, 5168.6+frandGauss()*500
				, 139.5+frandGauss()*13.0
				, 0.0+frandGaussAbs()
				//Castle
				//26.0f, 8.3f, 0.890f, -1000.0f, -800.0f, -2000.0f, 1.22f, 0.83f, 0.31f, -100.0f, 0.022f, 0.0f, 0.0f, 0.0f, 600.0f, 0.011f, 0.0f, 0.0f, 0.0f, 0.138f, 0.080f, 0.250f, 0.0f, -5.0f, 5168.6f, 139.5f, 0.0f, 0x20
				//stadium
				//26, 7.2f, 1.0f, -1000, -700, -200, 6.26f, 0.51f, 1.10f, -2400, 0.183f, 0.0f, 0.0f, 0.0f, -800, 0.038f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 0.250f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x20;
				// heaven
				//26, 19.6f, 0.940f, -1000, -200, -700, 5.04f, 1.12f, 0.56f, -1230, 0.020f, 0.0f, 0.0f, 0.0f, 200, 0.029f, 0.0f, 0.0f, 0.0f, 0.250f, 0.080f, 2.742f, 0.050f, -2.0f, 5000.0f, 250.0f, 0.0f, 0x3f;
				//sewer
				//21, 1.7f, 0.800f, -1000, -1000, 0, 2.81f, 0.14f, 1.0f, 429, 0.014f, 0.0f, 0.0f, 0.0f, 1023, 0.021f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 0.250f, 0.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x3f;
				//Crazy
				//25, 1.0f, 0.500f, -1000, -151, 0, 7.56f, 0.91f, 1.0f, -626, 0.020f, 0.0f, 0.0f, 0.0f, 774, 0.030f, 0.0f, 0.0f, 0.0f, 0.250f, 0.0f, 4.0f, 1.0f, -5.0f, 5000.0f, 250.0f, 0.0f, 0x1f;
				, 0x20);
	
	
	reverb.loadPreset(s);
	reverb.update(af);
	
	ding.trigger();
	mInited=true;
}


template <typename T, int OUTPUT_CHANNELS>
void CreepyVoice<T, OUTPUT_CHANNELS>::generate(qint64 num, T *out) {
	OC_METHODGATE();
	voice.generate(num, out);
	reverb.process(num, out, out);
	tremolo.process(num, out, out);
	comp.process(num, out, out);
	for(auto i=0; i<num; ++i){
		auto sam=out[i] * 0.8;
		if (sam > 0) {
			sam = 1 - exp(-sam*distAmount);
		} else {
			sam = -1 + exp(sam*distAmount);
		}
		out[i]=sam;
	}
}



#endif // CREEPYVOICE_HPP
