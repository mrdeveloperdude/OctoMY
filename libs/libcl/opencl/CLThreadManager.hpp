#ifndef CLTHREADMANAGER_HPP
#define CLTHREADMANAGER_HPP

#include "CLDeviceSelection.hpp"
#include <QObject>
#include <QVector>
#include <QString>

class QThread;
class CLWorker;
class CLWorkerFactory;

/*!
 * \brief The CLThreadManager class is designed to start one thread per OpenCL
 * device matching the query passed as arbuemtns to the constructor.
 *
 * Worker classes are created one per thread to carry out the OpenCL processing
 * and their running status and lifecycle is maintained.
 */
class CLThreadManager : public QObject
{
	Q_OBJECT
private:
	CLWorkerFactory &mFactory;
	CLDeviceSelection mDeviceSelection;
	QVector<CLWorker *> mWorkers;
	QVector<QThread *> mThreads;


public:
	explicit CLThreadManager(CLWorkerFactory &factory, QString deviceSelectionString="", bool allowGPU=true, bool allowCPU=false, QObject *parent = nullptr);

	virtual ~CLThreadManager();

public:

	bool isRunning();
	void setRunning(bool running, bool block=false);
};

#endif // CLTHREADMANAGER_HPP
