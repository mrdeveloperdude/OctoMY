#include "Compressor.hpp"

Compressor::Compressor(
	double  threshold
	, double  slope
	, int     sr
	, double  tla
	, double  twnd
	, double  tatt
	, double  trel
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


void Compressor::set(double  threshold, double  slope , double  tla, double  twnd , double  tatt , double  trel)
{
	threshold=(threshold) ; // threshold (factor)
	slope=(slope);      // slope angle (factor)
	tla=(tla) ;       // lookahead  (sec)
	twnd=(twnd);       // window time (sec)
	tatt=(tatt);       // attack time  (sec)
	trel=(trel);        // release time (sec)
	// attack and release "per sample decay"
	att =( (tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)));
	rel =( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)));
}

// IAudioProcessor interface

void Compressor::init(QAudioFormat f)
{
	sr=f.sampleRate();
	att =( (tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)));
	rel =( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)));
}

void Compressor::process(qint64 n, double *wav_in, double *wav_out)
{

	// envelope
	double  env = 0.0;

	// sample offset to lookahead wnd start
	int     lhsmp = (int) (sr * tla);

	// samples count in lookahead window
	int     nrms = (int) (sr * twnd);

	// for each sample...
	for (int i = 0; i < n; ++i) {
		// now compute RMS
		double  summ = 0;

		// for each sample in window
		for (int j = 0; j < nrms; ++j) {
			int     lki = i + j + lhsmp;
			double  smp;

			// if we in bounds of signal?
			// if so, convert to mono
			if (lki < n) {
				smp = wav_in[lki];
			} else {
				smp = 0.0;    // if we out of bounds we just get zero in smp
			}

			summ += smp * smp;  // square em..
		}

		double  rms = sqrt (summ / nrms);   // root-mean-square

		// dynamic selection: attack or release?
		double  theta = rms > env ? att : rel;

		// smoothing with capacitor, envelope extraction...
		// here be aware of pIV denormal numbers glitch
		env = (1.0 - theta) * rms + theta * env;

		// the very easy hard knee 1:N compressor
		double  gain = 1.0;
		if (env > threshold) {
			gain = gain - (env - threshold) * slope;
		}

		// result - two hard kneed compressed channels...
		wav_out[i]=wav_in[i] * gain;

	}
}
