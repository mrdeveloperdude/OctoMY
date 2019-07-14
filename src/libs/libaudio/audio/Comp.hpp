#ifndef COMP_HPP
#define COMP_HPP

#include <cmath>
#include <QDebug>

class RMSMIN {
	private:
//		int sr;
		int cc;
//		int twsz;
		int nrms ;
		double *rmsbuf;
		int rmsbi;
		double  rmssum;
		int ci;
		double *crms;
		double r;
//		double r2;

	public:

		RMSMIN(int sr=44100, int cc=1, double wsz=0.1):
	//		sr(sr) //samplerate
		  cc(cc)
		  ,nrms ((int) (sr * wsz))
		  ,rmsbuf(new double[nrms])
		  ,rmsbi(0)
		  ,rmssum(0)
		  ,ci(0)
		  ,crms(new double [cc])
		  ,r(0)
		//  ,r2(0)
		{
			for (int j = 0; j < nrms; ++j){
				rmsbuf[j]=0.0;
			}
			for(int k=0;k<cc;++k){
				crms[k]=0.0;
			}
		}



		inline double  process(double s){
			// compute RMS for next sample in window
			crms[ci]=s;
			const int oci=ci;
			ci=(ci+1)%cc;
			if(ci<=oci){
				double  smp=0.0;
				for(int k=0;k<cc;++k){
					smp+=crms[k];
				}
				smp/=(double)cc;
				smp*=smp;
				rmssum-=rmsbuf[rmsbi];
				rmsbuf[rmsbi]= smp;
				rmssum+=smp;
				rmsbi=(rmsbi+1)%nrms;
				r = sqrt (rmssum / nrms);   // root-mean-square
				//r2=r2*0.99+r*0.01;
				//	qDebug()<<"CC:"<<cc<<" CI:"<<ci<<" OCI:"<<oci<<" rmsbi:"<<rmsbi<< " nrms:"<<nrms<< " rmssum:"<<rmssum<<" smp:"<<smp<<" s:"<<s;
			}
			return r;
		}


		inline void process(float*  wav_in,  float*  wav_out,int n){
			// for each sample...
			for (int i = 0; i < n-cc; i+=cc)	{
				process(wav_in[i]);
				wav_out[i]=r;
			}
		}
};




class AGC{
	private:

//		int sr;
//		int cc;
//		int twsz;
		int nbuf ;
		double *buf;
		int bufi;
		double g;
		double g2;
		RMSMIN r;

	public:
		AGC(int sr=44100, int cc=1, double wsz=0.1):
//			sr(sr) //samplerate
//		  ,cc(cc)
		  nbuf ((int) (sr * wsz))
		  ,buf(new double[nbuf])
		  ,bufi(0)
		  ,g(0)
		  ,g2(0)
		  ,r(sr,cc,wsz)
		{
			for (int j = 0; j < nbuf; ++j){
				buf[j]=0.0;
			}
		}

		inline double  process(double s){
			const double s2=buf[bufi];
			buf[bufi]=s;
			bufi=(bufi+1)%nbuf;
			g=r.process(s);
			g2=g2*0.999+0.001*g;
			return s2*g2;
		}


};

class Comp{


	private:
//		int     sr;         // sample rate (smp/sec)
		int cc; //channel count
		double  threshold;  // threshold (percents)
		double  slope;      // slope angle (percents)

//		double  tla;        // lookahead  (ms)
//		double  twnd;       // window time (ms)
//		double  tatt;       // attack time  (ms)
//		double  trel;        // release time (ms)

		double  att ;
		double  rel ;
		// envelope
		double  env ;
		double  mgain ;
		// sample offset to lookahead wnd start
		int     lhsmp ;
		// samples count in lookahead window
		int     nrms ;

		float *rmsbuf;
		int rmsbi;
		double  rmssum;

//		float *ccbuf;

	public:

		Comp(	int     sr=44100, int cc=1, double  threshold=50,	double  slope=50,double  tla=3,double  twnd=1,double  tatt=0.1, double  trel=300):
		//	sr(sr) //samplerate
		  cc(cc)
		  ,threshold(threshold * 0.01)         // threshold to unity (0...1)
		  ,slope(slope * 0.01)              // slope to unity
		//  ,tla(tla * 1e-3)               // lookahead time to seconds
		//  ,twnd(twnd * 1e-3)               // window time to seconds
	//	  ,tatt(tatt * 1e-3)               // attack time to seconds
	//	  ,trel(trel * 1e-3)               // release time to seconds
		  // attack and release "per sample decay"
		  ,att  ((tatt == 0.0) ? (0.0) : exp (-1.0 / (sr * tatt)))
		  ,rel ( (trel == 0.0) ? (0.0) : exp (-1.0 / (sr * trel)))
		  ,env(0)
		  ,mgain(1.0)
		  // sample offset to lookahead wnd start
		  ,lhsmp ( (int) (sr * tla))
		  // samples count in lookahead window
		  ,nrms ((int) (sr * twnd))
		  ,rmsbuf(new float[nrms])
		  ,rmsbi(0)
		  ,rmssum(0)
		{
			for (int j = 0; j < nrms; ++j){
				rmsbuf[j]=0.0;
			}
		}

		inline void process(float*  wav_in,  float*  wav_out,int n){
			// for each sample...
			for (int i = 0; i < n-cc; i+=cc)	{
				// compute RMS for next sample in window
				int     lki = i  + lhsmp;
				double  smp=0.0;
				// if we are in bounds of signal, convert to mono
				if (lki < n){
					for(int k=0;k<cc;++k){
						smp +=  wav_in[lki+k];
					}
				}
				smp/=(double)cc;
				rmssum-=rmsbuf[rmsbi];
				rmsbuf[rmsbi]= smp * smp;  // square em..
				rmssum+=rmsbuf[rmsbi];
				rmsbi=(rmsbi+1)%nrms;

				double  rms = sqrt (rmssum / nrms);   // root-mean-square

				// dynamic selection: attack or release?
				double  theta = rms > env ? att : rel;

				// smoothing with capacitor, envelope extraction...
				// here be aware of pIV denormal numbers glitch
				env = (1.0 - theta) * rms + theta * env;

				// the very easy hard knee 1:N compressor
				double  gain = 1.0;
				if (env > threshold){
					gain = gain - (env - threshold) * slope;
				}
				mgain=mgain*0.9+gain*0.1;
				for(int k=0;k<cc;++k){
					wav_out[i+k]=wav_in[i+k]*mgain;
				}
			}
		}

};
#endif // COMP_HPP
