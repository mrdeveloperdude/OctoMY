#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "audio/IAudio.hpp"

#include <QtMath>

template <typename T, int OUTPUT_CHANNELS = 2>
class Compressor: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
private:
	
	T  threshold;  // threshold (factor)
	T  slope;      // slope angle (factor)
	int     sr;         // sample rate (smp/sec)
	T  tla;        // lookahead  (sec)
	T  twnd;       // window time (sec)
	T  tatt;       // attack time  (sec)
	T  trel;        // release time (sec)
	// attack and release "per sample decay"
	T  att;
	T  rel;
	
	
public:
	explicit Compressor( T  threshold=0.5, T  slope=0.5 , int     sr=44100 , T  tla=3.0/1000.0 , T  twnd=1.0/1000.0 , T  tatt=0.1/1000.0 , T  trel=300.0/1000.0);
	virtual ~Compressor() Q_DECL_OVERRIDE;
	
	void set(T  threshold=0.5, T  slope=0.5 , T  tla=3.0/1000.0 , T  twnd=1.0/1000.0 , T  tatt=0.1/1000.0 , T  trel=300.0/1000.0);
	
	//IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, T *in, T *out) Q_DECL_OVERRIDE;
};


////////////////////////////////////////////////////////////////////

template <typename T, int OUTPUT_CHANNELS>
Compressor<T, OUTPUT_CHANNELS>::Compressor(
		T  threshold
		, T  slope
		, int     sr
		, T  tla
		, T  twnd
		, T  tatt
		, T  trel
		)
	: threshold(threshold)  // threshold (factor)
	, slope(slope)      // slope angle (factor)
	, sr(sr)         // sample rate (smp/sec)
	, tla(tla)        // lookahead  (sec)
	, twnd(twnd)       // window time (sec)
	, tatt(tatt)       // attack time  (sec)
	, trel(trel)        // release time (sec)
	// attack and release "per sample decay"
	, att ( (tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)))
	, rel ( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)))
{
	
	
}



template <typename T, int OUTPUT_CHANNELS>
Compressor<T, OUTPUT_CHANNELS>::~Compressor()
{
	
}

template <typename T, int OUTPUT_CHANNELS>
void Compressor<T, OUTPUT_CHANNELS>::set(T  threshold, T  slope , T  tla, T  twnd , T  tatt , T  trel)
{
	this->threshold=(threshold) ; // threshold (factor)
	this->slope=(slope);      // slope angle (factor)
	this->tla=(tla) ;       // lookahead  (sec)
	this->twnd=(twnd);       // window time (sec)
	this->tatt=(tatt);       // attack time  (sec)
	this->trel=(trel);        // release time (sec)
	// attack and release "per sample decay"
	this->att =( (tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)));
	this->rel =( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)));
}

// IAudioProcessor interface

template <typename T, int OUTPUT_CHANNELS>
void Compressor<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	sr=f.sampleRate();
	att =( (tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)));
	rel =( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)));
}

template <typename T, int OUTPUT_CHANNELS>
void Compressor<T, OUTPUT_CHANNELS>::process(qint64 n, T *wav_in, T *wav_out)
{
	
	// envelope
	T  env = 0.0;
	
	// sample offset to lookahead wnd start
	int     lhsmp = (int) (sr * tla);
	
	// samples count in lookahead window
	int     nrms = (int) (sr * twnd);
	
	// for each sample...
	for (int i = 0; i < n; ++i) {
		// now compute RMS
		T  summ = 0;
		
		// for each sample in window
		for (int j = 0; j < nrms; ++j) {
			int     lki = i + j + lhsmp;
			T  smp;
			
			// if we in bounds of signal?
			// if so, convert to mono
			if (lki < n) {
				smp = wav_in[lki];
			} else {
				smp = 0.0;    // if we out of bounds we just get zero in smp
			}
			
			summ += smp * smp;  // square em..
		}
		
		T  rms = sqrt (summ / nrms);   // root-mean-square
		
		// dynamic selection: attack or release?
		T  theta = rms > env ? att : rel;
		
		// smoothing with capacitor, envelope extraction...
		// here be aware of pIV denormal numbers glitch
		env = (1.0 - theta) * rms + theta * env;
		
		// the very easy hard knee 1:N compressor
		T  gain = 1.0;
		if (env > threshold) {
			gain = gain - (env - threshold) * slope;
		}
		
		// result - two hard kneed compressed channels...
		wav_out[i]=wav_in[i] * gain;
		
	}
}

