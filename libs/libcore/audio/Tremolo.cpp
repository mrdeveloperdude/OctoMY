#include "Tremolo.hpp"


#include <QDebug>

Tremolo::Tremolo()
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
}

Tremolo::~Tremolo()
{
	isInited=false;
	if(nullptr!=buf) {
		delete[] buf;
		buf=nullptr;
	}
}

void Tremolo::set(double freq,double amp)
{
	double sr=(double)af.sampleRate();
	step=freq/(sr*M_PI*2.0);
	ampSR=amp*sr;
	size=((int)floor(ampSR))+1;
	qDebug()<<"TREM SIZE:"<<size<<" AMP:"<<amp<<" AMP-SR:"<<ampSR<<" STEP:"<<step<<" SR:"<<sr;
	index=0;
	delete[] buf;
	buf=nullptr;
	if(size>0) {
		buf=new double[size];
		for(int i=0; i<size; ++i) {
			buf[i]=0.0;
		}
	}
	isInited=sr>0;
}

void Tremolo::init(QAudioFormat f)
{
	af=f;
	set(freq,amp);
}

void Tremolo::process(qint64 num, double *in, double *out)
{
	if(isInited) {
		for(int i=0; i<num; ++i) {
			buf[index]=in[i];
			index=(index+1)%size;
			angle+=step;
			while(angle>M_PI*2.0) {
				angle-=M_PI*2.0;
			}
			const double displacement=(cos(angle)*0.5+0.5)*ampSR;
			const int a=(int)floor(displacement);
			const int b=b+1;
			const double c=displacement-(double)a;
			const double d=1.0-c;
			const int ai=(index+a)%size;
			const int bi=(index+b)%size;
			out[i]=(buf[ai]*d+buf[bi]*c)*0.9+(in[ai]*d+in[bi]*c)*0.1;//Some character :D
		}
	}
}
