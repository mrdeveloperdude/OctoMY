#include "GLContext.hpp"

#include "uptime/MethodGate.hpp"

#include <QOpenGLContext>
#include <QSurface>
#include <QDebug>



GLContext::GLContext(QOpenGLContext* ctx, QSurface *surf)
	: mGLCtx(ctx)
	, mSurface(surf)
{
	OC_METHODGATE();
	if(nullptr==ctx) {
		qWarning()<<"WARNING: ctx was null";
	}
}

GLContext::GLContext(const GLContext &other)
	: mGLCtx(other.mGLCtx)
	, mSurface(other.mSurface)
{
	OC_METHODGATE();
}


GLContext::~GLContext()
{
	OC_METHODGATE();
	mGLCtx=nullptr;
	mSurface=nullptr;
}


void GLContext::setSurface(QSurface &surf)
{
	OC_METHODGATE();
	this->mSurface=&surf;
}


QSurface *GLContext::surface()
{
	OC_METHODGATE();
	return mSurface;
}


QOpenGLContext *GLContext::context()
{
	OC_METHODGATE();
	return mGLCtx;
}

/*

  From http://doc.qt.io/qt-5/qopenglcontext.html#makeCurrent

Do not call this function from a different thread than the one the QOpenGLContext
instance lives in. If you wish to use QOpenGLContext from a different thread you
should first make sure its not current in the current thread, by calling
doneCurrent() if necessary. Then call moveToThread(otherThread) before using it
in the other thread.
 */

bool GLContext::setCurrent(bool current)
{
	OC_METHODGATE();
	qWarning().nospace()<<"GLCTX Setting to "<<(current?"CURRENT":"UN-CURRENT");
	bool ok=true;
	if(nullptr==mGLCtx) {
		qWarning()<<"ERROR: QOpenGLContext was null, failing 1";
		ok=false;
	} else if(current) {
		if(nullptr==mSurface) {
			qWarning()<<"ERROR: QSurface was null, failing 1";
			ok=false;
		} else {
			if(!mGLCtx->makeCurrent( mSurface )) {
				qWarning()<<"ERROR: Could not make QOpenGLContext current, failing";
				ok=false;
			}
		}
	} else {
		mGLCtx->doneCurrent();
	}
	return ok;
}


bool GLContext::moveToThread(QThread &th)
{
	OC_METHODGATE();
	bool ok=true;
	if(nullptr==mGLCtx) {
		qWarning()<<"ERROR: QOpenGLContext was null, failing 2";
		ok=false;
	} else if(nullptr==mSurface) {
		qWarning()<<"ERROR: QSurface was null, failing 2";
		ok=false;
	} else {
		//surf->moveToThread(&th);
		mGLCtx->moveToThread(&th);
		//qDebug()<<"Moved glctx to thread";
	}
	return ok;
}


static QString surfaceClassToString(QSurface::SurfaceClass c)
{
	OC_FUNCTIONGATE();
#define SURFACE_CLASS_TO_STRING_STANZA(A)	case(QSurface::A):return #A

	switch(c) {
		SURFACE_CLASS_TO_STRING_STANZA(Window);
		SURFACE_CLASS_TO_STRING_STANZA(Offscreen);
//    default:
	}
#undef SURFACE_CLASS_TO_STRING_STANZA
	return "UNKNOWN";
}


static QString surfaceTypeToString(QSurface::SurfaceType c)
{
	OC_FUNCTIONGATE();
#define SURFACE_TYPE_TO_STRING_STANZA(A)	case(QSurface::A):return #A

	switch(c) {
		SURFACE_TYPE_TO_STRING_STANZA(RasterSurface);
		SURFACE_TYPE_TO_STRING_STANZA(OpenGLSurface);
		SURFACE_TYPE_TO_STRING_STANZA(RasterGLSurface);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
		SURFACE_TYPE_TO_STRING_STANZA(OpenVGSurface);
		SURFACE_TYPE_TO_STRING_STANZA(VulkanSurface);
#endif

//	default:

	}
#undef SURFACE_TYPE_TO_STRING_STANZA
	return "UNKNOWN";
}


QString GLContext::toString()
{
	OC_METHODGATE();
	QString s;
	QDebug d= QDebug(&s).noquote().nospace();
	d << "GLCTX{ oglctx=";
	if(nullptr==mGLCtx) {
		d<<"NULL";
	} else {
		d << "QOpenGLContext(currentContext=" << mGLCtx->currentContext() << ", isOpenGLES=" << mGLCtx->isOpenGLES() << ", isValid=" << mGLCtx->isValid() << ", isWindowType=" << mGLCtx->isWindowType() << ")";
		//<<", format="<<mGLCtx->format() // USUALLY TOO VERBOSE
	}
	d<< ", surf=";
	if(nullptr==mSurface) {
		d<<"NULL";
	} else {
		d<<"QSurface( size="<<mSurface->size()<<", surfaceType="<<surfaceTypeToString(mSurface->surfaceType())<<", surfaceClass="<<surfaceClassToString(mSurface->surfaceClass())<<", supportsOpenGL="<<mSurface->supportsOpenGL()<<")";
		//<<mSurface->format() // USUALLY TOO VERBOSE
	}
	d << " }";
	return s;
}
