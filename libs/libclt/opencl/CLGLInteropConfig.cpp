#include "CLGLInteropConfig.hpp"


CLGLInteropConfig::CLGLInteropConfig(GLContext *sc, QSize sz)
	: mSharingContext(sc)
	, mSize(sz)
{

}

CLGLInteropConfig::CLGLInteropConfig(CLGLInteropConfig &other)
	: mSharingContext(other.mSharingContext)
	, mSize(other.mSize)
{

}

CLGLInteropConfig::CLGLInteropConfig(const CLGLInteropConfig &other)
	: mSharingContext(other.mSharingContext)
	, mSize(other.mSize)
{

}

CLGLInteropConfig::~CLGLInteropConfig()
{

}


bool CLGLInteropConfig::doGLInterop() const
{
	return (nullptr!=mSharingContext);
}

GLuint CLGLInteropConfig::width() const
{
	return mSize.width();
}

GLuint CLGLInteropConfig::height() const
{
	return mSize.height();
}


QSize CLGLInteropConfig::size() const
{
	return mSize;
}

GLContext *CLGLInteropConfig::sharingContext() const
{
	return mSharingContext;
}
