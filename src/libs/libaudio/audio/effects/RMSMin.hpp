#ifndef RMSMIN_HPP
#define RMSMIN_HPP

#include "audio/IAudio.hpp"

#include <cmath>
#include <QDebug>


template <typename T, int OUTPUT_CHANNELS = 2>
class RMSMin: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
private:
	T wsz;
	int nrms ;
	T *rmsbuf;
	int rmsbi;
	T  rmssum;
	int ci;
	T *crms;
	T r;
	T r2;
	
public:
	explicit RMSMin(T wsz=0.1);
	virtual ~RMSMin() Q_DECL_OVERRIDE;
	T  process(T s);
	
	//IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, T *in, T *out) Q_DECL_OVERRIDE;
	
};


template <typename T, int OUTPUT_CHANNELS>
RMSMin<T, OUTPUT_CHANNELS>::RMSMin(T wsz):
	wsz(wsz)
	, nrms (0)
  , rmsbuf(new T[nrms])
  , rmsbi(0)
  , rmssum(0)
  , ci(0)
  , crms(new T[OUTPUT_CHANNELS])
  , r(0)
  , r2(0)
  //  ,r2(0)
{
	for (int j = 0; j < nrms; ++j){
		rmsbuf[j]=0.0;
	}
	for (int j = 0; j < OUTPUT_CHANNELS; ++j){
		crms[j]=0.0;
	}
}


template <typename T, int OUTPUT_CHANNELS>
T RMSMin<T, OUTPUT_CHANNELS>::process(T s){
	// Compute RMS for next sample in window
	crms[ci]=s;
	const int oci=ci;
	ci=(ci+1) % OUTPUT_CHANNELS;
	if(ci<=oci){
		T smp=0.0;
		for(int k=0;k<OUTPUT_CHANNELS;++k){
			smp+=crms[k];
		}
		smp/=static_cast<T>(OUTPUT_CHANNELS);
		smp*=smp;
		rmssum-=rmsbuf[rmsbi];
		rmsbuf[rmsbi]= smp;
		rmssum+=smp;
		rmsbi=(rmsbi+1)%nrms;
		r = sqrt (rmssum / nrms);   // root-mean-square
		r2=r2*0.99+r*0.01;
		//	qDebug()<<"CC:"<<cc<<" CI:"<<ci<<" OCI:"<<oci<<" rmsbi:"<<rmsbi<< " nrms:"<<nrms<< " rmssum:"<<rmssum<<" smp:"<<smp<<" s:"<<s;
	}
	return r;
}


template <typename T, int OUTPUT_CHANNELS>
void RMSMin<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	auto sr=f.sampleRate();
	nrms=static_cast<int>(sr * wsz);
	if(rmsbuf){
		delete rmsbuf;
		rmsbuf=nullptr;
	}
	rmsbuf=(new T[nrms]);
	for (int j = 0; j < nrms; ++j){
		rmsbuf[j]=0.0;
	}
	for (int j = 0; j < OUTPUT_CHANNELS; ++j){
		crms[j]=0.0;
	}
}

template <typename T, int OUTPUT_CHANNELS>
void RMSMin<T, OUTPUT_CHANNELS>::process(qint64 n, T *in, T *out)
{
	// for each sample...
	for (int i = 0; i < n-OUTPUT_CHANNELS; i+=OUTPUT_CHANNELS)	{
		process(in[i]);
		out[i]=r2;
	}
}



#endif // RMSMIN_HPP
