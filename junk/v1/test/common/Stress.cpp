#include "Stress.hpp"

#include "utility/Standard.hpp"

#include <QDebug>

#include "CPUBurner.hpp"
#include "MEMTrasher.hpp"

class BurnThread : public QRunnable
{
public:
	static qint64 pct;
	static bool done;
	void run()
	{
		OC_METHODGATE();
		//qDebug() << "BurnThread START: " << QThread::currentThread();
		CPUBurner burner;
		bool isDone=false;
		do {
			isDone=done;
			burner.adaptiveBurnCPU(isDone?0:pct);
		} while(!isDone);
		//qDebug() << "BurnThread END: " << QThread::currentThread();
	}
};

class MemtrashThread : public QRunnable
{
public:
	static qint64 pct;
	static bool done;
	void run()
	{
		OC_METHODGATE();
		//qDebug() << "MemtrashThread START: " << QThread::currentThread();
		MEMTrasher trasher;
		bool isDone=false;
		do {
			isDone=done;
			trasher.adaptiveTrashMEM(isDone?0:pct);
		} while(!isDone);
		//qDebug() << "MemtrashThread END: " << QThread::currentThread();
	}
};

qint64 BurnThread::pct=0;
bool BurnThread::done=true;

qint64 MemtrashThread::pct=0;
bool MemtrashThread::done=true;

void Stress::burnOnAllCores()
{
	OC_METHODGATE();
	//Start only one trash thread
	MemtrashThread *memtrashTask = new MemtrashThread();
	tp.start((QRunnable *)memtrashTask);
	//Start one burn thread per core
	for(int i=0; i<cores; ++i) {
		BurnThread *burnTask = new BurnThread();
		// QThreadPool takes ownership and performs delete automatically
		tp.start((QRunnable *)burnTask);
	}
}

Stress::Stress() :
	QObject(0)
	, cores(QThread::idealThreadCount())
{
	OC_METHODGATE();
	tp.setMaxThreadCount(cores+1);
}


void Stress::setActive(bool active)
{
	OC_METHODGATE();
	bool wasActive=(!BurnThread::done) || (!MemtrashThread::done);
	if(wasActive!=active) {
		BurnThread::done=!active;
		MemtrashThread::done=!active;
		if(active) {
			burnOnAllCores();
		}
	}
}

void Stress::setCPUPct(qint64 pct)
{
	OC_METHODGATE();
	BurnThread::pct=pct;
	setActive(BurnThread::pct>0 && MemtrashThread::pct>0);
}


void Stress::setMEMPct(int pct)
{
	OC_METHODGATE();
	MemtrashThread::pct=pct;
	setActive(BurnThread::pct>0 && MemtrashThread::pct>0);
}
