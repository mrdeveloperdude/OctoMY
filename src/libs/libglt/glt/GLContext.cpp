#include "GLContext.hpp"

#include <QOpenGLContext>
#include <QSurface>
#include <QDebug>



GLContext::GLContext(QOpenGLContext* ctx, QSurface *surf)
    : mGLCtx(ctx)
    , mSurface(surf)
{

}

GLContext::GLContext(const GLContext &other)
    : mGLCtx(other.mGLCtx)
    , mSurface(other.mSurface)
{

}

GLContext::~GLContext()
{
    mGLCtx=nullptr;
    mSurface=nullptr;
}


void GLContext::setSurface(QSurface &surf)
{
    this->mSurface=&surf;
}

QSurface *GLContext::surface()
{
    return mSurface;
}

QOpenGLContext *GLContext::context()
{
    return mGLCtx;
}

/*

  From http://doc.qt.io/qt-5/qopenglcontext.html#makeCurrent

Do not call this function from a different thread than the one the QOpenGLContext
instance lives in. If you wish to use QOpenGLContext from a different thread you
should first make sure it's not current in the current thread, by calling
doneCurrent() if necessary. Then call moveToThread(otherThread) before using it
in the other thread.
 */


bool GLContext::setCurrent(bool current)
{
    qWarning()<<"GLCTX Setting to "<<(current?"CURRENT":"UN-CURRENT");
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
#define SURFACE_CLASS_TO_STRING_STANZA(A)	case(QSurface::A):return #A

    switch(c) {
        SURFACE_CLASS_TO_STRING_STANZA(Window);
        SURFACE_CLASS_TO_STRING_STANZA(Offscreen);
    default:
        return "UNKNOWN";
    }
#undef SURFACE_CLASS_TO_STRING_STANZA
}


static QString surfaceTypeToString(QSurface::SurfaceType c)
{
#define SURFACE_TYPE_TO_STRING_STANZA(A)	case(QSurface::A):return #A

    switch(c) {
        SURFACE_TYPE_TO_STRING_STANZA(RasterSurface);
        SURFACE_TYPE_TO_STRING_STANZA(OpenGLSurface);
        SURFACE_TYPE_TO_STRING_STANZA(RasterGLSurface);
    default:
        return "UNKNOWN";
    }
#undef SURFACE_TYPE_TO_STRING_STANZA
}


QString GLContext::toString()
{
    QString s;
    QDebug d= QDebug(&s).noquote().nospace();
    d << "GLCTX{ oglctx=";
    if(nullptr==mGLCtx) {
        d<<"NULL";
    } else {
        d << "QOpenGLContext(currentContext=" << mGLCtx->currentContext() << ", isOpenGLES=" << mGLCtx->isOpenGLES() << ", isValid=" << mGLCtx->isValid() << ", isWindowType=" << mGLCtx->isWindowType() << ")";
        //<<", format="<<mGLCtx->format()	TOO VERBOSE
    }
    d<< ", surf=";
    if(nullptr==mSurface) {
        d<<"NULL";
    } else {
        d<<"QSurface( size="<<mSurface->size()<<", surfaceType="<<surfaceTypeToString(mSurface->surfaceType())<<", surfaceClass="<<surfaceClassToString(mSurface->surfaceClass())<<", supportsOpenGL="<<mSurface->supportsOpenGL()<<")";
        //<<mSurface->format() TOOVERBOSE
    }
    d << " }";
    return s;
}
