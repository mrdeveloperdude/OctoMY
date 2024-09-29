#ifndef DING_HPP
#define DING_HPP

#include "uptime/MethodGate.hpp"
#include "audio/IAudio.hpp"
#include "audio/sources/Osc.hpp"
#include "audio/effects/Biquad.hpp"
#include "audio/effects/Tremolo.hpp"

#include <QAudioFormat>
#include <QtGlobal>
#include <cmath>


template <typename T, int OUTPUT_CHANNELS = 2>
class Ding: public IAudioSource<T, OUTPUT_CHANNELS> {
private:
	Osc<T, OUTPUT_CHANNELS> o;
	Tremolo<T, OUTPUT_CHANNELS> tremo;
	Biquad<T, OUTPUT_CHANNELS> b1;
	T g;
	T g2;
	//	T trem;
	T damp;
	T damp2;
	T secOfs;
	T step;
	
public:
	explicit Ding(T hz=100,T sr=44100,T amp=1,T trem=0.1,T trema=12,T damp=0.99998);
	virtual ~Ding() override;
	
public:
	inline 	void trigger(T secOfs=0.0);
	inline void set(T hz=100,T sr=44100,T amp=1,T trem=0.1,T trema=12,T damp=0.99998);
	inline T generate();
	
	//IAudioSource interface
public:
	void init(QAudioFormat f) override;
	void generate(qint64 num, T *out) override;
};


template <typename T, int OUTPUT_CHANNELS>
Ding<T, OUTPUT_CHANNELS>::Ding(T hz,T sr,T amp,T trem,T trema,T damp):
	o(hz, sr, amp, Osc<T, OUTPUT_CHANNELS>::SQR)
  , tremo()
  , g(0.0f)
  , g2(0.0f)
  , damp(damp)
  , damp2(damp*0.9)
  , secOfs(-1)
  , step(1.09/sr)
{
	OC_METHODGATE();
	b1.setBiquad(Biquad<T, OUTPUT_CHANNELS>::BANDPASS, hz/sr, 100.0, 6.0);
	//tremo.set(hz/trema, trema); // hz/trema, sr, trem, Osc<T, OUTPUT_CHANNELS>::SIN
}


template <typename T, int OUTPUT_CHANNELS>
Ding<T, OUTPUT_CHANNELS>::~Ding(){
	OC_METHODGATE();
}


template <typename T, int OUTPUT_CHANNELS>
void Ding<T, OUTPUT_CHANNELS>::trigger(T secOfs){
	OC_METHODGATE();
	this->secOfs=secOfs;
}

template <typename T, int OUTPUT_CHANNELS>
void Ding<T, OUTPUT_CHANNELS>::set(T hz,T sr,T amp,T trem,T trema,T damp){
	OC_METHODGATE();
	b1.setFc(hz/sr);
	this->damp = damp;
	this->damp2 = damp * 0.9;
	this->o.set(hz, sr, amp);
	this->tremo.set(hz/trema, sr, trem);
}

template <typename T, int OUTPUT_CHANNELS>
T Ding<T, OUTPUT_CHANNELS>::generate(){
	OC_METHODGATE();
	if(secOfs >= 0.0){
		secOfs -= step;
		if(secOfs < 0.0){
			g = 1.0f;
		}
	}
	g2 = g2 * damp2 + g * (1.0 - damp2);
	g = g * damp;
	o.offset(tremo.process(g));
	return b1.process( o.generate() * g2);
}




template <typename T, int OUTPUT_CHANNELS>
void Ding<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	//sr = f.sampleRate();
}

template <typename T, int OUTPUT_CHANNELS>
void Ding<T, OUTPUT_CHANNELS>::generate(qint64 num, T *out) {
	OC_METHODGATE();
	for(auto i=0; i<num; ++i){
		out[i]=generate();
	}
}



#endif
// DING_HPP
