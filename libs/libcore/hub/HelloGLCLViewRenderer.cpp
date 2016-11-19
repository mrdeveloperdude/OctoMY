#include "HelloGLCLViewRenderer.hpp"

#ifdef EXTERNAL_LIB_OPENCL

#include "HelloCLWorkerFactory.hpp"
#include "../libclt/opencl/CLThreadManager.hpp"


HelloGLCLViewRenderer::HelloGLCLViewRenderer()
	: mPBO(0)
	, mThreadManager(nullptr)
{

}

HelloGLCLViewRenderer::~HelloGLCLViewRenderer()
{
	delete mThreadManager;
	mThreadManager=nullptr;
}

void HelloGLCLViewRenderer::initialize(GLContext &ctx)
{
	qDebug()<<"Initializing with: "<<ctx.toString();
	//ctx.currentize();
	HelloCLWorkerFactory *factory=new HelloCLWorkerFactory();
	if(nullptr!=factory) {
		QSurface *surf=ctx.surface();
		if(nullptr!=surf) {
			CLGLInteropConfig config(true, 512, 512 );
			mThreadManager=new CLThreadManager(*factory,config, "", false,true);
			if(nullptr==mThreadManager) {
				qWarning()<<"ERROR: Could not allocate CL thread manager";
			}
		} else {
			qWarning()<<"ERROR: context had no surface";
		}
	} else {
		qWarning()<<"ERROR: Could not allocate CL worker factory";
	}
	//ctx.uncurrentize();
}


void HelloGLCLViewRenderer::resize(QSize sz)
{
	qDebug()<<"Resizing to: "<<sz;
}

bool HelloGLCLViewRenderer::setRunning(bool running, bool block)
{
	if(nullptr!=mThreadManager) {
		mThreadManager->setRunning(running, block);
	} else {
		qWarning()<<"ERROR: No CL thread manager while set running";
	}
	qDebug()<<"Setting running to: "<<running;
	return running;
}


bool HelloGLCLViewRenderer::isRunning() const
{
	bool ret=false;
	if(nullptr!=mThreadManager) {
		ret=mThreadManager->isRunning();
	} else {
		qWarning()<<"ERROR: No CL thread manager while checking for isRunning";
	}
	qDebug()<<"Returning "<<ret;
	return ret;
}
void HelloGLCLViewRenderer::renderFrame()
{
	qDebug()<<"Rendering frame :-)";
}

GLuint HelloGLCLViewRenderer::pbo()
{
	qDebug()<<"Returning pbo: "<<mPBO;
	return mPBO;
}


QString HelloGLCLViewRenderer::getRendererSpec()
{
	return "HelloGLCLViewRenderer";
}

#endif // EXTERNAL_LIB_OPENCL
