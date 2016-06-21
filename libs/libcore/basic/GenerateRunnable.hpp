#ifndef GENERATERUNNABLE_HPP
#define GENERATERUNNABLE_HPP

#include <QRunnable>

template<typename T>
class GenerateRunnable : public QRunnable{
	private:
		T &ks;
	public:
		GenerateRunnable(T &ks)
			: ks(ks)
		{

		}
	public:
		void run() {
			//qDebug() << "Started keystore bootstrap @ " << QThread::currentThread();
			ks.bootstrapWorker();
			//qDebug() << "Ended keystore bootstrap @ " << QThread::currentThread()<<" with result: " << ks.getLocalPublicKey();
		}
};

#endif // GENERATERUNNABLE_HPP
