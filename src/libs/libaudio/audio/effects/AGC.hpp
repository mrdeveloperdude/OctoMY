#ifndef AGC_H
#define AGC_H

#include "audio/IAudio.hpp"

#include "audio/effects/RMSMin.hpp"
#include <cmath>
#include <QDebug>

template <typename T, int OUTPUT_CHANNELS = 2>
class AGC: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
private:
	//		int sr;
	//		int cc;
	//		int twsz;
	T wsz;
	int nbuf;
	T *buf;
	int bufi;
	T g;
	T g2;
	RMSMin<T, OUTPUT_CHANNELS> r;
	
public:
	AGC(T wsz=0.1);
	
	T process(T s);
	
	//IAudioProcessor interface
public:
	void init(QAudioFormat f) override;
	void process(qint64 num, T *in, T *out) override;
	
	
};



//////////////////


template <typename T, int OUTPUT_CHANNELS>
AGC<T, OUTPUT_CHANNELS>::AGC(T wsz):
	//			sr(sr) //samplerate
	//		  ,cc(cc)
	wsz(wsz)
	, nbuf(0)
  , buf(new T[nbuf])
  , bufi(0)
  , g(0)
  , g2(0)
  , r(wsz)
{
	for (int j = 0; j < nbuf; ++j){
		buf[j]=0.0;
	}
}


template <typename T, int OUTPUT_CHANNELS>
T AGC<T, OUTPUT_CHANNELS>::process(T s){
	const T s2 = buf[bufi];
	buf[bufi] = s;
	bufi = (bufi + 1) % nbuf;
	g = r.process(s);
	g2 = g2 * 0.999 + 0.001 * g;
	return s2 * g2;
}



template <typename T, int OUTPUT_CHANNELS>
void AGC<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	auto sr=f.sampleRate();
	nbuf=static_cast<int>(sr * wsz);
	if(buf){
		delete buf;
		buf=nullptr;
	}
	buf=(new T[nbuf]);
	for (int j = 0; j < nbuf; ++j){
		buf[j]=0.0;
	}
}

template <typename T, int OUTPUT_CHANNELS>
void AGC<T, OUTPUT_CHANNELS>::process(qint64 n, T *in, T *out)
{
	// for each sample...
	for (int i = 0; i < n-OUTPUT_CHANNELS; i+=OUTPUT_CHANNELS)	{
		out[i]=process(in[i]);
	}
}




#endif // AGC_H
