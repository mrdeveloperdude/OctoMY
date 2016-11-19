#include "CLWorker.hpp"
#include "CLThreadManager.hpp"

#include "../libutil/utility/GLErrors.hpp"
#include "../libutil/utility/Utility.hpp"
#include "../libclt/opencl/CLUtils.hpp"

#include <QThread>



CLWorker::CLWorker(CLThreadManager &man, int index, QObject *parent)
	: QObject(parent)
	, mManager(man)
	, mIndex (index)
	, mDev (man.device(index))
	, mRunning(true)
	, mThread(nullptr)
	, mPbo(0)
	, mPboBuff(nullptr)
{
}


CLWorker::~CLWorker()
{
	deInitPBO();
}

void CLWorker::deInitPBO()
{
	if(0!=mPbo) {
		qDebug()<<"Trying to free PBO "<<QString::number(mPbo);
		glDeleteBuffers(1, &mPbo);
		mPbo=0;
	}
	delete mPboBuff;
	mPboBuff=nullptr;
}

void CLWorker::initPBO()
{
	if(0==mPbo) {
		if(nullptr!=mDev) {
			const CLGLInteropConfig config=mManager.interopConfig();
			if(config.doGLInterop()) {
				glGenBuffers(1, &mPbo);
				GLSPEWERROR;
				if(mPbo>0) {
					qDebug()<<"Created PBO ("<<mPbo<<") ";
					glBindBuffer(GL_ARRAY_BUFFER, mPbo);
					if(!GLSPEWERROR) {
						qDebug()<<"Bound PBO ("<<mPbo<<") ";
						const unsigned int width = config.width();
						const unsigned int height = config.height();
						GLsizeiptr sz=width * height * sizeof(GLubyte) * 4;
						glBufferData(GL_ARRAY_BUFFER, sz, nullptr, GL_STREAM_DRAW);
						if(!GLSPEWERROR) {
							qDebug()<<"Prepared CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::humanReadableSize(sz,2);
							mPboBuff = new cl::BufferGL(*mDev, CL_MEM_WRITE_ONLY, mPbo);
							if(!GLSPEWERROR) {
								qDebug()<<"Created CL buffer ("<<width<<"x"<<height <<") ";
							} else {
								qWarning()<<"ERROR: Failed to create CL buffer for PBO";
							}
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							if(!GLSPEWERROR) {
							} else {
								qWarning()<<"ERROR: Failed to un-bind PBO";
							}
						} else {
							qWarning()<<"ERROR: Failed to prepare CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::humanReadableSize(sz,2);
						}
					} else {
						qWarning()<<"ERROR: Failed to bind PBO";
					}
				} else {
					qWarning()<<"ERROR: Could not generate PBO";
				}
			}

		} else {
			qWarning()<<"ERROR: No device when creating PBO";
		}
	}
}



void CLWorker::setRunning(bool running)
{
	mRunning=running;
}

bool CLWorker::isRunning() const
{
	return mRunning;
}


QThread *CLWorker::thread() const
{
	return mThread;
}

int CLWorker::index()
{
	return mIndex;
}

// CLWorker interface wrappers
//////////////////////////////////////////


void CLWorker::preProcess(QThread &th)
{
	mThread=&th;
	qDebug()<<"CLWORKER! PRE start in thread " <<QThread::currentThreadId()<< " for device " << CLDeviceToString(mDev);
	initializeOpenGLFunctions();
	initPBO();
	preProcessImp();
	qDebug()<<"CLWORKER! PRE end in thread " <<QThread::currentThreadId();
}

void CLWorker::process()
{
	qDebug()<<"CLWORKER! PROC start in thread " <<QThread::currentThreadId();
	processImp();
	qDebug()<<"CLWORKER! PROC end in thread " <<QThread::currentThreadId();
	emit processFinished();
}

void CLWorker::postProcess()
{
	qDebug()<<"CLWORKER! POST start in thread " <<QThread::currentThreadId();
	processImp();
	deInitPBO();
	qDebug()<<"CLWORKER! POST end in thread " <<QThread::currentThreadId();
}


// CLWorker interface
//////////////////////////////////////////


void CLWorker::preProcessImp()
{
	qWarning()<<"preProcess not implemented";
}

void CLWorker::processImp()
{
	qWarning()<<"process not implemented";
}

void CLWorker::postProcessImp()
{
	qWarning()<<"postProcess not implemented";
}
