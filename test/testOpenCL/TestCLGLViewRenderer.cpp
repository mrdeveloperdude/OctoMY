#include "TestCLGLViewRenderer.hpp"

#include "clt/CLWorker.hpp"
#include "clt/CLThreadManager.hpp"

#include "uptime/MethodGate.hpp"


TestCLGLViewRenderer::TestCLGLViewRenderer()
	: CLGLViewRenderer()
	, mThreadManager(nullptr)
	, mDone(true)
{
	OC_METHODGATE();
	qDebug()<<"TCLGLVR Ctor";
}


TestCLGLViewRenderer::~TestCLGLViewRenderer()
{
	OC_METHODGATE();
	mDone=true;
	qDebug()<<"TCLGLVR Dtor";
}


void TestCLGLViewRenderer::setManager(CLThreadManager *man)
{
	OC_METHODGATE();
	mThreadManager=man;
}


void TestCLGLViewRenderer::initialize(GLContext &ctx)
{
	OC_METHODGATE();
	qDebug()<<"TCLGLVR Initializing with: "<<ctx.toString();
	if(nullptr!=mThreadManager) {
		CLWorker *renderWorker=mThreadManager->renderWorker();
		if(nullptr!=renderWorker) {
			//renderWorker->set
			//
			//
			//     TODO FIGURE THIS OUT
			//
			//
			//
		} else {
			qWarning()<<"ERROR: NO render worker while initializing with context";
		}
	} else {
		qWarning()<<"ERROR: NO thread manager while initializing with context";
	}
}


void TestCLGLViewRenderer::resize(QSize sz)
{
	OC_METHODGATE();
	qDebug()<<"TCLGLVR Resizing to: "<<sz;
}


bool TestCLGLViewRenderer::setRendering(bool running, bool block)
{
	OC_METHODGATE();
	if(nullptr!=mThreadManager) {
		if(running) {
			if(!mDone) {
				qWarning()<<"TCLGLVR WARNING: Already running, skipping";
				return true;
			}
			mDone=false;
		} else {
			mDone=true;
		}
		mThreadManager->setRunning(running, block);
	} else {
		qWarning()<<"TCLGLVR ERROR: No CL thread manager while set running to: "<<running;
	}
	qDebug()<<"TCLGLVR Setting running to: "<<running;
	return running;
}


bool TestCLGLViewRenderer::isRendering() const
{
	OC_METHODGATE();
	bool ret=false;
	if(nullptr!=mThreadManager) {
		ret=mThreadManager->isRunning();
	} else {
		qWarning()<<"TCLGLVR ERROR: No CL thread manager while checking for isRunning";
	}
	qDebug()<<"TCLGLVR Returning "<<ret;
	return ret;
}


void TestCLGLViewRenderer::renderFrame()
{
	OC_METHODGATE();
	qDebug()<<"TCLGLVR Rendering frame:";
	if(nullptr!=mThreadManager) {
		CLWorker *renderWorker=mThreadManager->renderWorker();
		if(nullptr!=renderWorker) {
			//renderWorker->
		}
	}
}


GLuint TestCLGLViewRenderer::pbo()
{
	OC_METHODGATE();
	GLuint p=0;
	if(nullptr!=mThreadManager) {
		CLWorker *renderWorker=mThreadManager->renderWorker();
		if(nullptr!=renderWorker) {
			p=renderWorker->pbo();
		}
	}
	qDebug()<<"TCLGLVR Returning pbo: "<<p;
	return p;
}


QString TestCLGLViewRenderer::getRendererSpec()
{
	OC_METHODGATE();
	return "TestCLGLViewRenderer";
}

