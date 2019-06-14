#include "CLWorker.hpp"
#include "CLThreadManager.hpp"

#include "uptime/New.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/string/String.hpp"

#include "glt/GLErrors.hpp"
#include "clt/CLUtils.hpp"

#include <QThread>
#include <qpa/qplatformnativeinterface.h>

#include "glt/IncludeOpenGLIntegration.hpp"


CLWorker::CLWorker(CLThreadManager &man, int index, QObject *parent)
	: QObject(parent)
	, mManager(man)
	, mIndex (index)
	, mDev (man.device(index))
	, mRunning(true)
	, mThread(nullptr)
	, mPbo(0)
	, mPboBuff(nullptr)
	, mCtx(nullptr)
	, mIsInited(false)
{
	OC_METHODGATE();
}


CLWorker::~CLWorker()
{
	OC_METHODGATE();
	setInitPBO(false);
}


void CLWorker::setInitPBO(const bool init)
{
	OC_METHODGATE();
	const bool doInterop=isGLInteropWorker();
	qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" PBO FOR INDEX "<<mIndex<<"( INTEROP=" << doInterop << " )";
	if (doInterop) {
		if(init) {
			if(0==mPbo) {
				if(nullptr!=mDev) {
					const CLGLInteropConfig config=mManager.interopConfig();
					if(config.doGLInterop()) {
						glGenBuffers(1, &mPbo);
						GLSPEWERROR;
						if(mPbo>0) {
							qDebug()<<"CLWORKER! Created PBO ("<<mPbo<<") ";
							glBindBuffer(GL_ARRAY_BUFFER, mPbo);
							if(!GLSPEWERROR) {
								qDebug()<<"CLWORKER! Bound PBO ("<<mPbo<<") ";
								const unsigned int width = config.width();
								const unsigned int height = config.height();
								GLsizeiptr sz=width * height * sizeof(GLubyte) * 4;
								glBufferData(GL_ARRAY_BUFFER, sz, nullptr, GL_STREAM_DRAW);
								if(!GLSPEWERROR) {
									qDebug().noquote().nospace()<<"CLWORKER! Prepared CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::string::humanReadableSize(sz,2);
									if(nullptr!=mCtx) {
										mPboBuff = OC_NEW cl::BufferGL(*mCtx, CL_MEM_WRITE_ONLY, mPbo);
										if(!GLSPEWERROR) {
											qDebug().noquote().nospace()<<"CLWORKER! Created CL buffer ("<<width<<"x"<<height <<") ";
										} else {
											qWarning()<<"ERROR: Failed to create CL buffer for PBO";
										}
										glBindBuffer(GL_ARRAY_BUFFER, 0);
										if(!GLSPEWERROR) {
										} else {
											qWarning()<<"ERROR: Failed to un-bind PBO";
										}
									} else {
										qWarning()<<"ERROR: ctx == null";
									}
								} else {
									qWarning().noquote().nospace()<<"ERROR: Failed to prepare CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::string::humanReadableSize(sz,2);
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
		} else {
			if(0!=mPbo) {
				qDebug()<<"Trying to free PBO "<<QString::number(mPbo);
				glDeleteBuffers(1, &mPbo);
				mPbo=0;
			}
			delete mPboBuff;
			mPboBuff=nullptr;
		}
	}
}


static void CL_CALLBACK clCallback( const char *a, const void *b, ::size_t c, void *d)
{
	OC_FUNCTIONGATE();
	qDebug()<<"CLWORKER! .oOo. clCallback: "<<a<<b<<c<<d;

}


void CLWorker::setInitCLContext(const bool init)
{
	OC_METHODGATE();
	qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" CL CONTEXT FOR INDEX "<<mIndex;
	if(init) {
		if(nullptr!=mDev) {
			cl::Platform platform = (cl::Platform)mDev->getInfo<CL_DEVICE_PLATFORM>();
			// Extra properties are adde when doing OpenCL/OpenGL interoperability
			if (isGLInteropWorker()) {
				GLContext *rctx=sharingGLContext();
				if (nullptr!=rctx) {
					const bool currentOK=rctx->setCurrent(true);
					if(currentOK) {
						qDebug()<<"CLWORKER! ADDING SHARING CTX TO CL PROPERTIES: "<<rctx->toString();
						QOpenGLContext *sharingContext=rctx->context();
						if(nullptr != sharingContext) {
							const cl_platform_id platformID=platform();
							cl_context_properties *properties=mCL.generateProperties(platformID, sharingContext);
							qDebug()<<"CLWORKER! Creating the CL Context";
							VECTOR_CLASS<cl::Device> devices;
							devices.push_back(*mDev);
							mCtx = OC_NEW cl::Context(devices, properties, &clCallback);
							if(nullptr==mCtx) {
								qWarning()<<"CLWORKER! ERROR: Could not create CL context";
							}
							initializeOpenGLFunctions();
						} else {
							qWarning()<<"CLWORKER! ERROR: no sharing QOpenGLContext";
						}
					} else {
						qWarning()<<"CLWORKER! ERROR: could not set sharing context to current";
					}
				}
			} else {
				qWarning()<<"CLWORKER! ERROR: no sharing GLcontext";
			}
		} else {
			qWarning()<<"CLWORKER! ERROR: no CL device";
		}
	} else {
		if(nullptr!=mCtx) {
			delete mCtx;
			mCtx=nullptr;
		} else {
			qWarning()<<"CLWORKER! ERROR: no CL context";
		}
	}
}


void CLWorker::setInit(const bool init)
{
	OC_METHODGATE();
	if(mIsInited!=init) {
		qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" FOR INDEX "<<mIndex;
		if(!mIsInited) {
			setInitCLContext(init);
			setInitPBO(init);
		} else {
			setInitPBO(init);
			setInitCLContext(init);
		}
		mIsInited=init;
	} else {
		qWarning()<<"CLWORKER! WARNING: Double "<<(init?"init":"de-init")<<" attempted";
	}
}


void CLWorker::setRunning(const bool running)
{
	OC_METHODGATE();
	mRunning=running;
}


bool CLWorker::isRunning() const
{
	OC_METHODGATE();
	return mRunning;
}


bool CLWorker::isGLInteropWorker() const
{
	OC_METHODGATE();
	//If OpenGL interop is enabled, and we are the first worker (index=0) that means we have the responsibility of rendering our results to PBO
	const bool doInterop=mManager.interopConfig().doGLInterop();
	return doInterop&&(0==mIndex);
}


QThread *CLWorker::thread() const
{
	OC_METHODGATE();
	return mThread;
}


int CLWorker::index() const
{
	OC_METHODGATE();
	return mIndex;
}


GLuint CLWorker::pbo() const
{
	OC_METHODGATE();
	return mPbo;
}


cl::BufferGL *CLWorker::pboBuff() const
{
	OC_METHODGATE();
	return mPboBuff;
}


CLThreadManager &CLWorker::manager() const
{
	OC_METHODGATE();
	return mManager;
}


cl::Context *CLWorker::clContext() const
{
	OC_METHODGATE();
	return mCtx;
}


const cl::Device *CLWorker::clDevice() const
{
	OC_METHODGATE();
	return mDev;
}


GLContext *CLWorker::sharingGLContext() const
{
	OC_METHODGATE();
	return mManager.interopConfig().sharingContext();
}


// CLWorker interface wrappers
//////////////////////////////////////////


void CLWorker::preProcess(QThread &th)
{
	OC_METHODGATE();
	mThread=&th;
	GLContext *sharingContext=manager().interopConfig().sharingContext();
	const bool doInterop=isGLInteropWorker();
	qDebug()<<"CLWORKER! PRE start in thread " <<QThread::currentThreadId()<< " for device " << CLDeviceToString(mDev)<<" with doRender="<<doInterop;
	if(doInterop) {
		QThread *currentThread=QThread::currentThread();
		if(nullptr!=currentThread) {
			if(nullptr!=mThread) {
				qDebug()<<"CLWORKER! Moving sharing context to worker thread";
				const bool currentOK=sharingContext->setCurrent(false);
				if(!currentOK) {
					qWarning()<<"ERROR: Could not set sharing context to un-current";
				}
				const bool moveOK=sharingContext->moveToThread(*mThread);
				if(!moveOK) {
					qWarning()<<"ERROR: Could not move worker to thread";
				}
			} else {
				qWarning()<<"ERROR: No thread for worker";
			}
		} else {
			qWarning()<<"ERROR: No current thread found";
		}
	}
	preProcessImp();
	qDebug()<<"CLWORKER! PRE end in thread " <<QThread::currentThreadId();
}


void CLWorker::process()
{
	OC_METHODGATE();
	qDebug()<<"CLWORKER! PROC start in thread " <<QThread::currentThreadId();
	setInit(true);
	processImp();
	setInit(false);
	qDebug()<<"CLWORKER! PROC end in thread " <<QThread::currentThreadId();
	emit processFinished();
}


void CLWorker::postProcess()
{
	OC_METHODGATE();
	const bool doInterop=mManager.interopConfig().doGLInterop();
	const bool doRender=isGLInteropWorker();
	qDebug()<<"CLWORKER! POST start in thread " <<QThread::currentThreadId()<< " with GL-INTEROP="<<doInterop<<" AND doRender="<<doRender;
	postProcessImp();
	if(doRender) {
		setInitPBO(false);
	}
	qDebug()<<"CLWORKER! POST end in thread " <<QThread::currentThreadId();
}