//////////////////////////



template <typename T, int OUTPUT_CHANNELS = 2>
class Compressor2: public IAudioProcessor<T, OUTPUT_CHANNELS>
{
private:
	int	sr;			// sample rate (smp/sec)
	int	cc;			//channel count
	T	threshold;	// threshold (percents)
	T	slope;		// slope angle (percents)
	T	tla;		// lookahead  (ms)
	T	twnd;		// window time (ms)
	T	tatt;		// attack time  (ms)
	T	trel;		// release time (ms)
	T	att ;
	T	rel ;
	// envelope
	T	env ;
	T	mgain ;
	// sample offset to lookahead wnd start
	int	lhsmp ;
	// samples count in lookahead window
	int	nrms ;
	T	*rmsbuf;
	int	rmsbi;
	T	rmssum;
	T	*ccbuf;
	
public:
	Compressor2(int sr=44100, int cc=1, T threshold=50,	T slope=50, T tla=3, T twnd=1, T tatt=0.1, T trel=300);
	
	
	T process(T in);
	
	//IAudioProcessor interface
public:
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, T *in, T *out) Q_DECL_OVERRIDE;
	
	
};


/////

template <typename T, int OUTPUT_CHANNELS>
Compressor2<T, OUTPUT_CHANNELS>::Compressor2(int sr, int cc, T threshold, T slope, T tla, T twnd, T tatt, T trel):
	sr(sr)
  , cc(cc)
  , threshold(threshold * 0.01)// threshold to unity (0...1)
  , slope(slope * 0.01)	// slope to unity
  , tla(tla * 1e-3)		// lookahead time to seconds
  , twnd(twnd * 1e-3)	// window time to seconds
  , tatt(tatt * 1e-3)	// attack time to seconds
  , trel(trel * 1e-3)	// release time to seconds
  // attack and release "per sample decay"
  , att  ((tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)))
  , rel ( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)))
  , env(0)
  , mgain(1.0)
  // sample offset to lookahead wnd start
  , lhsmp ( (int) (sr * tla))
  // samples count in lookahead window
  , nrms ((int) (sr * twnd))
  , rmsbuf(new T[nrms])
  , rmsbi(0)
  , rmssum(0)
{
	for (int j = 0; j < nrms; ++j){
		rmsbuf[j]=0.0;
	}
}


template <typename T, int OUTPUT_CHANNELS>
T Compressor2<T, OUTPUT_CHANNELS>::process(T in){
	// TODO: Extract main algo from the other method
	return in;
}


template <typename T, int OUTPUT_CHANNELS>
void Compressor2<T, OUTPUT_CHANNELS>::process(qint64 num, T *in, T *out){
	for (int i = 0; i < num-cc; i+=cc)	{
		// compute RMS for next sample in window
		int     lki = i  + lhsmp;
		T  smp=0.0;
		// if we are in bounds of signal, convert to mono
		if (lki < num){
			for(int k=0;k<cc;++k){
				smp +=  in[lki+k];
			}
		}
		smp/=(T)cc;
		rmssum-=rmsbuf[rmsbi];
		rmsbuf[rmsbi]= smp * smp;  // square em..
		rmssum+=rmsbuf[rmsbi];
		rmsbi=(rmsbi+1)%nrms;
		
		T  rms = sqrt (rmssum / nrms);   // root-mean-square
		
		// dynamic selection: attack or release?
		T  theta = rms > env ? att : rel;
		
		// smoothing with capacitor, envelope extraction...
		// here be aware of pIV denormal numbers glitch
		env = (1.0 - theta) * rms + theta * env;
		
		// the very easy hard knee 1:N compressor
		T  gain = 1.0;
		if (env > threshold){
			gain = gain - (env - threshold) * slope;
		}
		mgain = mgain*0.9 + gain*0.1;
		for(int k=0; k<cc; ++k){
			out[i+k] = in[i+k] * mgain;
		}
	}
}


#endif // COMPRESSOR_HPP
