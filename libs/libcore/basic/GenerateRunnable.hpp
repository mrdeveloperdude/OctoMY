#ifndef GENERATERUNNABLE_HPP
#define GENERATERUNNABLE_HPP

#include <QRunnable>
#include <QThread>

#include <QDebug>

template<typename T>
class GenerateRunnable : public QRunnable{
	private:
		T &ks;
	public:
		GenerateRunnable(T &ks)
			: ks(ks)
		{
			//qDebug() << "GENERATE RUNNABLE CTOR";
		}
	public:
		void run() {
			//qDebug() << "GENERATE RUNNABLE start bootstrap @ " << QThread::currentThread();
			ks.bootstrapWorker();
			//qDebug() << "GENERATE RUNNABLE end bootstrap @ " << QThread::currentThread();
		}
};

#endif // GENERATERUNNABLE_HPP

