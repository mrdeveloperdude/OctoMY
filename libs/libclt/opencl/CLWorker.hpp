#ifndef CLWORKER_HPP
#define CLWORKER_HPP

#include "../libclt/opencl/CLInclude.hpp"

#include "../libutil/utility/IncludeOpenGL.hpp"

#include <QObject>
#include <QDebug>

class QThread;
class CLThreadManager;



/*!
 * \brief The CLWorker class is a companion class to CLThreadManager that is
 * meant to be inherited from by implementations that wish to carry out work in
 * OpnCL devices. Each instance will run on one device, and will be managed by
 * the CLThereadManager.
 */
class CLWorker : public QObject, protected OCTOMY_QT_OGL_FUNCTIONS_CLASS
{
	Q_OBJECT
private:
	CLThreadManager &mManager;
	int mIndex;
	const cl::Device *mDev;
	bool mRunning;
	QThread *mThread;
	GLuint mPbo;
	cl::BufferGL *mPboBuff;


public:
	explicit CLWorker(CLThreadManager &man, int index, QObject *parent = nullptr);
	virtual	~CLWorker();

public:

	void setRunning(bool running);
	bool isRunning() const;

	QThread *thread() const;

	int index();

private:

	void deInitPBO();
	void initPBO();

	// CLWorker interface wrappers
	//////////////////////////////////////////

public slots:
	void preProcess(QThread &th);
	void process();
	void postProcess();

	// CLWorker interface
	//////////////////////////////////////////

public:
	virtual void preProcessImp();
	virtual void processImp();
	virtual void postProcessImp();

signals:

	void processFinished();

};

#endif // CLWORKER_HPP
