#include "CLWorker.hpp"
#include "CLThreadManager.hpp"

#include "../libutil/utility/GLErrors.hpp"
#include "../libutil/utility/Utility.hpp"
#include "../libclt/opencl/CLUtils.hpp"

#include <QThread>
#include <qpa/qplatformnativeinterface.h>

#include "../libutil/utility/IncludeOpenGLIntegration.hpp"

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
}


CLWorker::~CLWorker()
{
	setInitPBO(false);
}

void CLWorker::setInitPBO(const bool init)
{
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
									qDebug().noquote().nospace()<<"CLWORKER! Prepared CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::humanReadableSize(sz,2);
									if(nullptr!=mCtx) {
										mPboBuff = new cl::BufferGL(*mCtx, CL_MEM_WRITE_ONLY, mPbo);
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
									qWarning().noquote().nospace()<<"ERROR: Failed to prepare CL buffer data ("<<width<<"x"<<height <<") x 4 = "<<utility::humanReadableSize(sz,2);
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
	qDebug()<<"CLWORKER! .oOo. clCallback: "<<a<<b<<c<<d;

}





















void CLWorker::setInitCLContext(const bool init)
{
	qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" CL CONTEXT FOR INDEX "<<mIndex;
	if(init) {
		if(nullptr!=mDev) {
			VECTOR_CLASS<cl::Device> devices;
			devices.push_back(*mDev);
			cl::Platform platform = (cl::Platform)mDev->getInfo<CL_DEVICE_PLATFORM>();
			cl_context_properties * properties=nullptr;
			// Extra properties are adde when doing OpenCL/OpenGL interoperability
			if (isGLInteropWorker()) {
				GLContext *rctx=sharingGLContext();
				if (nullptr!=rctx) {
					rctx->currentize();
					qDebug()<<"CLWORKER! ADDING SHARING CTX TO CL PROPERTIES: "<<rctx->toString();

					initializeOpenGLFunctions();

					auto ctx=rctx->context();
/*
#################################################
					#################################################
					#################################################
					#################################################
					#################################################
					#################################################
					#################################################
					#################################################
BIG IDEA: put all openCL and context code in one bigg "utility" object that gets called from everywhere. Allows for testing code easier (whithout running up a whol bunch of unrelated stuff)
*/

					const cl_platform_id platformID=platform();

					cl_context_properties contextProps[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platformID,
															 CL_GL_CONTEXT_KHR, 0,
															 0, 0,
															 0
														   };
					QPlatformNativeInterface *nativeIf = qGuiApp->platformNativeInterface();
					void *dpy = nativeIf->nativeResourceForIntegration(QByteArrayLiteral("egldisplay")); // EGLDisplay
					if (nullptr!=dpy) {
						void *nativeContext = nativeIf->nativeResourceForContext("eglcontext", ctx);
						if (!nativeContext) {
							qWarning("ERROR: Failed to get the underlying EGL context from the current QOpenGLContext");
						} else {
							qDebug()<<"! ! ! GOT NATIVE EGL CONTEXT";
						}
						contextProps[3] = (cl_context_properties) nativeContext;
						contextProps[4] = CL_EGL_DISPLAY_KHR;
						contextProps[5] = (cl_context_properties) dpy;
					} else {
						dpy = nativeIf->nativeResourceForIntegration(QByteArrayLiteral("display")); // Display *
						void *nativeContext = nativeIf->nativeResourceForContext("glxcontext", ctx);
						if (!nativeContext) {
							qWarning("ERROR: Failed to get the underlying GLX context from the current QOpenGLContext");
						} else {
							qDebug()<<"! ! ! GOT NATIVE GLX CONTEXT";
						}
						contextProps[3] = (cl_context_properties) nativeContext;
						contextProps[4] = CL_GLX_DISPLAY_KHR;
						contextProps[5] = (cl_context_properties) dpy;
					}

					/*
					//Build context dynamically based on returned values for GLX context and display.
					cl_context_properties cps[7]= {0};
					const GLXContext glxCtx=glXGetCurrentContext();
					const Display* glxDisp=glXGetCurrentDisplay();
					const cl_platform_id platformID=platform();
					// Build properties list
					int i=0;
					if(0!=glxCtx) {
						qDebug()<<"CLWORKER! Added property: GLX enabled";
						cps[i++]=CL_GL_CONTEXT_KHR;
						cps[i++]= (cl_context_properties)(glxCtx);
					} else {
						qDebug()<<"CLWORKER! Added property: GLX disabled";
					}
					if(0!=glxDisp) {
						qDebug()<<"CLWORKER! Added property: GLX Display enabled";
						cps[i++]=CL_GLX_DISPLAY_KHR;
						cps[i++]= (cl_context_properties)(glxDisp);
					} else {
						qDebug()<<"CLWORKER! Added property: GLX Display disabled";
					}
					if(0!=platformID) {
						qDebug()<<"CLWORKER! Added property: Platform enabled with ID "<<platformID;
						cps[i++]=CL_CONTEXT_PLATFORM;
						cps[i++]=(cl_context_properties)(platformID);
					} else {
						qDebug()<<"CLWORKER! Added property: Platform disabled";
					}
					//Terminate with 0
					cps[i++]=0;
					qDebug()<<"CLWORKER! Put a total of "<<i <<" properties in the CL context";
					*/
					properties=contextProps;

					qDebug()<<"CLWORKER! Creating the CL Context";
					mCtx = new cl::Context(devices, properties, &clCallback);
					if(nullptr==mCtx) {
						qWarning()<<"CLWORKER! ERROR: Could not create CL context";
					}
				}
			} else {
				qWarning()<<"CLWORKER! ERROR: no sharing GL context";
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










void CLWorker::setInitCLContextOld(const bool init)
{
	qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" CL CONTEXT FOR INDEX "<<mIndex;
	if(init) {
		if(nullptr!=mDev) {
			VECTOR_CLASS<cl::Device> devices;
			devices.push_back(*mDev);
			cl::Platform platform = (cl::Platform)mDev->getInfo<CL_DEVICE_PLATFORM>();
			cl_context_properties * properties=nullptr;
			// Extra properties are adde when doing OpenCL/OpenGL interoperability
			if (isGLInteropWorker()) {
				GLContext *rctx=sharingGLContext();
				if (nullptr!=rctx) {
					rctx->currentize();
					qDebug()<<"CLWORKER! ADDING SHARING CTX TO CL PROPERTIES: "<<rctx->toString();

					initializeOpenGLFunctions();
					//Build context dynamically based on returned values for GLX context and display.
					cl_context_properties cps[7]= {0};
					const GLXContext glxCtx=glXGetCurrentContext();
					const Display* glxDisp=glXGetCurrentDisplay();
					const cl_platform_id platformID=platform();
					// Build properties list
					int i=0;
					if(0!=glxCtx) {
						qDebug()<<"CLWORKER! Added property: GLX enabled";
						cps[i++]=CL_GL_CONTEXT_KHR;
						cps[i++]= (cl_context_properties)(glxCtx);
					} else {
						qDebug()<<"CLWORKER! Added property: GLX disabled";
					}
					if(0!=glxDisp) {
						qDebug()<<"CLWORKER! Added property: GLX Display enabled";
						cps[i++]=CL_GLX_DISPLAY_KHR;
						cps[i++]= (cl_context_properties)(glxDisp);
					} else {
						qDebug()<<"CLWORKER! Added property: GLX Display disabled";
					}
					if(0!=platformID) {
						qDebug()<<"CLWORKER! Added property: Platform enabled with ID "<<platformID;
						cps[i++]=CL_CONTEXT_PLATFORM;
						cps[i++]=(cl_context_properties)(platformID);
					} else {
						qDebug()<<"CLWORKER! Added property: Platform disabled";
					}
					//Terminate with 0
					cps[i++]=0;
					qDebug()<<"CLWORKER! Put a total of "<<i <<" properties in the CL context";
					properties=cps;
				}
				qDebug()<<"CLWORKER! Creating the CL Context";
				mCtx = new cl::Context(devices, properties, &clCallback);
				if(nullptr==mCtx) {
					qWarning()<<"CLWORKER! ERROR: Could not create CL context";
				}
			} else {
				qWarning()<<"CLWORKER! ERROR: no sharing GL context";
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

/*
initContext();
initBuffers();
initPBO();
initParameters();
initKernels();
*/

void CLWorker::setInit(const bool init)
{
	if(mIsInited!=init) {
		qDebug().nospace()<<"CLWORKER! "<<(init?"INITIALIZING":"DE-INITIALIZING")<<" FOR INDEX "<<mIndex;
		if(!mIsInited) {
			setInitCLContext(init);
			//initBuffers();
			setInitPBO(init);
			//initParameters();
			//initKernels();
		} else {
			//initKernels();
			//initParameters();
			setInitPBO(init);
			//initBuffers();
			setInitCLContext(init);
		}
		mIsInited=init;
	} else {
		qWarning()<<"CLWORKER! WARNING: Double "<<(init?"init":"de-init")<<" attempted";
	}
}


void CLWorker::setRunning(const bool running)
{
	mRunning=running;
}

bool CLWorker::isRunning() const
{
	return mRunning;
}

bool CLWorker::isGLInteropWorker() const
{
	//If OpenGL interop is enabled, and we are the first worker (index=0) that means we have the responsibility of rendering our results to PBO
	const bool doInterop=mManager.interopConfig().doGLInterop();
	return doInterop&&(0==mIndex);
}
QThread *CLWorker::thread() const
{
	return mThread;
}

int CLWorker::index() const
{
	return mIndex;
}

GLuint CLWorker::pbo() const
{
	return mPbo;
}

cl::BufferGL *CLWorker::pboBuff() const
{
	return mPboBuff;

}

CLThreadManager &CLWorker::manager() const
{
	return mManager;
}


cl::Context *CLWorker::clContext() const
{
	return mCtx;
}

const cl::Device *CLWorker::clDevice() const
{
	return mDev;
}

GLContext *CLWorker::sharingGLContext() const
{
	return mManager.interopConfig().sharingContext();
}

// CLWorker interface wrappers
//////////////////////////////////////////





void CLWorker::preProcess(QThread &th)
{
	mThread=&th;
	GLContext *sc=manager().interopConfig().sharingContext();
	const bool doInterop=isGLInteropWorker();
	qDebug()<<"CLWORKER! PRE start in thread " <<QThread::currentThreadId()<< " for device " << CLDeviceToString(mDev)<<" with doRender="<<doInterop;
	if(doInterop) {
		QThread *ct=QThread::currentThread();
		if(nullptr!=ct) {
			if(nullptr!=mThread) {
				qDebug()<<"CLWORKER! Moving sharing context to worker thread";
				sc->uncurrentize();
				sc->moveToThread(*mThread);
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
	qDebug()<<"CLWORKER! PROC start in thread " <<QThread::currentThreadId();
	setInit(true);
	processImp();
	setInit(false);
	qDebug()<<"CLWORKER! PROC end in thread " <<QThread::currentThreadId();
	emit processFinished();
}

void CLWorker::postProcess()
{
	const bool doInterop=mManager.interopConfig().doGLInterop();
	const bool doRender=isGLInteropWorker();
	qDebug()<<"CLWORKER! POST start in thread " <<QThread::currentThreadId()<< " with GL-INTEROP="<<doInterop<<" AND doRender="<<doRender;
	postProcessImp();
	if(doRender) {
		setInitPBO(false);
	}
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
