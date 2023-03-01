#ifndef OSC_HPP
#define OSC_HPP

#include "uptime/MethodGate.hpp"
#include "audio/IAudio.hpp"

#include <QAudioFormat>
#include <QtGlobal>

template <typename T, int OUTPUT_CHANNELS = 2>
class Osc: public IAudioSource<T, OUTPUT_CHANNELS> {
public:
	enum Waveform{
		SIN,SAW,TRI,SQR
	};
private:
	T hz;
	T offhz;
	T sr;
	T ang;
	T amp;
	Waveform w;
	
public:
	explicit Osc(T hz=100,T sr=44100,T amp=1,Waveform w=SQR);
	inline	void set(T hz=100,T sr=44100,T amp=1,Waveform w=SQR);
	inline	void offset(T v);
	inline T wav(T a);
	inline	T generate();
	
	// IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void generate(qint64 num, T *out)Q_DECL_OVERRIDE;
	
};



template <typename T, int OUTPUT_CHANNELS>
Osc<T, OUTPUT_CHANNELS>::Osc(T hz,T sr,T amp,Waveform w):
	hz(hz)
  ,offhz(0.0)
  ,sr(sr)
  ,ang(0.0f)
  ,amp(amp)
  ,w(w)
{
	
}

template <typename T, int OUTPUT_CHANNELS>
void Osc<T, OUTPUT_CHANNELS>::set(T hz,T sr,T amp,Waveform w){
	this->hz=hz;
	this->sr=sr;
	this->amp=amp;
	this->w=w;
}

template <typename T, int OUTPUT_CHANNELS>
void Osc<T, OUTPUT_CHANNELS>::offset(T v){
	this->offhz=v;
}

template <typename T, int OUTPUT_CHANNELS>
T Osc<T, OUTPUT_CHANNELS>::wav(T a){
	switch(w){
	case(SQR):{
		a/=M_PI;
		return a>1.0?1.0:-1.0;
	}break;
	case(TRI):{
		a/=M_PI;
		return a>1.0?(1.0-(a-1.0)):a-1.0;
	}break;
	case(SAW):{
		a/=M_PI;
		return a-1.0;
	}break;
	case(SIN):{
		return sin(a);
	}break;
	}
	return 0;
}

template <typename T, int OUTPUT_CHANNELS>
T Osc<T, OUTPUT_CHANNELS>::generate(){
	ang+=(hz+offhz)/sr;
	T ipart=0.0;
	modf(ang, &ipart);
	ang-=ipart;
	const T s= wav(ang*M_PI*2.0)*amp;
	return (T)s;
}



template <typename T, int OUTPUT_CHANNELS>
void Osc<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	sr = f.sampleRate();
}

template <typename T, int OUTPUT_CHANNELS>
void Osc<T, OUTPUT_CHANNELS>::generate(qint64 num, T *out) {
	OC_METHODGATE();
	for(auto i=0; i<num; ++i){
		out[i]=generate();
	}
}




#endif // OSC_HPP
