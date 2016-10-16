#ifndef CLWORKER_HPP
#define CLWORKER_HPP

#include <QObject>
#include <QDebug>

class QThread;

/*!
 * \brief The CLWorker class is a companion class to CLThreadManager that is
 * meant to be inherited from by implementations that wish to carry out work in
 * OpnCL devices. Each instance will run on one device, and will be managed by
 * the CLThereadManager.
 */
class CLWorker : public QObject
{
	Q_OBJECT
private:
	bool mRunning;

public:
	explicit CLWorker(QObject *parent = 0);

public:

	void setRunning(bool running);
	bool isRunning() const;

public slots:


	virtual void preProcess(QThread &th);
	virtual void process();
	virtual void postProcess();

signals:

	void processFinished();

};

#endif // CLWORKER_HPP
