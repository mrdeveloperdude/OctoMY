#ifndef DRIBBLE_HPP
#define DRIBBLE_HPP

#include <QtCore>

#include <cmath>

class Noise{
	private:
		const double change;
		const double ichange;
		const double min;
//		const double max;
		const double range;


		double v;
		double n;


		const qint64 minp;
	//	const qint64 maxp;
		const qint64 rangep;
		qint64 ct;
		qint64 end;

	public:
		Noise(const double change, const double min, const double max, const qint64 minp, const qint64 maxp):
			change(change)
		  ,ichange(1.0-change)
		  ,min(min)
	//	  ,max(max)
		  ,range(max-min)
		  ,v(0.0)
		  , n(0.0)

		  ,minp(minp)
	//	  ,maxp(maxp)
		  ,rangep(maxp-minp)
		  ,ct(0)
		  ,end(0)
		{
		}

		inline double frand(){
			return ((double)rand())/((double)RAND_MAX);
		}

		inline double process( ){
			ct++;
			if(ct>end){
				ct=0;
				end=rand()%rangep+minp;
				n=frand();
			}
			v=(v*change)+(min + n * range )*ichange;
			return v;
		}
};

class Dribble{
	private:
		double sr;
		Noise fn;
		Noise an;
		double ang=0.0;
		double step=M_PI*0.1/sr;
//		bool lastOver=false;


	public:
		Dribble(double sr, double min, double max):sr(sr),fn(0.99,min,max,(qint64)((sr/1000.0)*5.0),(qint64)((sr/1000.0)*10.0)), an(0.7,0.5,1.0,(qint64)((sr/1000.0)*10.0),(qint64)((sr/1000.0)*20.0)){
		}

		inline double frand(){
			return ((double)rand())/((double)RAND_MAX);
		}

		inline double process( ){
			//Generate sample for this index
			const double currentF=fn.process();
			const double amp=an.process();
			step=(M_PI*2.0)*currentF/sr;
			const double s=sin(ang)*amp;
			//Cyclic angle for oscilator
			ang+=step;
			double ipart=0.0;
			(void)modf(ang/(M_PI*2.0),&ipart);
			ang-=ipart*M_PI*2.0;
			return s;
		}

		inline void process( float*  wav_out,int n){
			for(int i=0;i<n;++i){
				wav_out[i]=process();
			}
		}
};

#endif // DRIBBLE_HPP
