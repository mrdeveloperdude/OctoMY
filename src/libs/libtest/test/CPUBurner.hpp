#ifndef CPUBURNER_HPP
#define CPUBURNER_HPP

#include "utility/time/HumanTime.hpp"
#include "oc_getppid.hpp"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QtMath>



class CPUBurner
{
private:
	qint64	mwc_z;
	qint64 mwc_w;
	qint64 stressCPULoopCount;

public:

	explicit CPUBurner():
		  mwc_z(362436069)
		, mwc_w(521288629)
		, stressCPULoopCount(1000)
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
		qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();
		mwc_z = (now/1000)^ now;
		//mwc_z += ~((unsigned char *)&mwc_z - (unsigned char *)&tv);
		mwc_z += ~(reinterpret_cast<unsigned char *>(&mwc_z) - reinterpret_cast<unsigned char *>(&tv));
		mwc_w = static_cast<qint64>(oc_getpid()) ^ static_cast<qint64>(oc_getppid())<<12;
		n = static_cast<int>(mwc_z) % 1733;
		for (i = 0; i < n; i++) {
			(void)mwc();
		}
	}


	inline double burnCPU(qint64 iterations)
	{
		volatile double sum=0.0;
		for (int i = 0; i < iterations; ++i) {
			sum+=qSqrt(static_cast<qreal>(mwc()));
			sum-=qSqrt(static_cast<qreal>(mwc()));
			sum*=0.99;
		}
		return sum;
	}


	inline void adaptiveBurnCPU(qint64 pct)
	{
		const qint64 total_ms=1000;
		const qint64 cpu_ms=(total_ms*pct)/100;
		const qint64 start=utility::time::currentMsecsSinceEpoch<qint64>();
		qint64 interval=0;
		qint64 ct=0;
		volatile double sum=0.0;
		do {
			if(stressCPULoopCount > 0) {
				sum += burnCPU(stressCPULoopCount);
				sum /= stressCPULoopCount;
			}
			interval = utility::time::currentMsecsSinceEpoch<qint64>() - start;
			ct++;
		} while(interval<cpu_ms);
		qint64 msPer100Cycle = 1;
		if(stressCPULoopCount > 0) {
			msPer100Cycle = ((interval*100)/(stressCPULoopCount*ct));
			if(msPer100Cycle <= 0) {
				msPer100Cycle = 1;
			}
		}
		qint64 nlc = cpu_ms / msPer100Cycle;
		sum /= 2.0;
		(void)sum;
		stressCPULoopCount = ((stressCPULoopCount*90)+(nlc*10))/100;
		const qint64 sleep_ms = total_ms-interval;
		if(sleep_ms > 0) {
			QThread::msleep(static_cast<unsigned long>(sleep_ms));
		}
	}

};


#endif
// CPUBURNER_HPP
