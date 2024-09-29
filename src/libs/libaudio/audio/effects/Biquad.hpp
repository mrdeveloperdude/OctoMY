#ifndef BIQUAD_HPP
#define BIQUAD_HPP

#include "uptime/MethodGate.hpp"
#include "audio/IAudio.hpp"

#include <QtGlobal>
#include <cstdlib>
#include <cmath>

template <typename T, int OUTPUT_CHANNELS = 2>
class Biquad: public IAudioProcessor<T, OUTPUT_CHANNELS> {
private:
	void calcBiquad(void);
	
	int type;
	T sampleRate;
	T a0, a1, a2, b1, b2;
	T Fc, Q, peakGain;
	T z1, z2;
	
public:
	enum BiquadType{
		LOWPASS = 0,
		HIGHPASS,
		BANDPASS,
		NOTCH,
		PEAK,
		LOWSHELF,
		HIGHSHELF
	};
	
public:
	Biquad();
	Biquad(BiquadType type, T Fc, T Q, T peakGainDB);
	~Biquad();
	
	
public:
	void setType(BiquadType type);
	void setQ(T Q);
	void setFc(T Fc);
	void setPeakGain(T peakGainDB);
	void setBiquad(BiquadType type, T Fc, T Q, T peakGain);
	
	
public:
	T process(T in);
	
	// IAudioProcessor interface
public:
	void init(QAudioFormat f) override;
	void process(qint64 num, T *in, T *out)override;
	
};


////////////////////////////////////


template <typename T, int OUTPUT_CHANNELS>
Biquad<T, OUTPUT_CHANNELS>::Biquad() {
	OC_METHODGATE();
	type = LOWPASS;
	sampleRate = 44100.0;
	a0 = 1.0;
	a1 = 0.0;
	a2 = 0.0;
	b1 = 0.0;
	b2 = 0.0;
	Fc = 0.50;
	Q = 0.707;
	peakGain = 0.0;
	z1 = z2 = 0.0;
}

template <typename T, int OUTPUT_CHANNELS>
Biquad<T, OUTPUT_CHANNELS>::Biquad(BiquadType type, T Fc, T Q, T peakGainDB) {
	OC_METHODGATE();
	setBiquad(type, Fc, Q, peakGainDB);
	z1 = z2 = 0.0;
}

template <typename T, int OUTPUT_CHANNELS>
Biquad<T, OUTPUT_CHANNELS>::~Biquad() {
	OC_METHODGATE();
}

template <typename T, int OUTPUT_CHANNELS>
T Biquad<T, OUTPUT_CHANNELS>::process(T in) {
	OC_METHODGATE();
	T out = in * a0 + z1;
	z1 = in * a1 + z2 - b1 * out;
	z2 = in * a2 - b2 * out;
	return out;
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::init(QAudioFormat f)
{
	OC_METHODGATE();
	this->sampleRate = static_cast<T>(f.sampleRate());
	calcBiquad();
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::process(qint64 num, T *inbuf, T *outbuf) {
	OC_METHODGATE();
	for(auto i=0; i<num; ++i){
		outbuf[i]=process(inbuf[i]);
	}
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::setType(BiquadType type) {
	this->type = type;
	calcBiquad();
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::setQ(T Q) {
	this->Q = Q;
	calcBiquad();
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::setFc(T Fc) {
	this->Fc = Fc / sampleRate;
	calcBiquad();
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::setPeakGain(T peakGainDB) {
	this->peakGain = peakGainDB;
	calcBiquad();
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::setBiquad(BiquadType type, T Fc, T Q, T peakGainDB) {
	this->type = type;
	this->Q = Q;
	this->Fc = Fc / sampleRate;
	setPeakGain(peakGainDB);
}

template <typename T, int OUTPUT_CHANNELS>
void Biquad<T, OUTPUT_CHANNELS>::calcBiquad(void) {
	T V = pow(10, fabs(peakGain) / 20.0);
	T K = tan(M_PI * Fc);
	switch (this->type) {
	case LOWPASS:{
		auto norm = 1 / (1 + K / Q + K * K);
		a0 = K * K * norm;
		a1 = 2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
	}break;
		
	case HIGHPASS:{
		auto norm = 1 / (1 + K / Q + K * K);
		a0 = 1 * norm;
		a1 = -2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
	}break;
		
	case BANDPASS:{
		auto norm = 1 / (1 + K / Q + K * K);
		a0 = K / Q * norm;
		a1 = 0;
		a2 = -a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
	}break;
		
		
	case NOTCH:{
		auto norm = 1 / (1 + K / Q + K * K);
		a0 = (1 + K * K) * norm;
		a1 = 2 * (K * K - 1) * norm;
		a2 = a0;
		b1 = a1;
		b2 = (1 - K / Q + K * K) * norm;
	}break;
		
	case PEAK:{
		if (peakGain >= 0) {    // boost
			auto norm = 1 / (1 + 1/Q * K + K * K);
			a0 = (1 + V/Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - V/Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - 1/Q * K + K * K) * norm;
		}
		else {    // cut
			auto norm = 1 / (1 + V/Q * K + K * K);
			a0 = (1 + 1/Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - 1/Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - V/Q * K + K * K) * norm;
		}
	}break;
	case LOWSHELF:{
		if (peakGain >= 0) {    // boost
			auto norm = 1 / (1 + sqrt(2) * K + K * K);
			a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
			a1 = 2 * (V * K * K - 1) * norm;
			a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else {    // cut
			auto norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
			a0 = (1 + sqrt(2) * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - sqrt(2) * K + K * K) * norm;
			b1 = 2 * (V * K * K - 1) * norm;
			b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
		}
	}break;
	case HIGHSHELF:{
		if (peakGain >= 0) {    // boost
			auto norm = 1 / (1 + sqrt(2) * K + K * K);
			a0 = (V + sqrt(2*V) * K + K * K) * norm;
			a1 = 2 * (K * K - V) * norm;
			a2 = (V - sqrt(2*V) * K + K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - sqrt(2) * K + K * K) * norm;
		}
		else {    // cut
			auto norm = 1 / (V + sqrt(2*V) * K + K * K);
			a0 = (1 + sqrt(2) * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - sqrt(2) * K + K * K) * norm;
			b1 = 2 * (K * K - V) * norm;
			b2 = (V - sqrt(2*V) * K + K * K) * norm;
		}
	}break;
	}
	return;
}




#endif // BIQUAD_HPP
