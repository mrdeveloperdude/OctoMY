#include "GLContext.hpp"

#include <QOpenGLContext>
#include <QSurface>
#include <QDebug>



GLContext::GLContext(QOpenGLContext* glctx, QSurface *surf)
	: glctx(glctx)
	, surf(surf)
{

}

GLContext::GLContext(GLContext &other)
	: glctx(other.glctx)
	, surf(other.surf)
{

}


GLContext::~GLContext()
{
	glctx=nullptr;
	surf=nullptr;
}


void GLContext::setSurface(QSurface &surf)
{
	this->surf=&surf;
}

/*

  From http://doc.qt.io/qt-5/qopenglcontext.html#makeCurrent

Do not call this function from a different thread than the one the QOpenGLContext
instance lives in. If you wish to use QOpenGLContext from a different thread you
should first call make sure it's not current in the current thread, by calling
doneCurrent() if necessary. Then call moveToThread(otherThread) before using it
in the other thread.
 */

void GLContext::currentize()
{
	if(nullptr==glctx) {
		qWarning()<<"GLCTX was null 1";
		exit(1);
	} else if(nullptr==surf) {
		qWarning()<<"surf was null 1";
		exit(1);
	} else {
		if(!glctx->makeCurrent( surf )) {
			qWarning()<<"Could not make current";
			exit(1);
		}
	}
}
void GLContext::uncurrentize()
{
	if(nullptr==glctx) {
		qWarning()<<"GLCTX was null 2";
		exit(1);
	} else {
		glctx->doneCurrent();
	}
}


void GLContext::moveToThread(QThread &th)
{
	if(nullptr==glctx) {
		qWarning()<<"GLCTX was null 3";
		exit(1);
	} else if(nullptr==surf) {
		qWarning()<<"surf was null 2";
		exit(1);
	} else {
		//surf->moveToThread(&th);
		glctx->moveToThread(&th);
		qDebug()<<"Moved glctx to thread";
	}
}




QString GLContext::toString()
{
	return "GLCTX";
}
