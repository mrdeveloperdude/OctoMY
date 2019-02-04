#include "Tremolo.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QtMath>


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
	OC_METHODGATE();
}

Tremolo::~Tremolo()
{
	OC_METHODGATE();
	isInited=false;
	if(nullptr!=buf) {
		delete[] buf;
		buf=nullptr;
	}
}

void Tremolo::set(double freq,double amp)
{
	OC_METHODGATE();
	double sr=static_cast<double>(af.sampleRate());
	step=freq/(sr*M_PI*2.0);
	ampSR=amp*sr;
	size=(static_cast<size_t>(std::floor(ampSR)))+1;
	qDebug()<<"TREM SIZE:"<<size<<" AMP:"<<amp<<" AMP-SR:"<<ampSR<<" STEP:"<<step<<" SR:"<<sr;
	index=0;
	delete[] buf;
	buf=nullptr;
	if(size>0) {
		buf=OC_NEW double[size];
		for(size_t i=0; i<size; ++i) {
			buf[i]=0.0;
		}
	}
	isInited=sr>0;
}

void Tremolo::init(QAudioFormat f)
{
	OC_METHODGATE();
	af=f;
	set(freq,amp);
}

void Tremolo::process(qint64 num, double *in, double *out)
{
	OC_METHODGATE();
	if(isInited) {
		for(int i=0; i<num; ++i) {
			buf[index]=in[i];
			index=(index+1)%static_cast<int>(size);
			angle+=step;
			while(angle>M_PI*2.0) {
				angle-=M_PI*2.0;
			}
			const double displacement=(cos(angle)*0.5+0.5)*ampSR;
			const int a=static_cast<int>(std::floor(displacement));
			//TODO: Was b=b+1 here a typo?
			const int b=a+1;
			const double c=displacement-static_cast<double>(a);
			const double d=1.0-c;
			const int ai=(index+a)%static_cast<int>(size);
			const int bi=(index+b)%static_cast<int>(size);
			out[i]=(buf[ai]*d+buf[bi]*c)*0.9+(in[ai]*d+in[bi]*c)*0.1;//Some character :D
		}
	}
}
