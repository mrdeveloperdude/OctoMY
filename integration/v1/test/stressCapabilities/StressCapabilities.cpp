#include "StressCapabilities.hpp"

#include "../common/CPUBurner.hpp"
#include "../common/MEMTrasher.hpp"
#include "../common/GLStressWidget.hpp"

#include <QDateTime>

void StressCapabilities::stress()
{
	Q_INIT_RESOURCE(3d);
	const bool doStress=false;
	GLStressWidget glsw;
	glsw.show();
	glsw.initTextures(true);
	const quint64 start=utility::currentMsecsSinceEpoch<quint64>();
	const quint64 stress_time=1000*60;
	mStress.setActive(doStress);
	QTimer timer;
	const quint64 loop = 1000 * 20; //Loop at X seconds
	const quint64 startTime = utility::currentMsecsSinceEpoch<quint64>();
	connect(&timer, &QTimer::timeout, this, [=]() {
		const quint64 now=utility::currentMsecsSinceEpoch<quint64>();
		const quint64 interval=(now-startTime);
		const float factor=(interval % loop) /(float)loop;
		const float angle=factor * M_PI*2.0;
		const int cpuPct=(0.5+qCos(angle*1.0)*0.5)*100;
		const int memPct=(0.5+qCos(angle*2.0)*0.5)*100;
		//qDebug()<<"Update stress values: "<<interval<<"ms interval, "<<factor<<"factor, "<<angle<<"deg angle, "<<cpuPct<<"% cpu, "<<memPct<<"% mem";
		mStress.setCPUPct(cpuPct);
		mStress.setMEMPct(memPct);
	});
	if(doStress){
		timer.start(100);
	}
	while(utility::currentMsecsSinceEpoch<quint64>()-start < stress_time) {
		qApp->processEvents();
	}
	timer.stop();
	mStress.setActive(false);
}


OC_TEST_MAIN(stress, StressCapabilities)
