#ifndef DING_HPP
#define DING_HPP

#include <cmath>
#include "Biquad.hpp"

class Osc{
	public:
		enum Waveform{
			SIN,SAW,TRI,SQR
		};
	private:
		double hz;
		double offhz;
		double sr;
		double ang;
		double amp;

		Waveform w;

	public:

		explicit Osc(double hz=100,double sr=44100,double amp=1,Waveform w=SQR):
			hz(hz)
		  ,offhz(0.0)
		  ,sr(sr)
		  ,ang(0.0f)
		  ,amp(amp)
		  ,w(w)
		{

		}

		inline	void set(double hz=100,double sr=44100,double amp=1,Waveform w=SQR){
			this->hz=hz;
			this->sr=sr;
			this->amp=amp;
			this->w=w;
		}

		inline	void offset(double v){
			this->offhz=v;
		}

		inline double wav(double a){
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

		inline	double process(){
			ang+=(hz+offhz)/sr;
			double ipart=0.0;
			modf(ang,&ipart);
			ang-=ipart;
			const double s= wav(ang*M_PI*2.0)*amp;
			return (double)s;
		}


};

class Ding{
	private:
		Osc o;
		Osc tremo;
		Biquad b1;
		double g;
		double g2;
	//	double trem;
		double damp;
		double damp2;
		double secOfs;
		double step;
	public:
		explicit Ding(double hz=100,double sr=44100,double amp=1,double trem=0.1,double trema=12,double damp=0.99998):
			o(hz,sr,amp,Osc::SQR)
		  ,tremo(hz/trema,sr,trem,Osc::SIN)
		  ,g(0.0f)
		  ,g2(0.0f)
		  ,damp(damp)
		  ,damp2(damp*0.9)
		  ,secOfs(-1)
		  ,step(1.09/sr)
		{
			b1.setBiquad(Biquad::bq_type_bandpass,hz/sr,100.0,6.0);
		}

		inline 	void trigger(double secOfs=0.0){
			this->secOfs=secOfs;
		}

		inline void set(double hz=100,double sr=44100,double amp=1,double trem=0.1,double trema=12,double damp=0.99998){
			b1.setFc(hz/sr);
			this->damp=damp;
			this->damp2=damp*0.9;
			this->o.set(hz,sr,amp);
			this->tremo.set(hz/trema,sr,trem);
		}

		inline double process(){
			if(secOfs>=0.0){
				secOfs-=step;
				if(secOfs<0.0){
					g=1.0f;
				}
			}
			g2=g2*damp2+g*(1.0-damp2);
			g=g*damp;
			o.offset(tremo.process());
			return b1.process( (double)(o.process()*g2));

		}

};
#endif
// DING_HPP
