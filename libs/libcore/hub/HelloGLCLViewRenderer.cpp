#include "HelloGLCLViewRenderer.hpp"

#include "HelloCLWorkerFactory.hpp"
#include "../libclt/opencl/CLThreadManager.hpp"


HelloGLCLViewRenderer::HelloGLCLViewRenderer()
	: mRunning(false)
	, mPBO(0)
	, mThreadManager(nullptr)
{

}

HelloGLCLViewRenderer::~HelloGLCLViewRenderer()
{

}

void HelloGLCLViewRenderer::initialize(GLContext &ctx)
{
	qDebug()<<"Initializing with: "<<ctx.toString();

	HelloCLWorkerFactory *factory=new HelloCLWorkerFactory();
	if(nullptr!=factory) {
		mThreadManager=new CLThreadManager(*factory,"",true,true);
		if(nullptr!=mThreadManager) {
			mThreadManager->setRunning(true);
		} else {
			qWarning()<<"ERROR: Could not allocate CL thread manager";
		}
	} else {
		qWarning()<<"ERROR: Could not allocate CL worker factory";
	}

}

void HelloGLCLViewRenderer::resize(QSize sz)
{
	qDebug()<<"Resizing to: "<<sz;
}

bool HelloGLCLViewRenderer::setRunning(bool running, bool block)
{
	Q_UNUSED(block);
	mRunning=running;
	qDebug()<<"Setting running to: "<<mRunning;
	return mRunning;
}


bool HelloGLCLViewRenderer::isRunning() const
{
	qDebug()<<"Returning "<<mRunning;
	return mRunning;
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
