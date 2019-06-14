#include "CLGLInteropConfig.hpp"

#include "uptime/MethodGate.hpp"

CLGLInteropConfig::CLGLInteropConfig(GLContext *sc, QSize sz)
	: mSharingContext(sc)
	, mSize(sz)
{
	OC_METHODGATE();
	if(nullptr== mSharingContext) {
		qWarning()<<"WARNING: gl context was null";
	}
}


CLGLInteropConfig::CLGLInteropConfig(CLGLInteropConfig &other)
	: mSharingContext(other.mSharingContext)
	, mSize(other.mSize)
{
	if(nullptr== mSharingContext) {
		qWarning()<<"WARNING: gl context was null";
	}
	OC_METHODGATE();
}


CLGLInteropConfig::CLGLInteropConfig(const CLGLInteropConfig &other)
	: mSharingContext(other.mSharingContext)
	, mSize(other.mSize)
{
	if(nullptr== mSharingContext) {
		qWarning()<<"WARNING: gl context was null";
	}
	OC_METHODGATE();
}


CLGLInteropConfig::~CLGLInteropConfig()
{
	OC_METHODGATE();
}


bool CLGLInteropConfig::doGLInterop() const
{
	OC_METHODGATE();
	return (nullptr!=mSharingContext);
}


GLuint CLGLInteropConfig::width() const
{
	OC_METHODGATE();
	return static_cast<GLuint>(mSize.width());
}


GLuint CLGLInteropConfig::height() const
{
	OC_METHODGATE();
	return static_cast<GLuint>(mSize.height());
}


QSize CLGLInteropConfig::size() const
{
	OC_METHODGATE();
	return mSize;
}


GLContext *CLGLInteropConfig::sharingContext() const
{
	OC_METHODGATE();
	return mSharingContext;
}
