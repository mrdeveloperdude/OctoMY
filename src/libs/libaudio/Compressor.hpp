#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "IAudioSource.hpp"

class Compressor: public IAudioProcessor
{
private:

	double  threshold;  // threshold (factor)
	double  slope;      // slope angle (factor)
	int     sr;         // sample rate (smp/sec)
	double  tla;        // lookahead  (sec)
	double  twnd;       // window time (sec)
	double  tatt;       // attack time  (sec)
	double  trel;        // release time (sec)
	// attack and release "per sample decay"
	double  att;
	double  rel;


public:
	Compressor( double  threshold=0.5, double  slope=0.5 , int     sr=44100 , double  tla=3.0/1000.0 , double  twnd=1.0/1000.0 , double  tatt=0.1/1000.0 , double  trel=300.0/1000.0);

	//IAudioProcessor interface
public:
	void set(double  threshold=0.5, double  slope=0.5 , double  tla=3.0/1000.0 , double  twnd=1.0/1000.0 , double  tatt=0.1/1000.0 , double  trel=300.0/1000.0);
	void init(QAudioFormat f) Q_DECL_OVERRIDE;
	void process(qint64 num, double *in, double *out) Q_DECL_OVERRIDE;


};

#endif // COMPRESSOR_HPP
