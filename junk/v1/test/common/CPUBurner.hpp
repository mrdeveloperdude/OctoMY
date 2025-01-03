#ifndef CPUBURNER_HPP
#define CPUBURNER_HPP

#include "utility/Utility.hpp"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QtMath>

#include <unistd.h>


class CPUBurner
{
private:
	qint64	mwc_z;
	qint64 mwc_w;
	qint64 stressCPULoopCount;

public:

	explicit CPUBurner():
		mwc_z(362436069)
		,mwc_w(521288629)
		,stressCPULoopCount(1000)
	{
		mwc_reseed();
	}

	inline qint64 mwc(void)
	{
		mwc_z = 36969 * (mwc_z & 65535) + (mwc_z >> 16);
		mwc_w = 18000 * (mwc_w & 65535) + (mwc_w >> 16);
		return (mwc_z << 16) + mwc_w;
	}

	inline void mwc_reseed(void)
	{
		struct timeval tv;
		int i, n;
		mwc_z = 0;
		qint64 now=utility::currentMsecsSinceEpoch<quint64>();
		mwc_z = (now/1000)^ now;
		mwc_z += ~((unsigned char *)&mwc_z - (unsigned char *)&tv);
		mwc_w = (qint64)getpid() ^ (qint64)getppid()<<12;
		n = (int)mwc_z % 1733;
		for (i = 0; i < n; i++) {
			(void)mwc();
		}
	}


	inline double burnCPU(qint64 iterations)
	{
		volatile double sum=0.0;
		for (int i = 0; i < iterations; ++i) {
			sum+=qSqrt((double)mwc());
			sum-=qSqrt((double)mwc());
			sum*=0.99;
		}
		return sum;
	}


	inline void adaptiveBurnCPU(int pct)
	{
		const qint64 total_ms=1000;
		const qint64 cpu_ms=(total_ms*pct)/100;
		const qint64 start=utility::currentMsecsSinceEpoch<quint64>();
		qint64 interval=0;
		qint64 ct=0;
		volatile double sum=0.0;
		do {
			if(stressCPULoopCount>0) {
				sum+=burnCPU(stressCPULoopCount);
				sum/=stressCPULoopCount;
			}
			interval=utility::currentMsecsSinceEpoch<quint64>()-start;
			ct++;
		} while(interval<cpu_ms);
		qint64 msPer100Cycle=1;
		if(stressCPULoopCount>0) {
			msPer100Cycle=((interval*100)/(stressCPULoopCount*ct));
			if(msPer100Cycle<=0) {
				msPer100Cycle=1;
			}
		}
		qint64 nlc = cpu_ms/msPer100Cycle;
		sum/=2.0;
		stressCPULoopCount=((stressCPULoopCount*90)+(nlc*10))/100;
		const qint64 sleep_ms=total_ms-interval;
		if(sleep_ms>0) {
			QThread::msleep(sleep_ms);
		}
	}

};



#endif // CPUBURNER_HPP
