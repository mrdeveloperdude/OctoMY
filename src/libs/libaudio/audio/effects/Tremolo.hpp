#ifndef TREMOLO_HPP
#define TREMOLO_HPP

#include "audio/IAudio.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QtMath>

template <typename T, int OUTPUT_CHANNELS = 2>
class Tremolo: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
private:
	QAudioFormat af;
	T freq;
	T amp;
	T angle;
	T step;
	T ampSR;
	T *buf;
	size_t size;
	int index;
	bool isInited;
	
public:
	explicit Tremolo();
	virtual ~Tremolo() Q_DECL_OVERRIDE;
	
public:
	void set(T freq,T amp);
	T process(T in);
	
	//IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, T *in, T *out) Q_DECL_OVERRIDE;
};



/////////////////////////////////////////////////////


template <typename T, int OUTPUT_CHANNELS>
Tremolo<T, OUTPUT_CHANNELS>::Tremolo()
	: freq(20.0)
	, amp(0.00001)
	, angle(0)
	, step(0)
	, ampSR(0)
	, buf(nullptr)
	, size (0)
	, index(0)
	, isInited(false)
{
	OC_METHODGATE();
}

template <typename T, int OUTPUT_CHANNELS>
Tremolo<T, OUTPUT_CHANNELS>::~Tremolo()
{
	OC_METHODGATE();
	isInited=false;
	if(nullptr!=buf) {
		delete[] buf;
		buf=nullptr;
	}
}

template <typename T, int OUTPUT_CHANNELS>
void Tremolo<T, OUTPUT_CHANNELS>::set(T freq,T amp)
{
	OC_METHODGATE();
	T sr=static_cast<T>(af.sampleRate());
	step=freq/(sr*M_PI*2.0);
	ampSR=amp*sr;
	size=(static_cast<size_t>(std::floor(ampSR)))+1;
	qDebug()<<"TREM SIZE:"<<size<<" AMP:"<<amp<<" AMP-SR:"<<ampSR<<" STEP:"<<step<<" SR:"<<sr;
	index=0;
	delete[] buf;
	buf=nullptr;
	if(size>0) {
		buf=OC_NEW T[size];
		for(size_t i=0; i<size; ++i) {
			buf[i]=0.0;
		}
	}
	isInited=sr>0;
}

template <typename T, int OUTPUT_CHANNELS>
void Tremolo<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	af=f;
	set(freq,amp);
}


template <typename T, int OUTPUT_CHANNELS>
T Tremolo<T, OUTPUT_CHANNELS>::process(T in)
{
	OC_METHODGATE();
	buf[index]=in;
	index=(index+1)%static_cast<int>(size);
	angle+=step;
	while(angle>M_PI*2.0) {
		angle-=M_PI*2.0;
	}
	const T displacement=(cos(angle)*0.5+0.5)*ampSR;
	return displacement;
}
	
template <typename T, int OUTPUT_CHANNELS>
void Tremolo<T, OUTPUT_CHANNELS>::process(qint64 num, T *in, T *out)
{
	OC_METHODGATE();
	if(isInited) {
		for(int i=0; i<num; ++i) {
			auto displacement = process( in[i]);
			const int a=static_cast<int>(std::floor(displacement));
			//TODO: Was b=b+1 here a typo?
			const int b=a+1;
			const T c=displacement-static_cast<T>(a);
			const T d=1.0-c;
			const int ai=(index+a)%static_cast<int>(size);
			const int bi=(index+b)%static_cast<int>(size);
			out[i] = (buf[ai]*d+buf[bi]*c)*0.9+(in[ai]*d+in[bi]*c)*0.1;//Some character :D
		}
	}
}

#endif
// TREMOLO_HPP

